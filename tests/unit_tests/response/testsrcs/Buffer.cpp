#include "Buffer.hpp"

Buffer::Buffer() {}
Buffer::~Buffer() {}

std::map<std::string, std::string> Buffer::GetHeader() const { return header_; }
std::string Buffer::GetBody() const { return body_; }
std::string Buffer::GetStatus() const { return status_; }

void Buffer::SetStatus(const std::string &str) { status_ = str; }
void Buffer::SetBody(const std::string &str) { body_ = str; }
void Buffer::SetHeader(const std::string &key, const std::string &value)
{
	header_[key] = value;
}
