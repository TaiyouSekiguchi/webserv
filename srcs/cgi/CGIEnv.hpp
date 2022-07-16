#ifndef CGIENV_HPP
# define CGIENV_HPP

# include <string.h>
# include <string>
# include <vector>
# include <utility>

class CGIEnv
{
	public:
		CGIEnv(void);
		~CGIEnv(void);

		char**	GetEnv(void);

	private:
		typedef	std::pair<std::string, std::string> pair;

		void	SetEnv(void);

		std::vector<pair>	env_;
};

#endif
