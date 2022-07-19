#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <sstream>

namespace Utils
{
	std::vector<std::string>	MySplit
		(std::string const & str, std::string const & separator);
	std::string		MyTrim
		(const std::string& str, const char* set = " \t\v\r\n");
	bool 			MyisLower(const char ch);
	std::string		GetMicroSecondTime();
	bool			IsNotFound(const std::vector<std::string>& vec, const std::string& val);

	template <typename T>
	std::string		ToString(const T& n);
}  // namespace Utils

template <typename T>
std::string		Utils::ToString(const T& n)
{
	std::stringstream	ss;
	ss << n;
	return (ss.str());
}

#endif
