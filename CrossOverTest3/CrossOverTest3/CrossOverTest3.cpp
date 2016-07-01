// CrossOverTest3.cpp : definisce il punto di ingresso dell'applicazione console.
//

#include "stdafx.h"

#include <iostream>

#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace utility;
using namespace web::http;
using namespace web::http::client;
using namespace concurrency::streams;

#ifdef _WIN32
int wmain(int argc, wchar_t *args[])
#else
int main(int argc, char *args[])
#endif
{
	if (argc != 3)
	{
		printf("Usage: CrossOverTest3.exe search_term output_file\n");
		return -1;
	}
	const string_t searchTerm = args[1];
	const string_t outputFileName = args[2];

	// Open a stream to the file to write the HTTP response body into.
	auto fileBuffer = std::make_shared<streambuf<uint8_t>>();
	auto task = file_buffer<uint8_t>::open(outputFileName, std::ios::out).then([=](streambuf<uint8_t> outFile) -> pplx::task<http_response>
	{
		*fileBuffer = outFile;

		// Create an HTTP request.
		// Encode the URI query since it could contain special characters like spaces.
		http_client client(U("http://www.bing.com/"));
		return client.request(methods::GET, uri_builder(U("/search")).append_query(U("q"), searchTerm).to_string());
	})

		// Write the response body into the file buffer.
		.then([=](http_response response) /*-> pplx::task<size_t*>*/
	{
		printf("Response status code %u returned.\n", response.status_code());

		auto body = response.extract_string().get();		
		
		std::wstring delimiter = L"<";

		size_t pos = 0;
		std::wstring token;
		while ((pos = body.find(delimiter)) != std::string::npos) {
			token = body.substr(0, pos);
			std::wcout << token << std::endl;
			body.erase(0, pos + delimiter.length());
		}		
	});	

	task.wait();

	system("pause");
	return 0;
}

