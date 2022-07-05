#ifndef TOKENS_HPP
# define TOKENS_HPP

# include <string>
# include <vector>

class Tokens
{
	public:
		typedef	std::vector<std::string>::const_iterator	citr;

		explicit Tokens(const std::string& file_path);
		~Tokens();

		// Getter
		citr	begin() const;
		citr	end() const;

		// Utils
		static citr		GetEndBracesItr(citr braces_begin, citr end);
		static bool		isSpecialToken(const std::string& s);

	private:
		// Lexer
		void		LexConfigFile(const std::string& file_path);
		std::string	GetFileContent(const std::string& file_path) const;
		bool		isSpecialSymbol(const char c) const;
		std::string	GetToken(const std::string& content, const std::string::size_type& start) const;

		// Tokens
		std::vector<std::string>	tokens_;
};

#endif
