#include <fstream>
#include <stdexcept>
#include "Config.hpp"

Config::Config(const std::string& file_path)
{
	LexConfigFile(file_path);
}

Config::~Config()
{
	// std::vector<ServerDirective*>::iterator	it = servers_.begin();
	// std::vector<ServerDirective*>::iterator	end = servers_.end();
	// while (it != end)
	// {
	// 	delete *it;
	// 	++it;
	// }
}

const std::vector<std::string>&	Config::GetTokens() const { return (tokens_); }

std::string	Config::GetFileContent(const std::string& file_path)
{
	std::ifstream ifs(file_path);
	if (ifs.fail())
		std::runtime_error("open error");
	std::istreambuf_iterator<char> it(ifs);
	std::istreambuf_iterator<char> last;
	std::string content(it, last);
	return (content);
}

std::string	Config::GetToken(const std::string& content, const std::string::size_type& start)
{
	const char 				seps[] = {';', '{', '}'};
	std::string::size_type	i = start;
	char					quote;

	if (std::find(seps, &seps[3], content[start]) != &seps[3])
		return (content.substr(start, 1));

	while (i < content.length()
		&& !std::isspace(content[i])
		&& std::find(seps, &seps[3], content[i]) == &seps[3])
	{
		if (content[i] == '\'' || content[i] == '\"')
		{
			quote = content[i++];
			while (i < content.length() && content[i] != quote)
				i++;
			if (i == content.length())
				throw std::runtime_error("syntex error");
		}
		i++;
	}
	return (content.substr(start, i - start));
}

void	Config::LexConfigFile(const std::string& file_path)
{
	std::string::size_type		i;
	std::string					token;
	const std::string&			content = GetFileContent(file_path);

	i = 0;
	while (i < content.length())
	{
		while (i < content.length() && std::isspace(content[i]))
			i++;
		if (i == content.length())
			break;
		const std::string&	token = GetToken(content, i);
		tokens_.push_back(token);
		i += token.length();
	}
}
