#ifndef AIO_HPP
# define AIO_HPP

class AIo
{
	public:
		explicit	AIo(const int fd);
		virtual		~AIo();

		int			GetFd() const;
		bool		Fail() const;

	protected:
		int			fd_;
		bool		failed_;
};

#endif
