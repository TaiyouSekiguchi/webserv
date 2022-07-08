#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>

namespace Utils
{

	std::vector<std::string>	MySplit
		(std::string const & str, std::string const & separator);
	std::string		MyTrim
		(const std::string& str, const char* set = " \t\v\r\n");

}

#endif
