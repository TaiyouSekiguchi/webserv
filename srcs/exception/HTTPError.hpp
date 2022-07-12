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
			PAYLOAD_TOO_LARGE = 413,
			HTTP_VERSION_NOT_SUPPORTED = 505,
		};

		explicit HTTPError(const int status_code) : status_code_(status_code) {}

		int	GetStatusCode() const { return (status_code_); }

	private:
		int		status_code_;
};

#endif
