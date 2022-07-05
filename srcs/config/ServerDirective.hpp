#ifndef SERVERDIRECTIVE_HPP
# define SERVERDIRECTIVE_HPP

# include <string>
# include <vector>
# include <map>
# include <utility>
# include "LocationDirective.hpp"

class ServerDirective
{
	public:
		ServerDirective(Tokens::citr begin, Tokens::citr end);
		~ServerDirective();

		// Getter
		const std::pair<std::string, int>&		GetListen() const;
		const std::vector<std::string>&			GetServerNames() const;
		const std::vector<LocationDirective>&	GetLocations() const;
	 	// const std::map<int, std::string>&		GetErrorPages() const;
		// const size_t&							GetClientMaxBodySize() const;

	private:
		typedef void (ServerDirective::*SetFunc)(Tokens::citr, Tokens::citr);

		Tokens::citr	GetDirectiveEnd(const std::string& name, Tokens::citr begin, Tokens::citr end) const;
		void			SetDefaultValues();

		// Set Directive Variable
		void	SetListen(Tokens::citr begin, Tokens::citr end);
		void	SetServerNames(Tokens::citr begin, Tokens::citr end);
		void	SetLocation(Tokens::citr begin, Tokens::citr end);
		// void	SetErrorPages(Tokens::citr begin, Tokens::citr end);
		// void	SetClientMaxBodySize(Tokens::citr begin, Tokens::citr end);

		// Directive Variable
		std::pair<std::string, int>			listen_;
		std::vector<std::string> 			server_names_;
		std::vector<LocationDirective>		locations_;
		// std::map<int, std::string> 			error_pages_;
		// size_t								client_max_body_size_;
};

#endif  // SERVERDIRECTIVE_HPP
