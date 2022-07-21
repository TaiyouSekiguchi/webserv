#ifndef MODEL_HPP
# define MODEL_HPP
# include <vector>
# include <curl/curl.h>
# include "Buffer.hpp"

class Model
{
	public:
		explicit Model(std::string uri);
		Model(std::string uri, const std::vector<std::string> &rm_headers);
		~Model();

		std::string &GetResponse();
		const std::map<std::string, std::string> &GetHeader() const;

	private:
		const Buffer buf_;
		std::map<std::string, std::string> header_;
		std::string response_;
		const std::vector<std::string> rm_headers_;

		void CurlSetup(std::string uri);
		void CreateResponse();
		void ModifiyHeader(std::map<std::string, std::string> *header);
		void RemoveHeaderElem(std::map<std::string, std::string> *header, const std::vector<std::string> &elems);
		static size_t header_callback(char *buf, size_t size, size_t nmemb, Buffer *buff);
		static size_t write_callback(char *buf, size_t size, size_t nmemb, Buffer *buff);
};

#endif
