#ifndef CGI_HPP
# define CGI_HPP

# include <string>

class CGI
{
	public:
		CGI(void);
		~CGI(void);

		void	ExecuteCGI(const std::string& file_path);
		void	ParseCGI(void);

		std::string		GetContentType(void) const;
		std::string		GetBody(void) const;

	private:
		std::string		content_type_;
		std::string		body_;
};

#endif
