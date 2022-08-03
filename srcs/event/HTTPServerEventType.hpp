#ifndef HTTPSERVEREVENTTYPE_HPP
# define HTTPSERVEREVENTTYPE_HPP

enum e_HTTPServerEventType
{
	SEVENT_NO			= -1,
	SEVENT_SOCKET_RECV	= 0,
	SEVENT_SOCKET_SEND	= 1,
	SEVENT_FILE_READ	= 2,
	SEVENT_FILE_WRITE	= 3,
	SEVENT_FILE_DELETE 	= 4,
	SEVENT_END			= 5
};

#endif
