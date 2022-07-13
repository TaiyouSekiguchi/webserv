#ifndef CGI_HPP
# define CGI_HPP

# include <unistd.h> // close
# include <sys/types.h> // wait
# include <sys/wait.h> // wait
# include <iostream>
# include <string>
# include <cstdlib> // exit
# include <map>
# include <utility>
# include "HTTPError.hpp"
# include "utils.hpp"

class CGI
{
	public:
		CGI(const std::string& file_path);
		~CGI(void);

		std::string		GetData(void) const;
		std::string		GetContentType(void) const;
		std::string		GetBody(void) const;

	private:
		typedef	void 	(CGI::*ParseFunc)(const std::string& content);

		void	SetEnv(void);
		void	ExecuteCGI(const std::string& file_path);
		void	ParseCGI(void);
		void	DoChild(const std::string& file_path, const int pipe_fd[2]);
		void	DoParent(const int pipe_fd[2]);
		void	ParseHeader(const std::string& line);
		void	ParseContentType(const std::string& content);

		char			**env_;
		std::string		data_;
		std::string		content_type_;
		std::string		body_;
};

#endif
