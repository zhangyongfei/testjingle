/*
 * libjingle
 * Copyright 2004--2005, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright notice,
 *     this list of conditions and the following disclaimer in the documentation
 *     and/or other materials provided with the distribution.
 *  3. The name of the author may not be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <time.h>
#include <sstream>
#include "base/stringencode.h"
#include "xmpp/constants.h"
#include "xmpp/chattask.h"
#include "xmpp/xmppclient.h"

namespace buzz {

ChatTask::ChatTask(XmppTaskParentInterface* parent)
	: XmppTask(parent, XmppEngine::HL_TYPE)
{

}

ChatTask::~ChatTask() {
	Stop();
}

XmppReturnStatus ChatTask::Send(const Jid& to, const std::string& message) {
  if (GetState() != STATE_INIT && GetState() != STATE_START)
    return XMPP_RETURN_BADSTATE;

  buzz::XmlElement* stanza = new buzz::XmlElement(buzz::QN_MESSAGE);
  stanza->AddAttr(buzz::QN_TO, to.Str());
  stanza->AddAttr(buzz::QN_ID, talk_base::CreateRandomString(16));
  stanza->AddAttr(buzz::QN_TYPE, "chat");
  buzz::XmlElement* body = new buzz::XmlElement(buzz::QN_BODY);
  body->SetBodyText(message);
  stanza->AddElement(body);

  SendStanza(stanza);
  delete stanza;

  return XMPP_RETURN_OK;
}

bool ChatTask::HandleStanza(const XmlElement* stanza)
{
	// Verify that this is a presence stanze
	if (stanza->Name() != QN_MESSAGE) 
	{
		return false; // not sure if this ever happens.
	}

	const XmlElement *body = stanza->FirstNamed(QN_BODY);

	if (body == NULL)
	{
		return false;
	}
	
	const std::string from = stanza->Attr(QN_FROM);
	const std::string msg = body->BodyText();

	MessageRecv(Jid(from), msg);

	return true;
}

int ChatTask::ProcessStart()
{
	const XmlElement * stanza = NextStanza();
	if (stanza == NULL)
		return STATE_BLOCKED;


	return STATE_START;
}

}
