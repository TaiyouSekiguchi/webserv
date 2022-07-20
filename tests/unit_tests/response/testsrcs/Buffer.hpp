#ifndef BUFFER_HPP
# define BUFFER_HPP

# include <string>
# include <map>

class Buffer
{
	public:
		Buffer();
		~Buffer();

		std::string GetBody() const;
		std::string GetStatus() const;
		std::map<std::string, std::string> GetHeader() const;

		void SetBody(const std::string &str);
		void SetStatus(const std::string &str);
		void SetHeader(const std::string &key, const std::string &value);

	private:
		std::string status_;
		std::map<std::string, std::string> header_;
		std::string body_;
};

#endif
