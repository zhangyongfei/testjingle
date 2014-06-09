#include "jingleclient.h"
#include <iostream>
#include "base/stringencode.h"
#include "xmpp/pingtask.h"
#include "xmpp/constants.h"
#include "p2p/client/basicportallocator.h"
#include "p2p/base/sessionmanager.h"
#include "p2p/base/session.h"
#include "media/base/rtpdataengine.h"
#include "media/sctp/sctpdataengine.h"

// Must be period >= timeout.
const uint32 kPingPeriodMillis = 10000;
const uint32 kPingTimeoutMillis = 10000;

JingleClient::JingleClient(buzz::XmppClient *xmpp_client) :
	xmpp_client_(xmpp_client),
	call_(NULL),
	media_client_(NULL),
	media_engine_(NULL),
	data_engine_(NULL)
{

}

JingleClient::~JingleClient()
{

}

void JingleClient::OnStateChange(buzz::XmppEngine::State state)
{
	switch (state) {
	case buzz::XmppEngine::STATE_START:
		std::cout << "Connecting..." << std::endl;
		break;
	case buzz::XmppEngine::STATE_OPENING:
		std::cout << "Logging in. " << std::endl;
		break;
	case buzz::XmppEngine::STATE_OPEN:
		std::cout << "Logged in as " << xmpp_client_->jid().Str() << std::endl;
		InitP2P();
		InitPresence();
		break;
	case buzz::XmppEngine::STATE_CLOSED:
		std::cout << "Logged out." << std::endl;
		break;
	}
}

void JingleClient::StartXmppPing() 
{
	buzz::PingTask* ping = new buzz::PingTask(
		xmpp_client_, talk_base::Thread::Current(),
		kPingPeriodMillis, kPingTimeoutMillis);
	ping_task_.reset(ping);
	ping->SignalTimeout.connect(this, &JingleClient::OnPingTimeout);
	ping->Start();
}

void JingleClient::OnPingTimeout() 
{
	LOG(LS_WARNING) << "XMPP Ping timeout. Will keep trying...";
	StartXmppPing();
}

void JingleClient::InitPresence()
{
	roster_module_ = buzz::XmppRosterModule::Create();
	roster_module_->set_roster_handler(this);
	roster_module_->RegisterEngine(xmpp_client_->engine());
	roster_module_->BroadcastPresence();
	roster_module_->RequestRosterUpdate();

	chat_task_.reset(new buzz::ChatTask(xmpp_client_));
	chat_task_->MessageRecv.connect(this, &JingleClient::RecvChat);
	chat_task_->Start();

	// StartXmppPing(); 测试时关闭心跳功能
}

void JingleClient::InitP2P()
{
	worker_thread_ = new talk_base::Thread();
	// The worker thread must be started here since initialization of
	// the ChannelManager will generate messages that need to be
	// dispatched by it.
	worker_thread_->Start();

	// TODO: It looks like we are leaking many objects. E.g.
	// |network_manager_| is never deleted.
	network_manager_ = new talk_base::BasicNetworkManager();

	// TODO: Decide if the relay address should be specified here.
	talk_base::SocketAddress stun_addr(OPENFILEADDR, 3478);
	port_allocator_ =  new cricket::BasicPortAllocator(
		network_manager_, stun_addr, talk_base::SocketAddress(OPENFILEADDR, 10000),
		talk_base::SocketAddress(OPENFILEADDR, 10001), talk_base::SocketAddress(OPENFILEADDR, 10002));

	if (portallocator_flags_ != 0) {
		port_allocator_->set_flags(portallocator_flags_);
	}

	session_manager_ = new cricket::SessionManager(
		port_allocator_, worker_thread_);
	session_manager_->set_secure(dtls_policy_);
	session_manager_->set_identity(ssl_identity_.get());
	session_manager_->set_transport_protocol(transport_protocol_);
	session_manager_->SignalRequestSignaling.connect(
		this, &JingleClient::OnRequestSignaling);
	session_manager_->SignalSessionCreate.connect(
		this, &JingleClient::OnSessionCreate);
	session_manager_->OnSignalingReady();

	session_manager_task_ =
		new cricket::SessionManagerTask(xmpp_client_, session_manager_);
	session_manager_task_->EnableOutgoingMessages();
	session_manager_task_->Start();

	if (!media_engine_) {
		media_engine_ = cricket::MediaEngineFactory::Create();
	}

	if (!data_engine_) {
		if (data_channel_type_ == cricket::DCT_SCTP) {
#ifdef HAVE_SCTP
			data_engine_ = new cricket::SctpDataEngine();
#else
			LOG(LS_WARNING) << "SCTP Data Engine not supported.";
			data_channel_type_ = cricket::DCT_NONE;
			data_engine_ = new cricket::RtpDataEngine();
#endif
		} else {
			// Even if we have DCT_NONE, we still have a data engine, just
			// to make sure it isn't NULL.
			data_engine_ = new cricket::RtpDataEngine();
		}
	}

	media_client_ = new cricket::MediaSessionClient(
		xmpp_client_->jid(),
		session_manager_,
		media_engine_,
		data_engine_,
		cricket::DeviceManagerFactory::Create());
	media_client_->SignalCallCreate.connect(this, &JingleClient::OnCallCreate);
	media_client_->SignalCallDestroy.connect(this, &JingleClient::OnCallDestroy);
	media_client_->SignalDevicesChange.connect(this,
		&JingleClient::OnDevicesChange);
	media_client_->set_secure(sdes_policy_);
	media_client_->set_multisession_enabled(multisession_enabled_);

	// 文件共享
	file_client_ = new cricket::FileSessionClient(
		session_manager_
		);
}

