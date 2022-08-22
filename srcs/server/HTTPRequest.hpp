#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include <set>
#include "ServerSocket.hpp"
#include "HTTPError.hpp"
#include "HTTPStatusCode.hpp"
#include "utils.hpp"
# include "HTTPServerEventType.hpp"

class HTTPRequest
{
	public:
		explicit HTTPRequest(const ServerSocket& ssocket);
		~HTTPRequest();

		e_HTTPServerEventType	ParseRequest(void);
		void					RequestDisplay(void) const;
		void					HeadersDisplay(void);

		// Getter
		const ServerDirective::Listen&			GetListen(void) const;
		const ServerDirective*					GetServerConf(void) const;
		std::string								GetMethod(void) const;
		std::string								GetTarget(void) const;
		std::string								GetVersion(void) const;
		std::pair<std::string, std::string>		GetHost(void) const;
		size_t									GetContentLength(void) const;
		std::string								GetUserAgent(void) const;
		std::vector<std::string>				GetAcceptEncoding(void) const;
		bool									GetConnection(void) const;
		std::string								GetContentType(void) const;
		std::string								GetBody(void) const;
		std::string								GetAccept(void) const;
		std::pair<std::string, std::string>		GetAuthorization(void) const;

	private:
		typedef void (HTTPRequest::*ParseFunc)(const std::string& content);

		// arg
		const ServerSocket&							ssocket_;
		const ServerDirective::Listen&				listen_;
		const std::vector<const ServerDirective*>&	server_confs_;
		const ServerDirective*						server_conf_;

		// server_conf
		size_t										client_max_body_size_;

		// GetLine
		std::string									save_;
		int											step_;

		// request line
		std::string									method_;
		std::string									target_;
		std::string									version_;

		// header
		std::map<std::string, std::string>			headers_;
		std::pair<std::string, std::string>			host_;
		size_t										content_length_;
		std::string									user_agent_;
		std::vector<std::string>					accept_encoding_;
		bool										connection_;
		std::string									content_type_;
		std::string									transfer_encoding_;
		std::string									accept_;
		std::pair<std::string, std::string>			authorization_;

		// body
		std::string									body_;

		// chunk
		std::string									raw_body_;
		size_t										chunk_start_;
		long										chunk_size_;

		// func
		bool			IsToken(const std::string& str);
		bool			IsTChar(char c);
		bool			GetLine(std::string* line);
		bool			ReceiveRequestLine(void);
		void			ParseMethod(const std::string& method);
		void			ParseTarget(const std::string& target);
		void			ParseVersion(const std::string& version);
		void			RegisterHeaders(const std::string& field, const std::string& content);
		bool			ReceiveHeaders(void);
		void			ParseHeaders(void);
		void			CheckHeaders(void);
		void			ParseHeader(const std::string& field, const std::string& content);
		void			ParseHost(const std::string& content);
		void			ParseContentLength(const std::string& content);
		void			ParseUserAgent(const std::string& content);
		void			ParseAcceptEncoding(const std::string& content);
		void			ParseConnection(const std::string& content);
		void			ParseContentType(const std::string& content);
		void			ParseTransferEncoding(const std::string& content);
		void			ParseBody(void);
		bool			ReceiveBody(void);
		void			FindServerConf(void);
		bool			ParseChunk(void);
		bool			ParseOneChunk(void);
		void			ParseAccept(const std::string& content);
		void			ParseAuthorization(const std::string& content);
};

#endif
