#ifndef _JINGLECLIENT_
#define _JINGLECLIENT_
#include "xmpp/xmppengine.h"
#include "base/sigslot.h"
#include "xmpp/xmppclient.h"
#include "xmpp/rostermodule.h"
#include "test/console.h"
#include "xmpp/chattask.h"


namespace buzz
{
class PingTask;
}

class JingleClient : public sigslot::has_slots<>,
	                 public buzz::XmppRosterHandler,
                     public ConsoleClient
{
public:
	JingleClient(buzz::XmppClient *xmpp_client);
	virtual ~JingleClient();

	void OnStateChange(buzz::XmppEngine::State state);

	void ParseLine(const std::string& line);

protected:
	void InitPresence();

	void StartXmppPing();

	void OnPingTimeout();

	void SendChat(const buzz::Jid& to, const std::string& msg);

	void RecvChat(const buzz::Jid& from, const std::string& msg);

	void SubscriptionRequest(buzz::XmppRosterModule* roster,
		const buzz::Jid& requesting_jid,
		buzz::XmppSubscriptionRequestType type,
		const buzz::XmlElement* raw_xml);

	void SubscriptionError(buzz::XmppRosterModule* roster,
		const buzz::Jid& from,
		const buzz::XmlElement* raw_xml);

	void RosterError(buzz::XmppRosterModule* roster,
		const buzz::XmlElement* raw_xml);

	void IncomingPresenceChanged(buzz::XmppRosterModule* roster,
		const buzz::XmppPresence* presence);

	void ContactChanged(buzz::XmppRosterModule* roster,
		const buzz::XmppRosterContact* old_contact,
		size_t index);

	void ContactsAdded(buzz::XmppRosterModule* roster,
		size_t index, size_t number);

	void ContactRemoved(buzz::XmppRosterModule* roster,
		const buzz::XmppRosterContact* removed_contact,
		size_t index);

private:
	buzz::XmppClient *xmpp_client_;
	buzz::XmppRosterModule *roster_module_;
	talk_base::scoped_ptr<buzz::PingTask> ping_task_;
	talk_base::scoped_ptr<buzz::ChatTask> chat_task_;
};

#endif