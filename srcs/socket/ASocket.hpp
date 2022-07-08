#ifndef ASOCKET_HPP
# define ASOCKET_HPP

# include "ServerDirective.hpp"

class ASocket
{
	public:
		explicit ASocket(const int fd, const ServerDirective& server_conf);
		virtual ~ASocket();

		int						GetFd() const;
		const ServerDirective&	GetServerConf() const;

	protected:
		int						fd_;
		const ServerDirective&	server_conf_;
};

#endif
