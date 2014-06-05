#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h> 
#endif
#include "base/physicalsocketserver.h"
#include "base/xmldebug.h"
#include "base/ssladapter.h"
#include "xmpp/xmpppump.h"
#include "xmpp/xmppsocket.h"
#include "test/jingleclient.h"

int main(int argc, char * argv[])
{
#ifdef WIN32
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#else
   system("stty echo");
#endif
	
	talk_base::PhysicalSocketServer ss;

	talk_base::LogMessage::LogToDebug(talk_base::LS_VERBOSE);

	talk_base::InitializeSSL();

	buzz::XmppPump pump;
	buzz::Jid jid;
	buzz::XmppClientSettings xcs;
	talk_base::InsecureCryptStringImpl pass;
	std::string username;

	jid=buzz::Jid(std::string("jeff@localhost"));
	pass.password()="jeff";
	xcs.set_user(jid.node());
	xcs.set_resource("pcp");
	xcs.set_host(jid.domain());
	xcs.set_use_tls(buzz::TLS_DISABLED);
	xcs.set_allow_plain(true);

	xcs.set_pass(talk_base::CryptString(pass));
	xcs.set_server(talk_base::SocketAddress("10.192.1.197", 5222));

	talk_base::Thread main_thread(&ss);
	talk_base::ThreadManager::Instance()->SetCurrentThread(&main_thread);

	pump.client()->SignalLogInput.connect(DebugLog::instance(), &DebugLog::Input);
	pump.client()->SignalLogOutput.connect(DebugLog::instance(), &DebugLog::Output);

	JingleClient fs_client(pump.client());

	Console *console = new Console(&main_thread, &fs_client);

	pump.client()->SignalStateChange.connect(&fs_client, &JingleClient::OnStateChange);
	console->Start();

	pump.DoLogin(xcs, new buzz::XmppSocket(buzz::TLS_DISABLED), NULL);
	main_thread.Run();
	pump.DoDisconnect();

	return 0;
}