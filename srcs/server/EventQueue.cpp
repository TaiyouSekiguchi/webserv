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

void	EventQueue::RegisterEvent(const int fd, void *udata) const
{
	struct kevent	kev;
	int				ret;

	EV_SET(&kev, fd, EVFILT_READ, EV_ADD, 0, 0, udata);
	ret = kevent(kq_, &kev, 1, NULL, 0, NULL);
	if (ret == -1)
		throw std::runtime_error("kevent error");
}

void	*EventQueue::WaitEvent() const
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
	return (kev.udata);
}
