#ifndef HTTPERROR_HPP
# define HTTPERROR_HPP

# include <stdexcept>

class HTTPError : public std::exception
{
	public:
		explicit HTTPError(const int status_code) : status_code_(status_code) {}

		const int	GetStatusCode() const { return (status_code_); }

	private:
		int		status_code_;
};

#endif
