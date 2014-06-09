#ifndef FILE_PUMP
#define FILE_PUMP
#include "base/messagequeue.h"
#include "p2p/base/session.h"
#include "session/fileshare/FileSession.h"

namespace cricket {

class FileSessionClient;

class FilePump : public talk_base::MessageHandler, public sigslot::has_slots<>
{
public:
	FilePump(FileSessionClient *session_client);
	~FilePump();

	Session* InitiateSession(const buzz::Jid& to,
		const buzz::Jid& initiator,
		const FileOption& options);

	const uint32 id() { return id_; }

	sigslot::signal2<FilePump*, Session*> SignalAddSession;

	sigslot::signal2<FilePump*, Session*> SignalRemoveSession;

	sigslot::signal3<FilePump*, Session*, Session::State>
		SignalSessionState;

	sigslot::signal3<FilePump*, Session*, Session::Error>
		SignalSessionError;

	sigslot::signal3<FilePump*, Session*, const std::string &>
		SignalReceivedTerminateReason;

	void AcceptSession(Session* session, const FileOption& options);

protected:
	struct FileSession {
		Session* session;
		FileChannel *filechannel;
	};

	// Create a map of media sessions, keyed off session->id().
	typedef std::map<std::string, FileSession> FileSessionMap;
	FileSessionMap file_session_map_;

	bool AddSession(Session* session, const SessionDescription* offer);

	void OnSessionState(BaseSession* base_session, BaseSession::State state);

	void OnSessionError(BaseSession* base_session, Session::Error error);

	void OnSessionInfoMessage(Session* session,
		const buzz::XmlElement* action_elem);

	void OnRemoteDescriptionUpdate(BaseSession* base_session,
		const ContentInfos& updated_contents);

	void OnReceivedTerminateReason(Session* session,
		const std::string& reason);

	void OnMessage(talk_base::Message* msg);

	Session* InternalInitiateSession(const std::string& id,
		const buzz::Jid& to,
		const std::string& initiator_name,
		const FileOption& options);

	void IncomingSession(Session* session, const SessionDescription* offer);

private:
    
	uint32 id_;

    FileSessionClient* session_client_;

	friend class FileSessionClient;
};

}

#endif