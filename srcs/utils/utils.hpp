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

	template <typename T>
	std::string		ToString(const T& n);
	template <typename T>
	bool			IsNotFound(const std::vector<T>& vec, const T& val);
}  // namespace Utils

template <typename T>
std::string		Utils::ToString(const T& n)
{
	std::stringstream	ss;
	ss << n;
	return (ss.str());
}

template <typename T>
bool	Utils::IsNotFound(const std::vector<T>& vec, const T& val)
{
	typename std::vector<T>::const_iterator	end = vec.end();
	return (std::find(vec.begin(), end, val) == end);
}

#endif
