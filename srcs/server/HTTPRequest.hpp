#ifndef HTTPREQUEST_HPP
# define HTTPREQUEST_HPP

class HTTPRequest
{
	public:
		HTTPRequest();
		~HTTPRequest();

		void	ParseRequest( std::string const & recv_msg);

	private:
		enum	e_status
		{
			REQUEST,
			HEADER,
			BODY,
			OK,
			BAD,
		};

		enum	e_method
		{
			GET = 0,
			POST,
			DELETE,
		};

		e_status		request_status_;
		e_status		line_status_;
		e_method		method_;
		std::string		target_;
		std::string		version_;
};

#endif
