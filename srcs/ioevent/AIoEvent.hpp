#ifndef AIOEVENT_HPP
# define AIOEVENT_HPP

# include "EventFlag.hpp"
# include "EventQueue.hpp"

class EventQueue;

class AIoEvent
{
	public:
		explicit AIoEvent(const e_EventType event_type);
		virtual ~AIoEvent();

		e_EventType				GetEventType() const;

		virtual int				GetFd() const = 0;
		virtual e_EventStatus	RunEvent(EventQueue* equeue) = 0;

	protected:
		e_EventType		event_type_;
};

#endif
