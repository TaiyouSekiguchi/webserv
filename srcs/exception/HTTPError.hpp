#ifndef HTTPERROR_HPP
# define HTTPERROR_HPP

# include <stdexcept>

class HTTPError : public std::exception
{
	public:
		enum	e_status
		{
			BAD_REQUEST = 400,
			FORBIDDEN = 403,
			NOT_FOUND = 404,
			METHOD_NOT_ALLOWED = 405,
			CONFLICT = 409,
			PAYLOAD_TOO_LARGE = 413,
			INTERNAL_SERVER_ERROR = 500,
			HTTP_VERSION_NOT_SUPPORTED = 505
		};

		explicit HTTPError(const int status_code, const std::string& msg = "HTTPError") : status_code_(status_code), msg_(msg) {}

		int				GetStatusCode() const { return (status_code_); }
		std::string		GetMsg() const { return (msg_ + " throw exception."); }

	private:
		int				status_code_;
		std::string		msg_;
};

#endif
