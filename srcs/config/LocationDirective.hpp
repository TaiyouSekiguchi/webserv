#ifndef LOCATIONDIRECTIVE_HPP
# define LOCATIONDIRECTIVE_HPP

# include <string>
# include <vector>
# include <utility>
# include "Tokens.hpp"
# include "HTTPStatusCode.hpp"

class LocationDirective
{
	public:
		LocationDirective();
		LocationDirective(const std::string& path, Tokens::citr begin, Tokens::citr end);
		~LocationDirective();

		// Getter
		const std::string&					GetPath() const;
		const std::string&					GetRoot() const;
		const std::vector<std::string>&		GetIndex() const;
		const std::pair<e_StatusCode, std::string>&	GetReturn() const;
		const int&							GetAutoIndex() const;
		const std::vector<std::string>&		GetAllowedMethods() const;
		const std::string&					GetUploadRoot() const;
		const std::vector<std::string>&		GetCGIEnableExtension() const;

	private:
		typedef void (LocationDirective::*ParseFunc)(Tokens::citr, Tokens::citr);

		Tokens::citr	GetDirectiveEnd(const std::string& name, Tokens::citr begin, Tokens::citr end) const;
		void			SetInitValue();
		void			SetDefaultValue();

		// Parse Directive Variable
		void	ParseRoot(Tokens::citr begin, Tokens::citr end);
		void	ParseIndex(Tokens::citr begin, Tokens::citr end);
		void	ParseReturn(Tokens::citr begin, Tokens::citr end);
		void	ParseAutoIndex(Tokens::citr begin, Tokens::citr end);
		void	ParseAllowedMethods(Tokens::citr begin, Tokens::citr end);
		void	ParseUploadRoot(Tokens::citr begin, Tokens::citr end);
		void	ParseCGIEnableExtension(Tokens::citr begin, Tokens::citr end);

		// Location Path
		std::string						path_;

		// Directive Variable
		std::string						root_;
		std::vector<std::string>		index_;
		std::pair<e_StatusCode, std::string>		return_;
		int								autoindex_;
		std::vector<std::string>		allowed_methods_;
		std::string						upload_root_;
		std::vector<std::string>		cgi_enable_extension_;
};

#endif  // LOCATIONDIRECTIVE_HPP
