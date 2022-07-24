#include <unistd.h>
#include "ASocket.hpp"

ASocket::ASocket
	(const int fd, const ServerDirective::Listen& listen, const ServerDirective& server_conf)
	: fd_(fd), listen_(listen)
{
	server_confs_.push_back(&server_conf);
}

ASocket::ASocket
	(const int fd, const ServerDirective::Listen& listen, const std::vector<const ServerDirective*>& server_confs)
	: fd_(fd), listen_(listen), server_confs_(server_confs)
{
}

ASocket::~ASocket()
{
	close(fd_);
}

int											ASocket::GetFd() const	{ return (fd_); }
const ServerDirective::Listen&				ASocket::GetListen() const { return (listen_); }
const std::vector<const ServerDirective*>&	ASocket::GetServerConfs() const { return (server_confs_); }

void	ASocket::AddServerConf(const ServerDirective& server_conf)
{
	server_confs_.push_back(&server_conf);
}
