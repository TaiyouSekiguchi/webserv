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
	bool 			MyisLower(const char ch);
	std::string		toString(const long n);

	template <typename T>
	bool	isNotFound(const std::vector<T> container, const T value);
}  // namespace Utils

template <typename T>
bool	Utils::isNotFound(const std::vector<T> container, const T value)
{
	typename std::vector<T>::const_iterator	end = container.end();
	return (std::find(container.begin(), end, value) == end);
}

#endif
