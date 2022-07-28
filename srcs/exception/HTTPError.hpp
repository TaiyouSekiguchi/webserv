#ifndef HTTPERROR_HPP
# define HTTPERROR_HPP

# include <stdexcept>
# include <iostream>

class HTTPError : public std::exception
{
	public:
		explicit HTTPError(const int status_code, const char* msg) : status_code_(status_code), msg_(msg) {}

		int		GetStatusCode() const { return (status_code_); }
		void	PutMsg() const
		{
			std::cerr << msg_ << " throw exception." << std::endl;
		}

	private:
		int				status_code_;
		const char*		msg_;
};

#endif
