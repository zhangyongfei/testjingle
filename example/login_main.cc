#include "base/ssladapter.h"
#include "xmpp/xmpppump.h"
#include "base/physicalsocketserver.h"
#include "xmpp/xmppsocket.h"
#include "xmpp/presenceouttask.h"
#include "xmpp/presencereceivetask.h"
#include "xmpp/presencestatus.h"
#include "xmpp/rostermodule.h"
#include "base/xmldebug.h"

#include "base/network.h"
#include "p2p/client/httpportallocator.h"
#include "p2p/base/sessionmanager.h"
#include "p2p/client/sessionmanagertask.h"
#include "xmpp/jingleinfotask.h"
#include <iostream>


buzz::XmppPump *pump = NULL;
buzz::PresenceReceiveTask *presence_recv_ = NULL;
buzz::PresenceOutTask *presence_out_ = NULL;
buzz::XmppRosterModule *roster_module = NULL;

void Signout();

class ClientPresence : public buzz::XmppRosterHandler
{
public:
	void SubscriptionRequest(buzz::XmppRosterModule* roster,
		const buzz::Jid& requesting_jid,
		buzz::XmppSubscriptionRequestType type,
		const buzz::XmlElement* raw_xml)
	{
		roster_module->ApproveSubscriber(requesting_jid);
	}

	void SubscriptionError(buzz::XmppRosterModule* roster,
		const buzz::Jid& from,
		const buzz::XmlElement* raw_xml)
	{
		printf("SubscriptionError\n");
	}

	void RosterError(buzz::XmppRosterModule* roster,
		const buzz::XmlElement* raw_xml)
	{
		printf("RosterError\n");
	}

	void IncomingPresenceChanged(buzz::XmppRosterModule* roster,
		const buzz::XmppPresence* presence)
	{
		printf("IncomingPresenceChanged\n");
	}

	void ContactChanged(buzz::XmppRosterModule* roster,
		const buzz::XmppRosterContact* old_contact,
		size_t index)
	{
		printf("ContactChanged\n");
	}

	void ContactsAdded(buzz::XmppRosterModule* roster,
		size_t index, size_t number)
	{
		printf("ContactsAdded\n");
	}

	void ContactRemoved(buzz::XmppRosterModule* roster,
		const buzz::XmppRosterContact* removed_contact,
		size_t index)
	{
		printf("ContactRemoved\n");
	}
};


talk_base::BasicNetworkManager network_manager_;
cricket::HttpPortAllocator *port_allocator_;
cricket::SessionManager *session_manager_;
cricket::SessionManagerTask *session_manager_task;
buzz::JingleInfoTask *jingle_info_task;

class JingleInfoEvent : public sigslot::has_slots<>
{
public:
	void OnJingleInfoUpdate(
		const std::string &relay_token,
		const std::vector<std::string> &relay_addresses,
		const std::vector<talk_base::SocketAddress> &stun_addresses)
	{
		port_allocator_->SetStunHosts(stun_addresses);
		port_allocator_->SetRelayHosts(relay_addresses);
		port_allocator_->SetRelayToken(relay_token);
	}

};

JingleInfoEvent jingleinfo;

class ClientStatus: public sigslot::has_slots<>
{
public:
	void OnStatusUpdate(const buzz::PresenceStatus& status)
	{
		printf("test\n");
		printf("JID:%s SHOW:%d\n", status.jid().Str().c_str(), 
			status.show());	

		if (status.show() == buzz::PresenceStatus::SHOW_ONLINE)
		{			
			static bool first = true;

			if(first == true)
			{		
				// Create the HttpPortAllocator and HttpPortAllocator, and SessionManager objects.
				// We use the current thread (signaling thread) as the worker thread. To create
				// a new worker thread, pass a new Thread object into the SessionManager constructor.
				// See CallClient::InitPhone for an example of creating a worker thread.
				
				port_allocator_ = new cricket::HttpPortAllocator(&network_manager_, "pcp");
				session_manager_ = new cricket::SessionManager(port_allocator_, NULL);

				// Create the object that will be used to send/receive XMPP session requests
				// and start it up.
				session_manager_task = new cricket::SessionManagerTask(pump->client(), session_manager_);
				session_manager_task->EnableOutgoingMessages();
				session_manager_task->Start();

				// Query for the STUN and relay ports being used. This is an asynchronous call,
				// so we need to register for SignalJingleInfo, which sends the response.
				jingle_info_task = new buzz::JingleInfoTask(pump->client());
				jingle_info_task->RefreshJingleInfoNow();
				jingle_info_task->SignalJingleInfo.connect(&jingleinfo, &JingleInfoEvent::OnJingleInfoUpdate);
				jingle_info_task->Start();


				first = false;
			}
		}
		
		if(status.show() == buzz::PresenceStatus::SHOW_NONE)
		{
            getchar();
			exit(0);
		}
	}
};

