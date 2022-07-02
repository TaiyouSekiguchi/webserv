#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <vector>
// # include "ServerDirective.hpp"

class Config
{
	public:
		explicit Config(const std::string& file_path);
		~Config();

		const std::vector<std::string>&	GetTokens() const;

	private:
		std::vector<std::string>	tokens_;
		// std::vector<ServerDirective*>	servers_;

		void			LexConfigFile(const std::string& file_path);
		std::string		GetFileContent(const std::string& file_path);
		std::string		GetToken(const std::string& content, const std::string::size_type& start);
};

#endif  // CONFIG_HPP
