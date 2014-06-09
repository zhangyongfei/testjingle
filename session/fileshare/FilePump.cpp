#include "FilePump.h"


namespace cricket {

	const uint32 MSG_TERMINATEPUMP = 2;
	const int kSendToPumpTimeout = 1000*20;

	FilePump::FilePump(FileSessionClient *session_client)
		: session_client_(session_client)
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
		const CallOptions& options)
	{
		std::string id;
		std::string initiator_name = initiator.Str();
		return InternalInitiateSession(id, to, initiator_name, options);
	}

	Session* FilePump::InternalInitiateSession(const std::string& id,
		const buzz::Jid& to,
		const std::string& initiator_name,
		const CallOptions& options) {
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

}