void JingleClient::OnCallDestroy(cricket::Call* call)
{

}

void JingleClient::OnDevicesChange() 
{
	console_->PrintLine("Devices changed.");
	//SetMediaCaps(media_client_->GetCapabilities(), &my_status_);
	//SendStatus(my_status_);
}

void JingleClient::OnCallCreate(cricket::Call* call) {
	call->SignalSessionState.connect(this, &JingleClient::OnSessionState);
	call->SignalMediaStreamsUpdate.connect(
		this, &JingleClient::OnMediaStreamsUpdate);
}

void JingleClient::OnFileCreate(cricket::FilePump* call)
{

}

void JingleClient::OnFileDestroy(cricket::FilePump* call)
{
}

void JingleClient::OnMediaStreamsUpdate(cricket::Call* call,
	cricket::Session* session,
	const cricket::MediaStreams& added,
	const cricket::MediaStreams& removed) {

}

void JingleClient::OnSessionState(cricket::Call* call,
	cricket::Session* session,
	cricket::Session::State state) {
		if (state == cricket::Session::STATE_RECEIVEDINITIATE) {
			buzz::Jid jid(session->remote_name());
			if (call_ == call && multisession_enabled_) {
				// We've received an initiate for an existing call. This is actually a
				// new session for that call.
				console_->PrintLine("Incoming session from '%s'", jid.Str().c_str());
				AddSession(call_->id(), session);

				cricket::CallOptions options;
				options.has_video = call_->has_video();
				options.data_channel_type = data_channel_type_;
				call_->AcceptSession(session, options);
			} else {
				console_->PrintLine("Incoming call from '%s'", jid.Str().c_str());
				call_ = call;
				AddSession(call_->id(), session);
				incoming_call_ = true;
				
				if (auto_accept_) {
					cricket::CallOptions options;
					options.has_video = false;
					options.data_channel_type = data_channel_type_;
					Accept(options);
				}
			}
		} else if (state == cricket::Session::STATE_SENTINITIATE) {
			console_->PrintLine("calling...");
		} else if (state == cricket::Session::STATE_RECEIVEDACCEPT) {
			console_->PrintLine("call answered");
			SetupAcceptedCall();
		} else if (state == cricket::Session::STATE_RECEIVEDREJECT) {
			console_->PrintLine("call not answered");
		} else if (state == cricket::Session::STATE_INPROGRESS) {
			console_->PrintLine("call in progress");
			call->SignalSpeakerMonitor.connect(this, &JingleClient::OnSpeakerChanged);
			call->StartSpeakerMonitor(session);
		} else if (state == cricket::Session::STATE_RECEIVEDTERMINATE) {
			console_->PrintLine("other side terminated");
			TerminateAndRemoveSession(call, session->id());
		}
}

void JingleClient::OnSpeakerChanged(cricket::Call* call,
					  cricket::Session* session,
					  const cricket::StreamParams& speaker)
{
	if (!speaker.has_ssrcs()) {
		console_->PrintLine("Session %s has no current speaker.",
			session->id().c_str());
	} else if (speaker.id.empty()) {
		console_->PrintLine("Session %s speaker change to unknown (%u).",
			session->id().c_str(), speaker.first_ssrc());
	} else {
		console_->PrintLine("Session %s speaker changed to %s (%u).",
			session->id().c_str(), speaker.id.c_str(),
			speaker.first_ssrc());
	}
}

