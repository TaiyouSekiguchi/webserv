#ifndef CGI_HPP
# define CGI_HPP

# include <sys/wait.h>  // waitpid()
# include <sys/types.h>  // waitpid()

# include <string>
# include <cstdlib>  // exit()
# include <cerrno>  // errno

# include "CGIEnv.hpp"
# include "URI.hpp"
# include "Pipe.hpp"
# include "Stat.hpp"
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
		e_HTTPServerEventType	ReceiveCgiResult(void);

		std::string				GetData(void) const;
		std::map<std::string, std::string>	GetHeaders(void) const;
		e_StatusCode			GetStatusCode(void) const;
		std::string				GetBody(void) const;
		int						GetToCgiWriteFd(void) const;
		int						GetFromCgiReadFd(void) const;

	private:
		int						ExecveCGIScript(void);
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

		std::map<std::string, std::string>		headers_;
		e_StatusCode							status_code_;

		std::string				body_;
};

#endif
