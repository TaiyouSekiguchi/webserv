#ifndef PIPE_HPP
# define PIPE_HPP

class Pipe
{
	public:
		enum	e_PipeIo
		{
			READ = 0,
			WRITE = 1
		}

		Pipe(e_PipeIo type);
		~Pipe();

		int			OpenNonBlockingPipe(void)
		int			CloseUnusedPipeInParentProcess(void)
		int			CloseUnusedPipeInChildProcess(void)
		int			WriteToPipe(void* buf, unsigned int byte)
		ssize_t		ReadFromPipe(void* buf, size_t byte)
		int			RedirectToPipe(void)

	private:
		int			pipe_[2];
		e_PipeIo	type_;
};

#endif
