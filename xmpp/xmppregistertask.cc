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
#include "xmpp/xmppregistertask.h"
#include "xmpp/xmppclient.h"
#include "xmpp/xmppengineimpl.h"

namespace buzz {

	using namespace talk_base;

#ifdef _DEBUG
	const ConstantLabel XmppReigsterTask::REGISTERTASK_STATES[] = {
		KLABEL(REGISTERSTATE_INIT),
		KLABEL(REGISTERSTATE_STREAMSTART_SENT),
		KLABEL(REGISTERSTATE_STARTED_XMPP),
		KLABEL(REGISTERSTATE_REGISTER_SENT),
		KLABEL(REGISTERSTATE_DONE),
		LASTLABEL
	};
#endif  // _DEBUG

	XmppReigsterTask::XmppReigsterTask(XmppEngineImpl * pctx) : 
		pctx_(pctx),
		state_(REGISTERSTATE_INIT),
		pelStanza_(NULL)
	{

	}

	XmppReigsterTask::~XmppReigsterTask()
	{

	}

	const XmlElement *
		XmppReigsterTask::NextStanza() {
			const XmlElement * result = pelStanza_;
			pelStanza_ = NULL;
			return result;
	}

	bool XmppReigsterTask::HandleStartStream(const XmlElement *element) {

			if (element->Name() != QN_STREAM_STREAM)
				return false;

			if (element->Attr(QN_XMLNS) != "jabber:client")
				return false;

			if (element->Attr(QN_VERSION) != "1.0")
				return false;

			if (!element->HasAttr(QN_ID))
				return false;

			streamId_ = element->Attr(QN_ID);

			return true;
	}

	bool XmppReigsterTask::Failure(XmppEngine::Error reason, int subcode) {
	    state_ = REGISTERSTATE_DONE;
		pctx_->SignalError(reason, subcode);
		return false;
	}

	bool XmppReigsterTask::HandleFeatures(const XmlElement *element) {
		if (element->Name() != QN_STREAM_FEATURES)
			return false;

		pelFeatures_.reset(new XmlElement(*element));
		return true;
	}

	const XmlElement *
		XmppReigsterTask::GetFeature(const QName & name) {
		return pelFeatures_->FirstNamed(name);
	}

	bool XmppReigsterTask::HandleResult(const XmlElement * element)
	{
		if (element->Name() != QN_IQ)
		{
			return false;
		}

		if (element->Attr(QN_TYPE) == "error")
		{
			int subcode = 0;
			const XmlElement *tmp = NULL;
			if((tmp = element->FirstNamed(QN_ERROR)) != NULL)
			{
				subcode = atoi(tmp->Attr(QN_CODE).c_str());
			}
			Failure(XmppEngine::ERROR_EXISTED_USERNAME, 
				subcode);
			return false;
		}
		
		return true;
	}

	void XmppReigsterTask::SendRegister(const std::string & username, 
		const std::string & password, 
		const std::string & email, 
		const std::string & fullname)
	{
		XmlElement iq(QN_IQ);

		iq.AddAttr(QN_TYPE, STR_SET);
		iqId_ = pctx_->NextId();
		iq.AddAttr(QN_ID, iqId_);

		iq.AddElement(new XmlElement(QN_REGISTER_QUERY));
		iq.AddElement(new XmlElement(QN_REGISTER_USERNAME), 1);
		iq.AddText(username, 2);

		iq.AddElement(new XmlElement(QN_REGISTER_PASSWORD), 1);
		iq.AddText(password, 2);

		iq.AddElement(new XmlElement(QN_REGISTER_EMAIL), 1);
		iq.AddText(email, 2);

		iq.AddElement(new XmlElement(QN_REGISTER_NAME), 1);
		iq.AddText(fullname, 2);

		pctx_->InternalSendStanza(&iq);
	}

	bool XmppReigsterTask::Register(const XmlElement * element, 
		const std::string & username, 
		const std::string & password, 
		const std::string & email, 
		const std::string & fullname)
	{
		pelStanza_ = element;
		for (;;) {

			const XmlElement * element = NULL;

#if _DEBUG
			LOG(LS_VERBOSE) << "XmppLoginTask::Advance - "
				<< talk_base::ErrorName(state_, REGISTERTASK_STATES);
#endif  // _DEBUG

			switch (state_) {

			case REGISTERSTATE_INIT: {
				pctx_->RaiseReset();
				pelFeatures_.reset(NULL);

				// The proper domain to verify against is the real underlying
				// domain - i.e., the domain that owns the JID.  Our XmppEngineImpl
				// also allows matching against a proxy domain instead, if it is told
				// to do so - see the implementation of XmppEngineImpl::StartTls and
				// XmppEngine::SetTlsServerDomain to see how you can use that feature
				pctx_->InternalSendStart(pctx_->user_jid_.domain());
				state_ = REGISTERSTATE_STREAMSTART_SENT;
				break;
            }
			case REGISTERSTATE_STREAMSTART_SENT: {
				if (NULL == (element = NextStanza()))
					return true;

				if (!HandleStartStream(element))
					return Failure(XmppEngine::ERROR_VERSION);

				state_ = REGISTERSTATE_STARTED_XMPP;
				return true;
			}
			case REGISTERSTATE_STARTED_XMPP: {
				if (NULL == (element = NextStanza()))
					return true;

				if (!HandleFeatures(element))
					return Failure(XmppEngine::ERROR_VERSION);

				bool tls_present = (GetFeature(QN_TLS_STARTTLS) != NULL);
				// Error if TLS required but not present.
				if (pctx_->tls_option_ == buzz::TLS_REQUIRED && !tls_present) {
					return Failure(XmppEngine::ERROR_TLS);
				}
				
				SendRegister(username, password, email, fullname);
				state_ = REGISTERSTATE_REGISTER_SENT;
				return true;
			}		
			case REGISTERSTATE_REGISTER_SENT:
			{
				if (NULL == (element = NextStanza()))
					return true;
				HandleResult(element);
				state_ = REGISTERSTATE_DONE;
				pctx_->SignalError(XmppEngine::ERROR_NONE, 0);
				return true;
			}
			case REGISTERSTATE_DONE:
				return false;
			}
		}
		
	}

}
