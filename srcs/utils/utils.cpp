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

bool	Utils::MyisLower(const char ch)
{
    return std::islower(static_cast<unsigned char>(ch));
}

bool	Utils::IsNotFound(const std::vector<std::string>& vec, const std::string& val)
{
	std::vector<std::string>::const_iterator	end = vec.end();
	return (std::find(vec.begin(), end, val) == end);
}

std::string		Utils::GetMicroSecondTime()
{
	timeval		t;
	gettimeofday(&t, NULL);
	return (ToString(t.tv_sec * 1000000 + t.tv_usec));
}

bool	Utils::IsBlank(char c)
{
	if (c == 0x20 || c == 0x09)
		return (true);
	return (false);
}

bool	Utils::IsToken(const std::string& str)
{
	std::string::const_iterator		it;
	std::string::const_iterator		it_end;

	it = str.begin();
	it_end = str.end();

	for ( ; it != it_end; ++it)
	{
		if (!IsTChar(*it))
			return (false);
	}
	return (true);
}

bool	Utils::IsTChar(char c)
{
	if (c == '!'
		|| c == '#'
		|| c == '$'
		|| c == '%'
		|| c == '&'
		|| c == '\''
		|| c == '*'
		|| c == '+'
		|| c == '-'
		|| c == '.'
		|| c == '^'
		|| c == '_'
		|| c == '`'
		|| c == '|'
		|| c == '~'
		|| isdigit(c)
		|| isalpha(c))
	{
		return (true);
	}
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
