#ifndef LOCATIONDIRECTIVE_HPP
# define LOCATIONDIRECTIVE_HPP

# include <string>
# include <vector>
# include <utility>
# include "Tokens.hpp"

class LocationDirective
{
	public:
		LocationDirective(const std::string& path, Tokens::citr begin, Tokens::citr end);
		~LocationDirective();

		// Getter
		const std::string&					GetPath() const;
		const std::string&					GetRoot() const;
		const std::vector<std::string>&		GetIndex() const;
		// const std::pair<int, std::string>&	GetReturn();
		// const bool&							GetAutoIndex();
		// const std::vector<std::string>&		GetAllowdMethods();

	private:
		typedef void (LocationDirective::*SetFunc)(Tokens::citr, Tokens::citr);

		Tokens::citr	GetDirectiveEnd(const std::string& name, Tokens::citr begin, Tokens::citr end) const;
		void			SetDefaultValues();

		// Set Directive Variable
		void	SetRoot(Tokens::citr begin, Tokens::citr end);
		void	SetIndex(Tokens::citr begin, Tokens::citr end);
		// void	SetReturn(Tokens::citr begin, Tokens::citr end);
		// void	SetAutoIndex(Tokens::citr begin, Tokens::citr end);
		// void	SetAllowedMethods(Tokens::citr begin, Tokens::citr end);

		// Location Path
		std::string					path_;

		// Directive Variable
		std::string					root_;
		std::vector<std::string>	index_;
		// std::pair<int, std::string>	return_;
		// bool						autoindex_;
		// std::vector<std::string>	allowed_methods_;
};

#endif  // LOCATIONDIRECTIVE_HPP
