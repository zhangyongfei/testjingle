#ifndef FILE_PUMP
#define FILE_PUMP
#include "base/messagequeue.h"

namespace cricket {

class FileSessionClient;

class FilePump : public talk_base::MessageHandler
{
public:
	FilePump(FileSessionClient *session_client);
	~FilePump();

	Session* InitiateSession(const buzz::Jid& to,
		const buzz::Jid& initiator,
		const CallOptions& options);

protected:
	void OnMessage(talk_base::Message* msg);

	Session* InternalInitiateSession(const std::string& id,
		const buzz::Jid& to,
		const std::string& initiator_name,
		const CallOptions& options);

private:
    
    FileSessionClient* session_client_;

	friend class FileSessionClient;
};

}

#endif