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
		int			fd_;
		bool		failed_;
};

#endif
