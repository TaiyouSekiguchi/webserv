#ifndef AIO_HPP
# define AIO_HPP

class AIo
{
	public:
		explicit	AIo(const int fd);
		virtual		~AIo();

		bool		Fail() const;
		int			GetFd() const;

	protected:
		bool		failed_;
		int			fd_;
};

#endif
