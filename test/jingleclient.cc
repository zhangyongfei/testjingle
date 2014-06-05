#include "jingleclient.h"
#include <iostream>
#include "base/stringencode.h"
#include "xmpp/pingtask.h"
#include "xmpp/constants.h"

// Must be period >= timeout.
const uint32 kPingPeriodMillis = 10000;
const uint32 kPingTimeoutMillis = 10000;

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
		InitPresence();
		break;
	case buzz::XmppEngine::STATE_CLOSED:
		std::cout << "Logged out." << std::endl;
		break;
	}
}

void JingleClient::StartXmppPing() 
{
	buzz::PingTask* ping = new buzz::PingTask(
		xmpp_client_, talk_base::Thread::Current(),
		kPingPeriodMillis, kPingTimeoutMillis);
	ping_task_.reset(ping);
	ping->SignalTimeout.connect(this, &JingleClient::OnPingTimeout);
	ping->Start();
}

void JingleClient::OnPingTimeout() 
{
	LOG(LS_WARNING) << "XMPP Ping timeout. Will keep trying...";
	StartXmppPing();
}

void JingleClient::InitPresence()
{
	roster_module_ = buzz::XmppRosterModule::Create();
	roster_module_->set_roster_handler(this);
	roster_module_->RegisterEngine(xmpp_client_->engine());
	roster_module_->BroadcastPresence();
	roster_module_->RequestRosterUpdate();

	chat_task_.reset(new buzz::ChatTask(xmpp_client_));
	chat_task_->MessageRecv.connect(this, &JingleClient::RecvChat);
	chat_task_->Start();

	// StartXmppPing(); 测试时关闭心跳功能
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

void JingleClient::SendChat(const buzz::Jid& to, const std::string& msg) 
{
	chat_task_->Send(to, msg);
}

void JingleClient::RecvChat(const buzz::Jid& from, const std::string& msg)
{
	printf("Recv From[%s]: %s\n", from.BareJid().Str().c_str(), msg.c_str());
}

// TODO: Make present and record really work.
const char* HANGOUT_COMMANDS =
	"Available MUC commands:\n"
	"\n"
	"  present    Starts presenting (just signalling; not actually presenting.)\n"
	"  unpresent  Stops presenting (just signalling; not actually presenting.)\n"
	"  record     Starts recording (just signalling; not actually recording.)\n"
	"  unrecord   Stops recording (just signalling; not actually recording.)\n"
	"  rmute [nick] Remote mute another participant.\n"
	"  block [nick] Block another participant.\n"
	"  screencast [fps] Starts screencast. \n"
	"  unscreencast Stops screencast. \n"
	"  quit       Quits the application.\n"
	"";

const char* RECEIVE_COMMANDS =
	"Available commands:\n"
	"\n"
	"  accept [bw] Accepts the incoming call and switches to it.\n"
	"  reject  Rejects the incoming call and stays with the current call.\n"
	"  quit    Quits the application.\n"
	"";

const char* CONSOLE_COMMANDS =
	"Available commands:\n"
	"\n"
	"  roster              Prints the online friends from your roster.\n"
	"  friend user         Request to add a user to your roster.\n"
	"  call [jid] [bw]     Initiates a call to the user[/room] with the\n"
	"                      given JID and with optional bandwidth.\n"
	"  vcall [jid] [bw]    Initiates a video call to the user[/room] with\n"
	"                      the given JID and with optional bandwidth.\n"
	"  calls               Lists the current calls\n"
	"  switch [call_id]    Switch to the specified call\n"
	"  join [room_jid]     Joins a multi-user-chat with room JID.\n"
	"  ljoin [room_name]   Joins a MUC by looking up JID from room name.\n"
	"  invite user [room]  Invites a friend to a multi-user-chat.\n"
	"  leave [room]        Leaves a multi-user-chat.\n"
	"  nick [nick]         Sets the nick.\n"
	"  priority [int]      Sets the priority.\n"
	"  getdevs             Prints the available media devices.\n"
	"  quit                Quits the application.\n"
	"";

static int GetInt(const std::vector<std::string>& words, size_t index, int def) 
{
	int val;
	if (words.size() > index && talk_base::FromString(words[index], &val)) {
		return val;
	} else {
		return def;
	}
}

static std::string GetWord(const std::vector<std::string>& words,
						   size_t index, const std::string& def) 
{
	if (words.size() > index) {
		return words[index];
	} else {
		return def;
	}
}

void JingleClient::ParseLine(const std::string& line)
{
	std::vector<std::string> words;
	int start = -1;
	int state = 0;
	for (int index = 0; index <= static_cast<int>(line.size()); ++index) {
		if (state == 0) {
			if (!isspace(line[index])) {
				start = index;
				state = 1;
			}
		} else {
			ASSERT(state == 1);
			ASSERT(start >= 0);
			if (isspace(line[index])) {
				std::string word(line, start, index - start);
				words.push_back(word);
				start = -1;
				state = 0;
			}
		}
	}

	// Global commands
	const std::string& command = GetWord(words, 0, "");
	if (command == "quit") 
	{
		talk_base::Thread *thread = talk_base::ThreadManager::Instance()->CurrentThread();
		thread->Stop();
	}
	else if (command == "presence") 
	{
		int cnt = roster_module_->GetIncomingPresenceCount();

		if(cnt == 0)
		{
			printf("---Thers aren't presences.\n");
		}

		bool bonline = false;

		for (int i = 0; i < cnt; i++)
		{
			const buzz::XmppPresence* contact = roster_module_->GetIncomingPresence(i);

			printf("---presence[%d]:%s\n", i, contact->jid().Str().c_str());
		}
	} 
	else if (command == "roster") 
	{
		int cnt = roster_module_->GetRosterContactCount();

		if(cnt == 0)
		{
			printf("---thers aren't rosters.\n");
		}

		bool bonline = false;

		for (int i = 0; i < cnt; i++)
		{
			const buzz::XmppRosterContact* contact = roster_module_->GetRosterContact(i);

			printf("---roster[%d]:%s\n", i, contact->jid().Str().c_str());
		}
	} 
	else if ((words.size() == 3) && (command == "msg"))
	{
		SendChat(buzz::Jid(words[1]), words[2]);
	} 
	else if ((words.size() == 2) && (command == "friend"))
	{
		roster_module_->RequestSubscription(buzz::Jid(words[2]));
	} 
}