#ifndef ASOCKET_HPP
# define ASOCKET_HPP

class ASocket
{
	public:
		explicit ASocket(const int fd);
		virtual ~ASocket();

		int		GetFd() const;

	protected:
		int		fd_;
};

#endif
