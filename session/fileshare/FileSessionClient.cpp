#include "FileSessionClient.h"
#include "FilePump.h"

namespace cricket {

const std::string NS_GOOGLE_SHARE("urn:xmpp:jingle:apps:file-transfer:3");

const buzz::QName QN_SHARE_DESCRIPTION(NS_GOOGLE_SHARE, "description");
const buzz::QName QN_SHARE_FOLDER(NS_GOOGLE_SHARE, "folder");
const buzz::QName QN_SHARE_OFFER(NS_GOOGLE_SHARE, "offer");
const buzz::QName QN_SHARE_FILE(NS_GOOGLE_SHARE, "file");
const buzz::QName QN_SHARE_NAME(NS_GOOGLE_SHARE, "name");
const buzz::QName QN_SHARE_SIZE(NS_GOOGLE_SHARE, "size");
const buzz::QName QN_SHARE_IMAGE(NS_GOOGLE_SHARE, "image");
const buzz::QName QN_SHARE_PROTOCOL(NS_GOOGLE_SHARE, "protocol");
const buzz::QName QN_SHARE_HTTP(NS_GOOGLE_SHARE, "http");
const buzz::QName QN_SHARE_URL(NS_GOOGLE_SHARE, "url");
const buzz::QName QN_SHARE_CHANNEL(NS_GOOGLE_SHARE, "channel");
const buzz::QName QN_SHARE_COMPLETE(NS_GOOGLE_SHARE, "complete");

FileSessionClient::FileSessionClient(
	const buzz::Jid& jid, 
	SessionManager *manager)
     : session_manager_(manager),
	 jid_(jid)
{
    Construct();
}

FileSessionClient::~FileSessionClient()
{

}

void FileSessionClient::Construct()
{
	session_manager_->AddClient(NS_GOOGLE_SHARE, this);
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

SessionDescription* FileSessionClient::CreateAnswer(const SessionDescription* offer,
								 const FileOption& options)
{
	SessionDescription* sdesc = offer->Copy();
	return sdesc;
}

void FileSessionClient::OnSessionState(BaseSession* base_session,
                                        BaseSession::State state)
{
    Session* session = static_cast<Session*>(base_session);

	if (state == Session::STATE_RECEIVEDINITIATE) 
	{
		// The creation of the call must happen after the session has
		// processed the initiate message because we need the
		// remote_description to know what content names to use in the
		// call.

		// If our accept would have no codecs, then we must reject this call.
		const SessionDescription* offer = session->remote_description();
		//const SessionDescription* accept = CreateAnswer(offer, FileOption());
		//const ContentInfo* audio_content = GetFirstAudioContent(accept);
		//bool audio_rejected = (!audio_content) ? true : audio_content->rejected;
		//const AudioContentDescription* audio_desc = (!audio_content) ? NULL :
		//	static_cast<const AudioContentDescription*>(audio_content->description);

		// For some reason, we need a call even if we reject. So, either find a
		// matching call or create a new one.
		// The matching of existing calls is used to support the multi-session mode
		// required for p2p handoffs: ie. once a MUC call is established, a new
		// session may be established for the same call but is direct between the
		// clients. To indicate that this is the case, the initiator of the incoming
		// session is set to be the same as the remote name of the MUC for the
		// existing session, thus the client can know that this is a new session for
		// the existing call, rather than a whole new call.
		FilePump* pump = NULL;
		

		if (pump == NULL) {
			// Could not find a matching call, so create a new one.
			pump = CreatePump();
		}

		session_map_[session->id()] = pump;
		pump->IncomingSession(session, offer);

// 		if (audio_rejected || !audio_desc || audio_desc->codecs().size() == 0) {
// 			session->Reject(STR_TERMINATE_INCOMPATIBLE_PARAMETERS);
// 		}

//		delete accept;
	}
}

void FileSessionClient::OnSessionDestroy(Session *session) 
{

}

bool FileSessionClient::IsWritable(SignalingProtocol protocol,
				const ContentDescription* content) 
{
	return true;
}

bool FileSessionClient::WriteContent(SignalingProtocol protocol,
				  const ContentDescription* content,
				  buzz::XmlElement** elem,
				  WriteError* error)
{
	const FileDescription *fdesc = reinterpret_cast<const FileDescription *>(content);
	*elem = new buzz::XmlElement(QN_SHARE_DESCRIPTION);

	buzz::XmlElement *desdesc = new buzz::XmlElement(QN_SHARE_DESCRIPTION);
	desdesc->AddText("This is a test. If this were a real file...");

	buzz::XmlElement *desname = new buzz::XmlElement(QN_SHARE_NAME);
	desname->AddText(fdesc->option().file_name().filename());

	buzz::XmlElement *dessize = new buzz::XmlElement(QN_SHARE_SIZE);
	char buff[1024] = {0};
	uint64 size = fdesc->option().file_size();
#ifdef WIN32
	sprintf_s(buff, sizeof(buff) - 1, "%lu", size);
#else
	snprintf(buff, sizeof(buff) - 1, "%lu", size);
#endif
	dessize->AddText(buff);

	buzz::XmlElement *desfile = new buzz::XmlElement(QN_SHARE_FILE);
	desfile->AddElement(desdesc);
	desfile->AddElement(desname);
	desfile->AddElement(dessize);

	buzz::XmlElement *desoffer = new buzz::XmlElement(QN_SHARE_OFFER);
	desoffer->AddElement(desfile);

	(*elem)->AddElement(desoffer);

    return true;
}

bool FileSessionClient::ParseContent(SignalingProtocol protocol,
				  const buzz::XmlElement* elem,
				  ContentDescription** content,
				  ParseError* error)
{
	FileDescription *tmp = new FileDescription();

	const buzz::XmlElement *desoffer = elem->FirstNamed(QN_SHARE_OFFER);

	const buzz::XmlElement *desfile = desoffer->FirstNamed(QN_SHARE_FILE);

	const buzz::XmlElement *desname = desfile->FirstNamed(QN_SHARE_NAME);

	FileOption option;

	option.set_file_name(desname->BodyText());

	const buzz::XmlElement *dessize = desfile->FirstNamed(QN_SHARE_SIZE);

	option.set_file_size(atoi(dessize->BodyText().c_str()));

	tmp->set_option(option);

	*content = tmp->Copy();

    return true;
}

SessionDescription* FileSessionClient::CreateOffer(const FileOption& options)
{
	SessionDescription* sdesc = new SessionDescription();

	FileDescription *desc = new FileDescription();

	desc->set_option(options);

	sdesc->AddContent("file", NS_GOOGLE_SHARE, desc);

	return sdesc;
}

Session *FileSessionClient::CreateSession(FilePump *pump)
{
	std::string id;
	return CreateSession(id, pump);
}

Session *FileSessionClient::CreateSession(const std::string& id, FilePump* pump)
{
	const std::string& type = NS_GOOGLE_SHARE;
	Session *session = session_manager_->CreateSession(id, jid().Str(), type);
	session_map_[session->id()] = pump;
	return session;
}

}