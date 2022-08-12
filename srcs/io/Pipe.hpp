#ifndef PIPE_HPP
# define PIPE_HPP

# include <unistd.h> //  write(), read(), close(), pipe(), dup2()
# include <fcntl.h> // fcntl() 

class Pipe
{
	public:
		enum	e_PipeIo
		{
			READ = 0,
			WRITE = 1
		};

		Pipe(void);
		~Pipe(void);

		int			OpenPipe(void);
		int			ClosePipe(e_PipeIo type);
		void		NonBlockingPipe(e_PipeIo type) const;
		int			WriteToPipe(void* buf, unsigned int byte) const;
		ssize_t		ReadFromPipe(void* buf, size_t byte) const;
		int			RedirectToPipe(e_PipeIo type, int fd);
		int			GetPipeFd(e_PipeIo type) const;

	private:
		int				pipe_[2];
};

#endif
