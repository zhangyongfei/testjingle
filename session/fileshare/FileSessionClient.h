#ifndef FILE_SESSION_CLIENT
#define FILE_SESSION_CLIENT
#include "p2p/base/parsing.h"
#include "p2p/base/sessionclient.h"
#include "p2p/client/sessionmanagertask.h"
#include "p2p/base/session.h"

namespace cricket {

class FilePump;

class FileSessionClient : public SessionClient, public sigslot::has_slots<>
{
public:
	FileSessionClient(const buzz::Jid& jid, SessionManager *manager);
	~FileSessionClient();

	FilePump *CreatePump();
	
	const SessionManager& session_manager() { return session_manager_; }

	SessionDescription* CreateOffer(const CallOptions& options) {
		return NULL;
	}

	Session *CreateSession(FilePump *call);
	Session *CreateSession(const std::string& id, FilePump* call);

	sigslot::signal1<FilePump *> SignalPumpCreate;
    sigslot::signal1<FilePump *> SignalPumpDestroy;

protected:
	void Construct();

	void OnSessionCreate(Session *session,
                         bool received_initiate);

	void OnSessionState(BaseSession* base_session,
                        BaseSession::State state);

	void OnSessionDestroy(Session *session);

	bool IsWritable(SignalingProtocol protocol,
		const ContentDescription* content);

	bool WriteContent(SignalingProtocol protocol,
		const ContentDescription* content,
		buzz::XmlElement** elem,
		WriteError* error);

	bool ParseContent(SignalingProtocol protocol,
		const buzz::XmlElement* elem,
		ContentDescription** content,
		ParseError* error);

private:
	SessionManager* session_manager_;
	buzz::Jid id_;
};

}

#endif