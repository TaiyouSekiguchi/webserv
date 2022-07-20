#ifndef MODEL_HPP
# define MODEL_HPP
# include <vector>
# include <curl/curl.h>
# include "Buffer.hpp"

class Model
{
	public:
		explicit Model(std::string uri);
		~Model();

		std::string &GetResponse();

	private:
		const Buffer buf_;
		std::string response_;

		void CreateResponse(std::string uri);
		void ModifiyHeader(std::map<std::string, std::string> *header);
		void RemoveHeaderElem(std::map<std::string, std::string> *header, const std::vector<std::string> &elems);
		static size_t header_callback(char *buf, size_t size, size_t nmemb, Buffer *buff);
		static size_t write_callback(char *buf, size_t size, size_t nmemb, Buffer *buff);
};

#endif
