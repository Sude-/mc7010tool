/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details. */

#ifndef UTIL_H
#define UTIL_H

#include <curl/curl.h>
#include <rhash.h>
#include <tuple>
#include <iostream>

namespace Util
{
	namespace Curl
	{
		size_t writeCallback(char *ptr, size_t size, size_t nmemb, void *userp);
		std::tuple<std::string, CURLcode> get(CURL* curlhandle, const std::string &url);
		std::tuple<std::string, CURLcode> post(CURL* curlhandle, const std::string &url, const std::string &data);
	};
	namespace Rhash
	{
		std::string getHash(const std::string &str, const unsigned &hash_id);
		std::string md5(const std::string &str);
		std::string sha256(const std::string &str);
	};
	std::string uppercase(const std::string &str);
};

#endif // UTIL_H
