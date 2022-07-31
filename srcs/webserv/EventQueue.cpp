#include "EventQueue.hpp"

EventQueue::EventQueue()
{
	kq_ = kqueue();
	if (kq_ == -1)
		throw std::runtime_error("kqueue error");
}

EventQueue::~EventQueue()
{
}

void	EventQueue::SetIoEvent(AIoEvent *io_event, const e_EventType type, const e_EventAction act) const
{
	struct kevent	kev;
	int				ret;

	EV_SET(&kev, io_event->GetFd(), type, act, 0, 0, io_event);
	ret = kevent(kq_, &kev, 1, NULL, 0, NULL);
	if (ret == -1)
		throw std::runtime_error("kevent error");
}

AIoEvent*		EventQueue::WaitIoEvent() const
{
	struct kevent		kev;
	struct timespec		waitspec = { 2, 500000 };
	int					n;

	while (1)
	{
		n = kevent(kq_, NULL, 0, &kev, 1, &waitspec);
		if (n == -1)
			throw std::runtime_error("kevent error");
		else if (n != 0)
			break;
	}
	return (static_cast<AIoEvent*>(kev.udata));
}
