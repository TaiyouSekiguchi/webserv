#ifndef ASOCKET_HPP
# define ASOCKET_HPP

# include "ServerDirective.hpp"

class ASocket
{
	public:
		ASocket(const int fd, const ServerDirective::Listen& listen, const ServerDirective& server_conf);
		ASocket(const int fd, const ServerDirective::Listen& listen, const std::vector<const ServerDirective*>& server_confs);

		virtual ~ASocket();

		int											GetFd() const;
		const ServerDirective::Listen&				GetListen() const;
		const std::vector<const ServerDirective*>&	GetServerConfs() const;

		void	AddServerConf(const ServerDirective& server_conf);

	protected:
		int									fd_;
		const ServerDirective::Listen&		listen_;
		std::vector<const ServerDirective*>	server_confs_;
};

#endif
