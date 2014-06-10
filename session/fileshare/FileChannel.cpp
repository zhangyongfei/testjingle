#include "FileChannel.h"

#include "base/bind.h"
#include "p2p/base/session.h"
#include "p2p/base/transportchannel.h"

namespace cricket
{
	FileChannel::FileChannel(
		talk_base::Thread* worker_thread, 
		BaseSession* session, 
		const std::string& content_name)
		: worker_thread_(worker_thread),
        session_(session),
		content_name_(content_name),
		sctp_channel_(NULL),
		writable_(false),
		was_ever_writable_(false)
	{

	}

	FileChannel::~FileChannel()
	{

	}

	bool FileChannel::Init()
	{
		InvokeOnWorker(talk_base::Bind(&FileChannel::CreateChannel_W, this));

		return true;
	}

	bool FileChannel::CreateChannel_W()
	{
		sctp_channel_ = session_->CreateChannel(content_name_, 
			"sctp", 
			ICE_CANDIDATE_COMPONENT_DEFAULT);

		sctp_channel_->SignalWritableState.connect(
			this, &FileChannel::OnWritableState);
		sctp_channel_->SignalReadPacket.connect(
			this, &FileChannel::OnChannelRead);
		sctp_channel_->SignalReadyToSend.connect(
			this, &FileChannel::OnReadyToSend);

		return true;
	}

	void FileChannel::OnWritableState(TransportChannel* channel)
	{
		ASSERT(channel == sctp_channel_);
		if (sctp_channel_->writable()) {
				ChannelWritable_w();
		} else {
			ChannelNotWritable_w();
		}
	}

	void FileChannel::ChannelWritable_w()
	{
		ASSERT(worker_thread_ == talk_base::Thread::Current());
		if (writable_)
			return;

		LOG(LS_INFO) << "Channel socket writable ("
			<< sctp_channel_->content_name() << ", "
			<< sctp_channel_->component() << ")"
			<< (was_ever_writable_ ? "" : " for the first time");

		std::vector<ConnectionInfo> infos;
		sctp_channel_->GetStats(&infos);
		for (std::vector<ConnectionInfo>::const_iterator it = infos.begin();
			it != infos.end(); ++it) {
				if (it->best_connection) {
					LOG(LS_INFO) << "Using " << it->local_candidate.ToSensitiveString()
						<< "->" << it->remote_candidate.ToSensitiveString();
					break;
				}
		}

		was_ever_writable_ = true;
		writable_ = true;
		ChangeState();
	}

	void FileChannel::ChangeState() 
	{
		
	}

	void FileChannel::ChannelNotWritable_w() 
	{
		ASSERT(worker_thread_ == talk_base::Thread::Current());
		if (!writable_)
			return;

		LOG(LS_INFO) << "Channel socket not writable ("
			<< sctp_channel_->content_name() << ", "
			<< sctp_channel_->component() << ")";
		writable_ = false;
		ChangeState();
	}

	void FileChannel::OnChannelRead(TransportChannel* channel,
		const char* data, size_t len,
		const talk_base::PacketTime& packet_time,
		int flags) 
	{
			// OnChannelRead gets called from P2PSocket; now pass data to MediaEngine
			ASSERT(worker_thread_ == talk_base::Thread::Current());

			// When using RTCP multiplexing we might get RTCP packets on the RTP
			// transport. We feed RTP traffic into the demuxer to determine if it is RTCP.
			//bool rtcp = PacketIsRtcp(channel, data, len);
			//talk_base::Buffer packet(data, len);
			//HandlePacket(rtcp, &packet, packet_time);
	}

	void FileChannel::OnReadyToSend(TransportChannel* channel) 
	{
		SetReadyToSend(channel, true);
	}

	void FileChannel::SetReadyToSend(TransportChannel* channel, bool ready) 
	{
		ASSERT(channel == sctp_channel_);
		/*if (channel == transport_channel_) {
			rtp_ready_to_send_ = ready;
		}
		if (channel == rtcp_transport_channel_) {
			rtcp_ready_to_send_ = ready;
		}

		if (!ready) {
			// Notify the MediaChannel when either rtp or rtcp channel can't send.
			media_channel_->OnReadyToSend(false);
		} else if (rtp_ready_to_send_ &&
			// In the case of rtcp mux |rtcp_transport_channel_| will be null.
			(rtcp_ready_to_send_ || !rtcp_transport_channel_)) {
				// Notify the MediaChannel when both rtp and rtcp channel can send.
				media_channel_->OnReadyToSend(true);
		}*/
	}
}