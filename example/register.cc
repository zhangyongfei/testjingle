#include "base/ssladapter.h"
#include "xmpp/xmpppump.h"
#include "base/physicalsocketserver.h"
#include "xmpp/xmppsocket.h"
#include <iostream>

buzz::XmppPump *pump = NULL;

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
				break;
			case buzz::XmppEngine::STATE_CLOSED:
				// Connection ended. 				
				getchar();
				exit(0);
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
	pump = new buzz::XmppPump(&testnotify, buzz::REGISTER_ENGINE);

	buzz::Jid jid;
	buzz::XmppClientSettings xcs;
	talk_base::InsecureCryptStringImpl pass;
	std::string username;
	std::string email;
	std::string fullname;

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

	std::cout << "Email: ";
	std::cin >>  email;

	std::cout << "Fullname: ";
	std::cin >>  fullname;

    std::cout << std::endl;

    getchar();
	// Store the values required for signing in. These
	// include jid, resource, name, and password.
	// The example uses a hard-coded value to indicate the Google Talk 
	// service.
	xcs.set_user(jid.node());
	xcs.set_resource("pcp");  // Arbitrary resource name.
	xcs.set_host(jid.domain());
	xcs.set_email(email);
	xcs.set_fullname(fullname);
	xcs.set_use_tls(buzz::TLS_ENABLED);
	xcs.set_pass(talk_base::CryptString(pass));
	xcs.set_allow_plain(false);
	xcs.set_server(talk_base::SocketAddress("127.0.0.1", 5222));
	
	// Queue up the sign in request.
	buzz::XmppSocket *ctest = new buzz::XmppSocket(buzz::TLS_ENABLED);

	pump->DoRegister(xcs, ctest);


	// Start the thread and run indefinitely.
	main_thread.Run();

    delete pump;
}