ClientStatus clientstatus_;
ClientPresence clientpresence_;

void AddContact()
{
	roster_module->RequestSubscription(buzz::Jid("jeff@fengmao"));
}

void DelContact()
{
	roster_module->RequestSubscription(buzz::Jid("jeff@fengmao"));
}

void Signout()
{
	roster_module->outgoing_presence()->set_available(buzz::XMPP_PRESENCE_UNAVAILABLE);
	roster_module->BroadcastPresence();
}

void Signon()
{
	presence_recv_ = new buzz::PresenceReceiveTask(pump->client());
	
	presence_recv_->PresenceUpdate.connect(
		&clientstatus_, &ClientStatus::OnStatusUpdate);
	presence_recv_->Start();

	roster_module = buzz::XmppRosterModule::Create();
	roster_module->set_roster_handler(&clientpresence_);
	roster_module->RegisterEngine(pump->client()->engine());
	roster_module->BroadcastPresence();
	roster_module->RequestRosterUpdate();
}

class TestPumpNotify: public buzz::XmppPumpNotify
{  
public:
	TestPumpNotify()
	{  

	} 

	void OnStateChange(buzz::XmppEngine::State state) {

		buzz::XmppEngine::Error error;
		int suberror = 0;
		error = pump->client()->GetError(&suberror);
		printf("XmppEngine::State---%d  Error:%d\n", 
			state, suberror);

		switch (state) {
			case buzz::XmppEngine::STATE_START:
				// Attempting sign in.
				break;
			case buzz::XmppEngine::STATE_OPENING:
				// Negotiating with server.
				break;
			case buzz::XmppEngine::STATE_OPEN:
				// Connection succeeded. Send your presence information.
				// and sign up to receive presence notifications.
				Signon();
				break;
			case buzz::XmppEngine::STATE_CLOSED:
				// Connection ended. 
				break;
		}
	}

};

int main(int argc, char **argv) {
	// Initialize SSL channel.
	talk_base::InitializeSSL();

	// Create the signaling thread.
	// AutoThread captures the current OS thread and sets it to be
	// ThreadManager::CurrentThread, which will be called and used by SessionManager 
	talk_base::PhysicalSocketServer ss;
	talk_base::AutoThread main_thread(&ss);
	talk_base::ThreadManager::Instance()->SetCurrentThread(&main_thread);

	TestPumpNotify testnotify;

	// Get the information we'll need to sign in.
	pump = new buzz::XmppPump(&testnotify);

	pump->client()->SignalLogInput.connect(DebugLog::instance(), &DebugLog::Input);
	pump->client()->SignalLogOutput.connect(DebugLog::instance(), &DebugLog::Output);

	buzz::Jid jid;
	buzz::XmppClientSettings xcs;
	talk_base::InsecureCryptStringImpl pass;
	std::string username;

	// Read and verify the user's JID from the command-line.
	std::cout << "JID: ";
	std::cin >>  username;

	jid = buzz::Jid(username);
	if (!jid.IsValid() || jid.node() == "") {
		printf("Invalid JID. JIDs should be in the form user@domain\n");
		return 1;
	}

	// Read the user's password (invisibly) from the screen.
	std::cout << "Password: ";
	std::cin >> pass.password();

	std::cout << std::endl;

	getchar();
	// Store the values required for signing in. These
	// include jid, resource, name, and password.
	// The example uses a hard-coded value to indicate the Google Talk 
	// service.
	xcs.set_user(jid.node());
	xcs.set_resource("pcp");  // Arbitrary resource name.
	xcs.set_host(jid.domain());
	xcs.set_use_tls(buzz::TLS_ENABLED);
	xcs.set_pass(talk_base::CryptString(pass));
	xcs.set_allow_plain(false);
	xcs.set_server(talk_base::SocketAddress("127.0.0.1", 5222));

	// Queue up the sign in request.
	buzz::XmppSocket *ctest = new buzz::XmppSocket(buzz::TLS_ENABLED);

	pump->DoLogin(xcs, ctest, NULL);


	// Start the thread and run indefinitely.
	main_thread.Run();

	delete pump;
}