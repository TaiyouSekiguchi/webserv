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
	template <typename T, typename F, typename V>
	typename std::vector<T>::const_iterator		FindMatchMember(const std::vector<T>& vec, const F get_func, const V& val);
	template <typename T, typename F, typename V>
	typename std::vector<T*>::const_iterator	FindMatchMember(const std::vector<T*>& vec, const F get_func, const V& val);
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

template <typename T, typename F, typename V>
typename std::vector<T>::const_iterator	Utils::FindMatchMember
	(const std::vector<T>& vec, const F get_func, const V& val)
{
	typename std::vector<T>::const_iterator	itr = vec.begin();
	typename std::vector<T>::const_iterator	end = vec.end();

	while (itr != end)
	{
		if (((*itr).*(get_func))() == val)
			return (itr);
		++itr;
	}
	return (end);
}

template <typename T, typename F, typename V>
typename std::vector<T*>::const_iterator	Utils::FindMatchMember
		(const std::vector<T*>& vec, const F get_func, const V& val)
{
	typename std::vector<T*>::const_iterator	itr = vec.begin();
	typename std::vector<T*>::const_iterator	end = vec.end();

	while (itr != end)
	{
		if (((*itr)->*(get_func))() == val)
			return (itr);
		++itr;
	}
	return (end);
}

#endif
