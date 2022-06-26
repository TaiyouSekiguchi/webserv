#ifndef HTTPSERVER_HPP
#define HTTPSERVER_HPP

# include "ListenSocket.hpp"

class HTTPServer
{
	public:
		HTTPServer();
		~HTTPServer();

		void	Start() const;

	private:
		void	Communication(const ListenSocket& lsocket) const;
};

#endif  // HTTPSERVER_HPP
