#include <sys/time.h>
#include "utils.hpp"

std::vector<std::string>	Utils::MySplit
	(std::string const & str, std::string const & separator)
{
	std::vector<std::string>	list;
	std::string::size_type		sep_len;
	std::string::size_type		offset;
	std::string::size_type		pos;
	std::string					tmp;

	sep_len = separator.length();

	if (str == "")
		return (list);

	if (sep_len == 0)
		list.push_back(str);
	else
	{
		offset = 0;
		while (1)
		{
			pos = str.find(separator, offset);
			if (pos == std::string::npos)
			{
				tmp = str.substr(offset);
				if (tmp != "")
					list.push_back(str.substr(offset));
				break;
			}
			else
			{
				tmp = str.substr(offset, pos - offset);
				if (tmp != "")
					list.push_back(tmp);
				offset = pos + sep_len;
			}
		}
	}

	return (list);
}

std::string		Utils::MyTrim
	(const std::string& str, const char* set)
{
	std::string	result;

	std::string::size_type	left = str.find_first_not_of(set);

	if (left != std::string::npos)
	{
		std::string::size_type right = str.find_last_not_of(set);

		result = str.substr(left, right - left + 1);
	}

	return (result);
}

std::string	Utils::GetTimeStampStr()
{
	time_t			now = time(NULL);
	struct tm		current_time;
	char			str[50];
	struct timeval	t;

	gmtime_r(&now, &current_time);
	strftime(str, sizeof(str), "%Y%m%d%H%M%S", &current_time);
	gettimeofday(&t, NULL);
	return (str + ToString(t.tv_usec));
}

std::string	Utils::GetDateStr()
{
	time_t			now = time(NULL);
	struct tm		current_time;
	char			str[50];

	gmtime_r(&now, &current_time);
	strftime(str, sizeof(str), "%a, %d %b %Y %H:%M:%S GMT", &current_time);
	return (str);
}

bool	Utils::IsBlank(char c)
{
	if (c == 0x20 || c == 0x09)
		return (true);
	return (false);
}

std::string	Utils::StringToLower(const std::string& str)
{
	std::string		ret;

	ret = str;
	for (size_t i = 0; i < ret.size(); i++)
	{
		char	c;

		c = ret.at(i);
		if (c >= 'A' && c <= 'Z')
			ret.at(i) = tolower(c);
	}

	return (ret);
}
