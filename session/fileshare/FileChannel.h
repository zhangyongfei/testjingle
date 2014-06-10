#ifndef FILE_CHANNEL
#define FILE_CHANNEL
#include "base/thread.h"
#include "base/asyncpacketsocket.h"

namespace cricket
{
	class BaseSession;
	class TransportChannel;

	class FileChannel : public sigslot::has_slots<>
	{
	public:
		FileChannel(
			talk_base::Thread* worker_thread,
			BaseSession* session,
			const std::string& content_name);

		~FileChannel();

		bool Init();

	protected:
		// Helper function for invoking bool-returning methods on the worker thread.
		template <class FunctorT>
		bool InvokeOnWorker(const FunctorT& functor) {
			return worker_thread_->Invoke<bool>(functor);
		}

		bool CreateChannel_W();

		void OnWritableState(TransportChannel* channel);

		void ChannelWritable_w();

		void ChannelNotWritable_w();

		void ChangeState();

		void OnChannelRead(TransportChannel* channel,
			const char* data, size_t len,
			const talk_base::PacketTime& packet_time,
			int flags);

		void OnReadyToSend(TransportChannel* channel);

		void SetReadyToSend(TransportChannel* channel, bool ready);

	private:
		talk_base::Thread *worker_thread_;
        BaseSession* session_;
		std::string content_name_;
		TransportChannel* sctp_channel_;

		bool was_ever_writable_;
		bool writable_;
	};
}



#endif