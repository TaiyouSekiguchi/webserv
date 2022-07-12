#ifndef CGI_HPP
# define CGI_HPP

# include <unistd.h> // close
# include <sys/types.h> // wait
# include <sys/wait.h> // wait
# include <iostream>
# include <string>
# include "HTTPError.hpp"

class CGI
{
	public:
		CGI(void);
		~CGI(void);

		void	ExecuteCGI(const std::string& file_path);
		void	ParseCGI(void);

		std::string		GetData(void) const;
		std::string		GetContentType(void) const;
		std::string		GetBody(void) const;

	private:
		void	do_child(const std::string& file_path, const int pipe_fd[2]);
		void	do_parent(const int pipe_fd[2]);

		std::string		data_;
		std::string		content_type_;
		std::string		body_;
};

#endif
