#ifndef CONFIG_HPP
# define CONFIG_HPP

# include "ServerDirective.hpp"

class Config
{
	public:
		Config();
		~Config();

		void	ParseConfigFile();

	private:
		std::vector<ServerDirective*>	servers_;
};

#endif  // CONFIG_HPP
