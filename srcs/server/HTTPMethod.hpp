#ifndef HTTPMETHOD_HPP
# define HTTPMETHOD_HPP

# include "HTTPRequest.hpp"

class HTTPMethod
{
	public:
		HTTPMethod();
		~HTTPMethod();

		int		ExecHTTPMethod(const HTTPRequest& req, const ServerDirective& server_conf);

		const std::string&	GetContentType() const;
		const std::string&	GetLocation() const;
		const std::string&	GetBody() const;

		void				MethodDisplay() const;

	private:
		LocationDirective	SelectLocation(const std::string& target, const std::vector<LocationDirective>& locations) const;
		bool				CheckSlashEnd(const std::string& target, const std::string& hostname, const int port);
		bool				GetFile(const std::string& file_path);
		bool				GetFileWithIndex(const std::string& access_path, const std::vector<std::string>& indexes);
		bool				GetAutoIndexFile(const bool autoindex);

		int	ExecGETMethod(const LocationDirective& location, const std::string& access_path, const struct stat& st);
		int	ExecDELETEMethod(const std::string& access_path, const struct stat& st);

		std::string		content_type_;
		std::string		location_;
		std::string		body_;

		const HTTPRequest*			req_;
		const ServerDirective*		server_conf_;
};

#endif
