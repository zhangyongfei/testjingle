#include "FileSessionClient.h"
#include "FilePump.h"

namespace cricket {

FileSessionClient::FileSessionClient(
	const buzz::Jid& jid, 
	SessionManager *manager)
     : session_manager_(manager)
	 id_(jid)
{
    Construct();
}

FileSessionClient::~FileSessionClient()
{

}

void FileSessionClient::Construct()
{
	// Register ourselves as the handler of audio and video sessions.
	session_manager_->AddClient(NS_JINGLE_RTP, this);
}

FilePump *FileSessionClient::CreatePump()
{
	FilePump *pump = new FilePump(this);

	SignalPumpCreate(pump);
	return pump;
}

void FileSessionClient::OnSessionCreate(Session *session,
                                         bool received_initiate) 
{
  if (received_initiate) {
    session->SignalState.connect(this, &FileSessionClient::OnSessionState);
  }
}

void FileSessionClient::OnSessionState(BaseSession* base_session,
                                        BaseSession::State state)
{
    Session* session = static_cast<Session*>(base_session);

}

void FileSessionClient::OnSessionDestroy(Session *session) 
{

}

bool FileSessionClient::IsWritable(SignalingProtocol protocol,
				const ContentDescription* content) {
					return true;
}

bool FileSessionClient::WriteContent(SignalingProtocol protocol,
				  const ContentDescription* content,
				  buzz::XmlElement** elem,
				  WriteError* error)
{
    return true;
}

bool FileSessionClient::ParseContent(SignalingProtocol protocol,
				  const buzz::XmlElement* elem,
				  ContentDescription** content,
				  ParseError* error)
{
    return true;
}



}