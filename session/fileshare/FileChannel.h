#ifndef FILE_CHANNEL
#define FILE_CHANNEL

namespace cricket
{
	class FileChannel
	{
	public:
		FileChannel(
			talk_base::Thread* thread,
			BaseSession* session,
			const std::string& content_name);

		~FileChannel();

		bool Init();


	private:
		talk_base::Thread *thread_;
        BaseSession* session_;
		std::string content_name_;
		TransportChannel* sctp_channel_;
	};
}



#endif