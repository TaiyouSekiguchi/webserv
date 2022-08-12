#ifndef CGI_HPP
# define CGI_HPP

# include <sys/wait.h>  // waitpid()
# include <sys/types.h>  // waitpid()

# include <string>
# include <cstdlib>  // exit()

# include "CGIEnv.hpp"
# include "URI.hpp"
# include "Pipe.hpp"
# include "HTTPRequest.hpp"
# include "HTTPError.hpp"
# include "HTTPStatusCode.hpp"
# include "utils.hpp"

class CGI
{
	public:
		CGI(const URI& uri, const HTTPRequest& req);
		~CGI(void);

		e_HTTPServerEventType	ExecCGI(void);
		void					PostToCgi(void);
		e_HTTPServerEventType	ReceiveCgiResult(const bool eof_flag);

		std::string				GetData(void) const;
		std::string				GetContentType(void) const;
		std::string				GetLocation(void) const;
		e_StatusCode			GetStatusCode(void) const;
		std::string				GetBody(void) const;
		int						GetToCgiFd(void) const;
		int						GetFromCgiFd(void) const;

	private:
		void					ExecveCGIScript(void);
		void					ParseCGI(void);
		void					ParseHeader(const std::string& line);
		void					ParseContentType(const std::string& content);
		void					ParseLocation(const std::string& content);
		void					ParseStatusCode(const std::string& content);

		const URI&				uri_;
		const HTTPRequest&		req_;
		Pipe					to_cgi_pipe_;
		Pipe					from_cgi_pipe_;
		pid_t					pid_;
		std::string				data_;
		std::string				content_type_;
		std::string				location_;
		e_StatusCode			status_code_;
		std::string				body_;
};

#endif
