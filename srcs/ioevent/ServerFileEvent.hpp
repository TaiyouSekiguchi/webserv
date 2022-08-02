#ifndef SERVERFILEEVENT_HPP
# define SERVERFILEEVENT_HPP

# include "RegularFile.hpp"
# include "AServerIoEvent.hpp"
# include "HTTPServer.hpp"

class ServerFileEvent : public AServerIoEvent
{
	public:
		ServerFileEvent(const RegularFile* rfile, const e_EventType type);
		~ServerFileEvent();

		int				GetFd() const;
		e_EventStatus	RunEvent(EventQueue* equeue);

	private:
		const RegularFile*	rfile_;
};

#endif