void JingleClient::TerminateAndRemoveSession(cricket::Call* call, 
							   const std::string& id)
{

	std::vector<cricket::Session*>& call_sessions = sessions_[call->id()];
	for (std::vector<cricket::Session*>::iterator iter = call_sessions.begin();
		iter != call_sessions.end(); ++iter) {
			if ((*iter)->id() == id) {
				call_->TerminateSession(*iter);
				call_sessions.erase(iter);
				break;
			}
	}

}

void JingleClient::Accept(const cricket::CallOptions& options)
{
	ASSERT(call_ && incoming_call_);
	ASSERT(sessions_[call_->id()].size() == 1);
	cricket::Session* session = GetFirstSession(call_->id());
	call_->AcceptSession(session, options);
	media_client_->SetFocus(call_);
	SetupAcceptedCall();
	incoming_call_ = false;
}

void JingleClient::SetupAcceptedCall()
{
	if (call_->has_data()) {
		call_->SignalDataReceived.connect(this, &JingleClient::OnDataReceived);
	}
}

void JingleClient::OnDataReceived(cricket::Call*,
					const cricket::ReceiveDataParams& params,
					const talk_base::Buffer& payload)
{
	// TODO(mylesj): Support receiving data on sessions other than the first.
	cricket::Session* session = GetFirstSession(call_->id());
	if (!session)
		return;

	cricket::StreamParams stream;
	const std::vector<cricket::StreamParams>* data_streams =
		call_->GetDataRecvStreams(session);
	std::string text(payload.data(), payload.length());
	if (data_streams && GetStreamBySsrc(*data_streams, params.ssrc, &stream)) {
		console_->PrintLine(
			"Received data from '%s' on stream '%s' (ssrc=%u): %s",
			stream.groupid.c_str(), stream.id.c_str(),
			params.ssrc, text.c_str());
	} else {
		console_->PrintLine(
			"Received data (ssrc=%u): %s",
			params.ssrc, text.c_str());
	}
}

void JingleClient::OnRequestSignaling() 
{
	session_manager_->OnSignalingReady();
}

void JingleClient::OnSessionCreate(cricket::Session* session, bool initiate) 
{
	session->set_current_protocol(signaling_protocol_);
}

void JingleClient::SubscriptionRequest(buzz::XmppRosterModule* roster,
									   const buzz::Jid& requesting_jid,
									   buzz::XmppSubscriptionRequestType type,
									   const buzz::XmlElement* raw_xml)
{
	roster_module_->ApproveSubscriber(requesting_jid);
}

void JingleClient::SubscriptionError(buzz::XmppRosterModule* roster,
									 const buzz::Jid& from,
									 const buzz::XmlElement* raw_xml)
{
	printf("SubscriptionError\n");
}

void JingleClient::RosterError(buzz::XmppRosterModule* roster,
							   const buzz::XmlElement* raw_xml)
{
	printf("RosterError\n");
}

void JingleClient::IncomingPresenceChanged(buzz::XmppRosterModule* roster,
										   const buzz::XmppPresence* presence)
{
	printf("IncomingPresenceChanged\n");
}

void JingleClient::ContactChanged(buzz::XmppRosterModule* roster,
								  const buzz::XmppRosterContact* old_contact,
								  size_t index)
{
	printf("ContactChanged\n");
}

void JingleClient::ContactsAdded(buzz::XmppRosterModule* roster,
								 size_t index, size_t number)
{
	printf("ContactsAdded\n");
}

void JingleClient::ContactRemoved(buzz::XmppRosterModule* roster,
								  const buzz::XmppRosterContact* removed_contact,
								  size_t index)
{
	printf("ContactRemoved\n");
}

void JingleClient::SendChat(const buzz::Jid& to, const std::string& msg) 
{
	chat_task_->Send(to, msg);
}

void JingleClient::RecvChat(const buzz::Jid& from, const std::string& msg)
{
	printf("Recv From[%s]: %s\n", from.BareJid().Str().c_str(), msg.c_str());
}

// TODO: Make present and record really work.
const char* HANGOUT_COMMANDS =
	"Available MUC commands:\n"
	"\n"
	"  present    Starts presenting (just signalling; not actually presenting.)\n"
	"  unpresent  Stops presenting (just signalling; not actually presenting.)\n"
	"  record     Starts recording (just signalling; not actually recording.)\n"
	"  unrecord   Stops recording (just signalling; not actually recording.)\n"
	"  rmute [nick] Remote mute another participant.\n"
	"  block [nick] Block another participant.\n"
	"  screencast [fps] Starts screencast. \n"
	"  unscreencast Stops screencast. \n"
	"  quit       Quits the application.\n"
	"";

