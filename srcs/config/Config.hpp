#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <vector>
# include "Tokens.hpp"
# include "ServerDirective.hpp"

class Config
{
	public:
		explicit Config(const std::string& file_path);
		~Config();

		// Getter
		const std::vector<ServerDirective>&	GetServers() const;

	private:
		// Set Directive Variable
		void	SetServer(Tokens::citr start, Tokens::citr end, int *advaced_len);

		// Tokens
		Tokens							tokens_;
		// Directive Variable
		std::vector<ServerDirective>	servers_;
};

#endif  // CONFIG_HPP
