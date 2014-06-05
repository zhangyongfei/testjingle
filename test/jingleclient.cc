#include "jingleclient.h"
#include <iostream>

JingleClient::JingleClient(buzz::XmppClient *xmpp_client) :
     xmpp_client_(xmpp_client)
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
		//if (!waiting_for_file_)
		//	std::cout << "Waiting for " << send_to_jid_.Str() << std::endl;
		InitPresence();
		break;
	case buzz::XmppEngine::STATE_CLOSED:
		std::cout << "Logged out." << std::endl;
		break;
	}
}

void JingleClient::InitPresence()
{
	roster_module_ = buzz::XmppRosterModule::Create();
	roster_module_->set_roster_handler(this);
	roster_module_->RegisterEngine(xmpp_client_->engine());
	roster_module_->BroadcastPresence();
	roster_module_->RequestRosterUpdate();
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

void JingleClient::ParseLine(const std::string& line)
{

}