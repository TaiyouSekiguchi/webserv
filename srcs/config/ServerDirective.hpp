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
		typedef std::pair<unsigned int, int>	Listen;

		ServerDirective(Tokens::citr begin, Tokens::citr end);
		~ServerDirective();

		// Getter
		const std::vector<std::pair<unsigned int, int> >&	GetListen() const;
		const std::vector<std::string>&						GetServerNames() const;
	 	const std::map<int, std::string>&					GetErrorPages() const;
		const long&											GetClientMaxBodySize() const;
		const std::vector<LocationDirective>&				GetLocations() const;

	private:
		typedef void (ServerDirective::*ParseFunc)(Tokens::citr, Tokens::citr);

		Tokens::citr	GetDirectiveEnd(const std::string& name, Tokens::citr begin, Tokens::citr end) const;
		void			SetInitValue();
		void			SetDefaultValue();

		// Parse Directive Variable
		void	ParseListen(Tokens::citr begin, Tokens::citr end);
		void	ParseServerNames(Tokens::citr begin, Tokens::citr end);
		void	ParseLocation(Tokens::citr begin, Tokens::citr end);
		void	ParseErrorPages(Tokens::citr begin, Tokens::citr end);
		void	ParseClientMaxBodySize(Tokens::citr begin, Tokens::citr end);

		// Directive Variable
		std::vector<std::pair<unsigned int, int> >	listen_;
		std::vector<std::string> 					server_names_;
		std::vector<LocationDirective>				locations_;
		std::map<int, std::string> 					error_pages_;
		long										client_max_body_size_;
};

#endif  // SERVERDIRECTIVE_HPP
