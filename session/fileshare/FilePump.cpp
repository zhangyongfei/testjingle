#include "FilePump.h"
#include "session/fileshare/FileSessionClient.h"

namespace cricket {

	const uint32 MSG_TERMINATEPUMP = 2;
	const int kSendToPumpTimeout = 1000*20;

	FilePump::FilePump(FileSessionClient *session_client)
		: session_client_(session_client),
		id_(talk_base::CreateRandomId())
	{

	}

	FilePump::~FilePump()
	{

	}

	void FilePump::OnMessage(talk_base::Message* msg)
	{

	}

	Session* FilePump::InitiateSession(const buzz::Jid& to,
		const buzz::Jid& initiator,
		const FileOption& options)
	{
		std::string id;
		std::string initiator_name = initiator.Str();
		return InternalInitiateSession(id, to, initiator_name, options);
	}

	Session* FilePump::InternalInitiateSession(const std::string& id,
		const buzz::Jid& to,
		const std::string& initiator_name,
		const FileOption& options) {
			const SessionDescription* offer = session_client_->CreateOffer(options);

			Session* session = session_client_->CreateSession(id, this);
			// Only override the initiator_name if it was manually supplied. Otherwise,
			// session_client_ will supply the local jid as initiator in CreateOffer.
			if (!initiator_name.empty()) {
				session->set_initiator_name(initiator_name);
			}

			AddSession(session, offer);
			session->Initiate(to.Str(), offer);

			// After this timeout, terminate the call because the callee isn't
			// answering
			session_client_->session_manager()->signaling_thread()->Clear(this,
				MSG_TERMINATEPUMP);
			session_client_->session_manager()->signaling_thread()->PostDelayed(
				kSendToPumpTimeout,
				this, MSG_TERMINATEPUMP);
			return session;
	}

	bool FilePump::AddSession(Session* session, const SessionDescription* offer)
	{
		bool succeeded = true;
		FileSession file_session;
		file_session.session = session;

		file_session.filechannel = new FileChannel();
		file_session.filechannel->Init();
		
		if (succeeded) {
			// Add session to list, create channels for this session.
			file_session_map_[session->id()] = file_session;
			session->SignalState.connect(this, &FilePump::OnSessionState);
			session->SignalError.connect(this, &FilePump::OnSessionError);
			session->SignalInfoMessage.connect(
				this, &FilePump::OnSessionInfoMessage);
			session->SignalRemoteDescriptionUpdate.connect(
				this, &FilePump::OnRemoteDescriptionUpdate);
			session->SignalReceivedTerminateReason
				.connect(this, &FilePump::OnReceivedTerminateReason);

			// If this call has the focus, enable this session's channels.
			//if (session_client_->GetFocus() == this) {
			//	EnableSessionChannels(session, true);
			//}

			// Signal client.
			SignalAddSession(this, session);
		}

		return succeeded;
	}

	void FilePump::AcceptSession(Session* session, const FileOption& options)
	{
		FileSessionMap::iterator it = file_session_map_.find(session->id());
		if (it != file_session_map_.end()) {
			const SessionDescription* answer = session_client_->CreateAnswer(
				session->remote_description(), options);
			it->second.session->Accept(answer);
		}
	}

	void FilePump::OnSessionState(BaseSession* base_session, BaseSession::State state) 
	{
		Session* session = static_cast<Session*>(base_session);
		switch (state) {
		case Session::STATE_RECEIVEDACCEPT:
			//UpdateRemoteMediaStreams(session,
			//	session->remote_description()->contents(), false);
			session_client_->session_manager()->signaling_thread()->Clear(this,
				MSG_TERMINATEPUMP);
			break;
		case Session::STATE_RECEIVEDREJECT:
		case Session::STATE_RECEIVEDTERMINATE:
			session_client_->session_manager()->signaling_thread()->Clear(this,
				MSG_TERMINATEPUMP);
			break;
		default:
			break;
		}
		SignalSessionState(this, session, state);
	}

	void FilePump::OnSessionError(BaseSession* base_session, Session::Error error) 
	{
		session_client_->session_manager()->signaling_thread()->Clear(this,
			MSG_TERMINATEPUMP);
		SignalSessionError(this, static_cast<Session*>(base_session), error);
	}

	void FilePump::OnSessionInfoMessage(Session* session,
		const buzz::XmlElement* action_elem) 
	{
			
	}

	void FilePump::OnRemoteDescriptionUpdate(BaseSession* base_session,
		const ContentInfos& updated_contents) 
	{
		Session* session = static_cast<Session*>(base_session);

		
	}

	void FilePump::OnReceivedTerminateReason(Session* session,
		const std::string& reason)
	{
		session_client_->session_manager()->signaling_thread()->Clear(this,
				MSG_TERMINATEPUMP);
		SignalReceivedTerminateReason(this, session, reason);
	}

	void FilePump::IncomingSession(Session* session, const SessionDescription* offer)
	{
		AddSession(session, offer);

		// Make sure the session knows about the incoming ssrcs. This needs to be done
		// prior to the SignalSessionState call, because that may trigger handling of
		// these new SSRCs, so they need to be registered before then.
		//UpdateRemoteMediaStreams(session, offer->contents(), false);

		// Missed the first state, the initiate, which is needed by
		// call_client.
		SignalSessionState(this, session, Session::STATE_RECEIVEDINITIATE);
	}

}