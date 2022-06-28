#include "EventQueue.hpp"

EventQueue::EventQueue()
{
	waitspec_.tv_sec = 2;
	waitspec_.tv_nsec = 500000;
}

EventQueue::~EventQueue()
{
}

void	EventQueue::CreateQueue(void)
{
	kq_ = kqueue();
	if (kq_ == -1)
		throw std::runtime_error("kqueue error");
}

void	EventQueue::RegisterEvent(int sock)
{
	struct kevent	kev;
	int				ret;

	EV_SET(&kev, sock, EVFILT_READ, EV_ADD, 0, 0, NULL);

	ret = kevent(kq_, &kev, 1, NULL, 0, NULL);
	if (ret == -1)
		throw std::runtime_error("kevent error");
}

int		EventQueue::WaitEvent(struct kevent * kev)
{
	int		n;

	n = kevent(kq_, NULL, 0, kev, 1, &waitspec_);
	if (n == -1)
		throw std::runtime_error("kevent error");

	return (n);
}
