#ifndef HTTPERROR_HPP
# define HTTPERROR_HPP

# include <stdexcept>
# include <iostream>
# include "HTTPStatusCode.hpp"

class HTTPError : public std::exception
{
	public:
		explicit HTTPError(const e_StatusCode status_code, const char* msg) : status_code_(status_code), msg_(msg) {}

		e_StatusCode	GetStatusCode() const { return (status_code_); }
		void			PutMsg() const
		{
			std::cerr << msg_ << " throw exception." << std::endl;
		}

	private:
		e_StatusCode	status_code_;
		const char*		msg_;
};

#endif
