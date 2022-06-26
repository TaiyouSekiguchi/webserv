#ifndef DEBUG_HPP
#define DEBUG_HPP

# include <iostream>

# ifdef PRINT_FLAG
static const bool kPrintFlag = true;
# else
static const bool kPrintFlag = false;
# endif

template <typename T>
void	MyPrint(const T& msg)
{
	if (kPrintFlag)
		std::cout << msg << std::endl;
}

#endif
