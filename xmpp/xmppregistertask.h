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

#ifndef _REGISTEROUTTASK_H_
#define _REGISTEROUTTASK_H_

#include "xmpp/xmppengine.h"
#include "xmpp/xmpptask.h"
#include "xmpp/xmppsocket.h"
#include "xmpp/xmppengine.h"
#include "base/logging.h"

namespace buzz {

class XmppEngineImpl;

class XmppReigsterTask {
public:
	XmppReigsterTask(XmppEngineImpl * pctx);
	~XmppReigsterTask();

	bool IsDone()
	{ return state_ == REGISTERSTATE_DONE; }

	bool Register(const XmlElement * element, 
		          const std::string & username, 
		          const std::string & password, 
		          const std::string & email, 
				  const std::string & fullname);

private:
	enum RegisterTaskState {
		REGISTERSTATE_INIT = 0,
		REGISTERSTATE_STREAMSTART_SENT,
		REGISTERSTATE_STARTED_XMPP,
		REGISTERSTATE_REGISTER_SENT,
		REGISTERSTATE_DONE,
	};

	const XmlElement * NextStanza();
	bool HandleStartStream(const XmlElement * element);
	bool Failure(XmppEngine::Error reason, int subcode = 0);
	bool HandleFeatures(const XmlElement * element);
	const XmlElement * GetFeature(const QName & name);

	bool HandleResult(const XmlElement * element);

	void SendRegister(const std::string & username, 
		const std::string & password, 
		const std::string & email, 
		const std::string & fullname);

	XmppEngineImpl *pctx_;
	RegisterTaskState state_;
	const XmlElement * pelStanza_;

	std::string streamId_;

	std::string iqId_;

	talk_base::scoped_ptr<XmlElement> pelFeatures_;

#ifdef _DEBUG
	static const talk_base::ConstantLabel REGISTERTASK_STATES[];
#endif  // _DEBUG
};
}

#endif
