#include <iostream>
#include "HTTPServer.hpp"

int	main()
{
	try
	{
		HTTPServer hserver;
		hserver.Start();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (0);
}
