#ifndef _JINGLECLIENT_
#define _JINGLECLIENT_
#include "xmpp/xmppengine.h"
#include "base/sigslot.h"
#include "xmpp/xmppclient.h"
#include "xmpp/rostermodule.h"
#include "test/console.h"
#include "xmpp/chattask.h"
#include "p2p/client/sessionmanagertask.h"
#include "media/base/mediaengine.h"
#include "session/media/mediasessionclient.h"
#include "session/fileshare/FileSessionClient.h"


namespace buzz
{
class PingTask;
}

class JingleClient : public sigslot::has_slots<>,
	                 public buzz::XmppRosterHandler,
                     public ConsoleClient
{
public:
	JingleClient(buzz::XmppClient *xmpp_client);
	virtual ~JingleClient();

	void OnStateChange(buzz::XmppEngine::State state);

	void ParseLine(const std::string& line);

	void SetConsole(Console *console) {
		console_ = console;
	}

	void SetPortAllocatorFlags(uint32 flags) { portallocator_flags_ = flags; }
	void SetAllowLocalIps(bool allow_local_ips) {
		allow_local_ips_ = allow_local_ips;
	}

	void SetSignalingProtocol(cricket::SignalingProtocol protocol) {
		signaling_protocol_ = protocol;
	}
	void SetTransportProtocol(cricket::TransportProtocol protocol) {
		transport_protocol_ = protocol;
	}
	void SetSecurePolicy(cricket::SecurePolicy sdes_policy,
		cricket::SecurePolicy dtls_policy) {
			sdes_policy_ = sdes_policy;
			dtls_policy_ = dtls_policy;
	}
	void SetSslIdentity(talk_base::SSLIdentity* identity) {
		ssl_identity_.reset(identity);
	}

	void SetShowRosterMessages(bool show_roster_messages) {
		show_roster_messages_ = show_roster_messages;
	}

	void SetDataChannelType(cricket::DataChannelType data_channel_type) {
		data_channel_type_ = data_channel_type;
	}

	void SetMultiSessionEnabled(bool multisession_enabled) {
		multisession_enabled_ = multisession_enabled;
	}

	void SetAutoAccept(bool auto_accept) {
		auto_accept_ = auto_accept;
	}

protected:
	void InitP2P();

	void InitPresence();

	void StartXmppPing();

	void OnPingTimeout();

	bool PlaceCall(const std::string& name, cricket::CallOptions options);

	bool PlacePump(const std::string& name, const std::string& filename);

	void Accept(const cricket::CallOptions& options);

	void SetupAcceptedCall();

	void OnDataReceived(cricket::Call*,
		const cricket::ReceiveDataParams& params,
		const talk_base::Buffer& payload);

	void OnSpeakerChanged(cricket::Call* call,
		cricket::Session* session,
		const cricket::StreamParams& speaker_stream);

	void TerminateAndRemoveSession(cricket::Call* call, 
		const std::string& id);

	void SendChat(const buzz::Jid& to, const std::string& msg);

	void RecvChat(const buzz::Jid& from, const std::string& msg);

	void OnFileCreate(cricket::FilePump* call);

	void OnFileDestroy(cricket::FilePump* call);

	void OnSessionState(cricket::FilePump* call,
		cricket::Session* session,
		cricket::Session::State state);

	void OnCallCreate(cricket::Call* call);

	void OnSessionState(cricket::Call* call,
		cricket::Session* session,
		cricket::Session::State state);

	void OnMediaStreamsUpdate(cricket::Call* call,
		cricket::Session* session,
		const cricket::MediaStreams& added,
		const cricket::MediaStreams& removed);

	void OnCallDestroy(cricket::Call* call);

	void OnDevicesChange();

	void OnRequestSignaling();

	void OnSessionCreate(cricket::Session* session, bool initiate);

	void SubscriptionRequest(buzz::XmppRosterModule* roster,
		const buzz::Jid& requesting_jid,
		buzz::XmppSubscriptionRequestType type,
		const buzz::XmlElement* raw_xml);

	void SubscriptionError(buzz::XmppRosterModule* roster,
		const buzz::Jid& from,
		const buzz::XmlElement* raw_xml);

	void RosterError(buzz::XmppRosterModule* roster,
		const buzz::XmlElement* raw_xml);

	void IncomingPresenceChanged(buzz::XmppRosterModule* roster,
		const buzz::XmppPresence* presence);

	void ContactChanged(buzz::XmppRosterModule* roster,
		const buzz::XmppRosterContact* old_contact,
		size_t index);

	void ContactsAdded(buzz::XmppRosterModule* roster,
		size_t index, size_t number);

	void ContactRemoved(buzz::XmppRosterModule* roster,
		const buzz::XmppRosterContact* removed_contact,
		size_t index);

	cricket::Session* GetFirstSession(uint32 id) { return sessions_[id][0]; } //call_->id()
	void AddSession(uint32 id, cricket::Session* session) {
		sessions_[id].push_back(session); //call_->id()
	}

private:
	buzz::XmppClient *xmpp_client_;
	buzz::XmppRosterModule *roster_module_;
	talk_base::scoped_ptr<buzz::PingTask> ping_task_;
	talk_base::scoped_ptr<buzz::ChatTask> chat_task_;

	talk_base::Thread* worker_thread_;
	talk_base::NetworkManager* network_manager_;
	cricket::PortAllocator* port_allocator_;
	cricket::SessionManager* session_manager_;
	cricket::SessionManagerTask* session_manager_task_;

	cricket::MediaEngineInterface* media_engine_;
	cricket::DataEngineInterface* data_engine_;

	cricket::MediaSessionClient* media_client_;
	cricket::FileSessionClient* file_client_;

	uint32 portallocator_flags_;

	bool allow_local_ips_;

	cricket::DataChannelType data_channel_type_;
	cricket::SignalingProtocol signaling_protocol_;
	cricket::TransportProtocol transport_protocol_;
	cricket::SecurePolicy sdes_policy_;
	cricket::SecurePolicy dtls_policy_;
	talk_base::scoped_ptr<talk_base::SSLIdentity> ssl_identity_;
	bool multisession_enabled_;
	std::string last_sent_to_;

	bool show_roster_messages_;

	bool incoming_call_;
	bool auto_accept_;

	Console *console_;
	cricket::Call* call_;
	cricket::FilePump* pump_;
	typedef std::map<uint32, std::vector<cricket::Session *> > SessionMap;
	SessionMap sessions_;
};

#define OPENFILEADDR "10.192.1.192"

#endif