#ifndef AEVENT_HPP
# define AEVENT_HPP

# include <sys/event.h>

class AEvent
{
	public:
		AEvent();
		virtual ~AEvent() = 0;

		void	SetFlags(const u_short flags);
		bool	IsEOF() const;

	protected:
		u_short		flags_;
};

#endif
