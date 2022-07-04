#ifndef CLIENTCLOSED_HPP
# define CLIENTCLOSED_HPP

# include <stdexcept>

class ClientClosed : public std::exception
{
};

#endif  // CLIENTCLOSED_HPP