const char* RECEIVE_COMMANDS =
	"Available commands:\n"
	"\n"
	"  accept [bw] Accepts the incoming call and switches to it.\n"
	"  reject  Rejects the incoming call and stays with the current call.\n"
	"  quit    Quits the application.\n"
	"";

const char* CONSOLE_COMMANDS =
	"Available commands:\n"
	"\n"
	"  roster              Prints the online friends from your roster.\n"
	"  friend user         Request to add a user to your roster.\n"
	"  call [jid] [bw]     Initiates a call to the user[/room] with the\n"
	"                      given JID and with optional bandwidth.\n"
	"  vcall [jid] [bw]    Initiates a video call to the user[/room] with\n"
	"                      the given JID and with optional bandwidth.\n"
	"  calls               Lists the current calls\n"
	"  switch [call_id]    Switch to the specified call\n"
	"  join [room_jid]     Joins a multi-user-chat with room JID.\n"
	"  ljoin [room_name]   Joins a MUC by looking up JID from room name.\n"
	"  invite user [room]  Invites a friend to a multi-user-chat.\n"
	"  leave [room]        Leaves a multi-user-chat.\n"
	"  nick [nick]         Sets the nick.\n"
	"  priority [int]      Sets the priority.\n"
	"  getdevs             Prints the available media devices.\n"
	"  quit                Quits the application.\n"
	"";

static int GetInt(const std::vector<std::string>& words, size_t index, int def) 
{
	int val;
	if (words.size() > index && talk_base::FromString(words[index], &val)) {
		return val;
	} else {
		return def;
	}
}

static std::string GetWord(const std::vector<std::string>& words,
						   size_t index, const std::string& def) 
{
	if (words.size() > index) {
		return words[index];
	} else {
		return def;
	}
}

void JingleClient::ParseLine(const std::string& line)
{
	std::vector<std::string> words;
	int start = -1;
	int state = 0;
	for (int index = 0; index <= static_cast<int>(line.size()); ++index) {
		if (state == 0) {
			if (!isspace(line[index])) {
				start = index;
				state = 1;
			}
		} else {
			ASSERT(state == 1);
			ASSERT(start >= 0);
			if (isspace(line[index])) {
				std::string word(line, start, index - start);
				words.push_back(word);
				start = -1;
				state = 0;
			}
		}
	}

	// Global commands
	const std::string& command = GetWord(words, 0, "");
	if (command == "quit") 
	{
		talk_base::Thread *thread = talk_base::ThreadManager::Instance()->CurrentThread();
		thread->Stop();
	}
	else if (command == "presence") 
	{
		int cnt = roster_module_->GetIncomingPresenceCount();

		if(cnt == 0)
		{
			printf("---Thers aren't presences.\n");
		}

		bool bonline = false;

		for (int i = 0; i < cnt; i++)
		{
			const buzz::XmppPresence* contact = roster_module_->GetIncomingPresence(i);

			printf("---presence[%d]:%s\n", i, contact->jid().Str().c_str());
		}
	} 
	else if (command == "roster") 
	{
		int cnt = roster_module_->GetRosterContactCount();

		if(cnt == 0)
		{
			printf("---thers aren't rosters.\n");
		}

		bool bonline = false;

		for (int i = 0; i < cnt; i++)
		{
			const buzz::XmppRosterContact* contact = roster_module_->GetRosterContact(i);

			printf("---roster[%d]:%s\n", i, contact->jid().Str().c_str());
		}
	}
	else if ((words.size() == 2) && (command == "call"))
	{
		std::string to = GetWord(words, 1, "");
		cricket::CallOptions options;
		options.data_channel_type = data_channel_type_;
		if (!PlaceCall(to, options)) {
			console_->PrintLine("Failed to initiate call.");
		}
	} 
	else if ((words.size() == 3) && (command == "msg"))
	{
		SendChat(buzz::Jid(words[1]), words[2]);
	} 
	else if ((words.size() == 2) && (command == "friend"))
	{
		roster_module_->RequestSubscription(buzz::Jid(words[2]));
	} 
}

bool JingleClient::PlaceCall(const std::string& name, cricket::CallOptions options)
{
	buzz::Jid jid(name);

	if (!call_) {
		call_ = media_client_->CreateCall();
		AddSession(call_->id(), call_->InitiateSession(jid, media_client_->jid(), options));
	}

	media_client_->SetFocus(call_);

	return true;
}

bool JingleClient::PlacePump(const std::string& name, const std::string& filename)
{
    buzz::Jid jid(name);

	if (!pump_) {
		pump_ = file_client_->CreatePump();
		AddSession(pump_->id(), pump_->InitiateSession(jid, file_client_->jid(), options));
	}

    return true;
}