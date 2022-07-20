#include "Model.hpp"
#include <iostream>
#include <sstream>

Model::Model(std::string uri)
{
	CreateResponse(uri);
}

Model::~Model() {}
std::string &Model::GetResponse() { return response_; }

void Model::CreateResponse(std::string uri)
{
	CURL *curl;

	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, uri.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
	curl_easy_setopt(curl, CURLOPT_HEADER, 1L);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &buf_);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buf_);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

	curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	std::map<std::string, std::string> header = buf_.GetHeader();

	ModifiyHeader(&header);

	std::map<std::string, std::string>::const_iterator ite = header.begin();
	std::stringstream ss;

	ss << buf_.GetStatus();
	for (; ite != header.end(); ite++)
		ss << ite->first << ": " << ite->second;
	ss << "\r\n";
	ss << buf_.GetBody() << std::endl;

	response_ = ss.str();
}

// C++ではコールバック関数を static宣言するのが必須
// buf: 受信したデータ
// size, nmemb: サイズに関する情報
// user_struct: 任意のデータをコールバック関数側に渡すための構造体ポインタ
size_t Model::header_callback(char *buf, size_t size, size_t nmemb, Buffer *buff)
{
	std::string str(buf);
	size_t cpos = str.find(": ");

	if (cpos == std::string::npos)
	{
		if (str != "\r\n")
		{
			buff->SetStatus(str);
		}
	}
	else
	{
		std::string key = str.substr(0, cpos);
		std::string value = str.substr(cpos + 2);
		buff->SetHeader(key, value);
	}
	return size * nmemb;
}

size_t Model::write_callback(char *buf, size_t size, size_t nmemb, Buffer *buff)
{
	std::stringstream ss;

	ss << buf;
	buff->SetBody(ss.str());
	// libcurlに返す最大バッファサイズ(符号なし整数型)を表し、必ずこれをreturn すること。
	// アップロードが終了する時は0を返す。
	return size * nmemb;
}

void Model::ModifiyHeader(std::map<std::string, std::string> *header)
{
	const std::vector<std::string> remove_elems = {"ETag", "Last-Modified", "Accept-Ranges", "Server"};

	RemoveHeaderElem(header, remove_elems);
	header->insert(std::make_pair("Server", "webserv\r\n"));
}

void Model::RemoveHeaderElem(std::map<std::string, std::string> *header, const std::vector<std::string> &elems)
{
	std::vector<std::string>::const_iterator ite = elems.begin();
	for (; ite < elems.end(); ite++)
		header->erase(*ite);
}
