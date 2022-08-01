#ifndef REGULARFILEEVENT_HPP
# define REGULARFILEEVENT_HPP

# include "RegularFile.hpp"
# include "AIoEvent.hpp"
# include "HTTPServer.hpp"

class MethodFileEvent : public AIoEvent
{
	public:
		MethodFileEvent(const RegularFile* rfile, const e_EventType type, HTTPServer* hserver);
		~MethodFileEvent();

		int				GetFd() const;
		e_EventStatus	RunEvent(EventQueue* equeue);

	private:
		e_EventStatus	RunReadEvent(EventQueue* equeue);
		e_EventStatus	RunWriteEvent(EventQueue* equeue);

		const RegularFile*	rfile_;
		HTTPServer*			hserver_;
};

#endif
