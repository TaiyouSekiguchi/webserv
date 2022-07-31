#ifndef ASOCKET_HPP
# define ASOCKET_HPP

# include "AIo.hpp"
# include "ServerDirective.hpp"

class ASocket : public AIo
{
	public:
		ASocket(const int fd, const ServerDirective::Listen& listen, const ServerDirective& server_conf);
		ASocket(const int fd, const ServerDirective::Listen& listen, const std::vector<const ServerDirective*>& server_confs);

		virtual ~ASocket();

		const ServerDirective::Listen&				GetListen() const;
		const std::vector<const ServerDirective*>&	GetServerConfs() const;

		void	AddServerConf(const ServerDirective& server_conf);

	protected:
		const ServerDirective::Listen&		listen_;
		std::vector<const ServerDirective*>	server_confs_;
};

#endif
