#include "FileChannel.h"

namespace cricket
{
	FileChannel::FileChannel(
		talk_base::Thread* thread, 
		BaseSession* session, 
		const std::string& content_name)
		: thread_(thread),
        session_(session),
		content_name_(content_name)
	{

	}

	FileChannel::~FileChannel()
	{

	}

	bool FileChannel::Init()
	{
		sctp_channel_ = session_->CreateChannel(content_name_, 
			"sctp", 
			ICE_CANDIDATE_COMPONENT_DEFAULT);

		return true;
	}
}