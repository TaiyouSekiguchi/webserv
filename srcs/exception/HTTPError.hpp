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
		};

		explicit HTTPError(const int status_code) : status_code_(status_code) {}

		int	GetStatusCode() const { return (status_code_); }

	private:
		int		status_code_;
};

#endif
