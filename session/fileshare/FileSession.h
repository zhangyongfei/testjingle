#ifndef FILE_SESSION
#define FILE_SESSION
#include "p2p/base/sessiondescription.h"
#include "base/pathutils.h"

namespace cricket {

class FileOption
{
public:
	FileOption()
	{

	}
	~FileOption()
	{

	}

	void set_file_name(const talk_base::Pathname& file_name) { file_name_ = file_name; }
	void set_file_size(const uint64_t file_size) { file_size_ = file_size; }

	const talk_base::Pathname& file_name() const { return file_name_; }
	const uint64_t file_size() const { return file_size_; }

private:
	talk_base::Pathname file_name_;
	uint64_t    file_size_;
};

class FileDescription : public ContentDescription
{
public: 
	FileDescription()
	{

	}

	~FileDescription()
	{

	}

	void set_option(const FileOption& option) { option_ = option; }
	const FileOption& option() const { return option_; }

	ContentDescription* Copy() const
	{
		FileDescription *copy = new FileDescription();
		copy->option_ = this->option_;
		return copy;
	}

private:
	FileOption option_;
};

}

#endif