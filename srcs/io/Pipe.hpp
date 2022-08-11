#ifndef PIPE_HPP
# define PIPE_HPP

class Pipe
{
	public:
		Pipe();
		~Pipe();

		void		CloseReadPipe(void)
		void		CloseWritePipe(void)
		int			WriteToPipe(void* buf, unsigned int byte)
		ssize_t		ReadFromPipe(void* buf, size_t byte)
		int			StdinRedirectToReadPipe(void)
		int			StdoutRedirectToWritePipe(void)

	private:
		enum	e_PipeIo
		{
			READ = 0,
			WRITE = 1
		}

		void		OpenPipe(void);
		void		NonBlockingPipe(void);

		int			pipe_[2];
};

#endif
