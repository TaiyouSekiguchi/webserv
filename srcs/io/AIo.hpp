#ifndef AIO_HPP
# define AIO_HPP

class AIo
{
	public:
		explicit	AIo(const int fd);
		virtual		~AIo();

		int			GetFd() const;

	protected:
		const int	fd_;
};

#endif
