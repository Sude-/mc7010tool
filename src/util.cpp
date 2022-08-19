/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details. */

#include "util.h"
#include <sstream>
#include <algorithm>
#include <cstring>

using namespace std;

size_t Util::Curl::writeCallback(char *ptr, size_t size, size_t nmemb, void *userp)
{
	ostringstream *stream = (ostringstream*)userp;
	streamsize count = (streamsize) size * nmemb;
	stream->write(ptr, count);
	return count;
}

std::tuple<std::string, CURLcode> Util::Curl::get(CURL* curlhandle, const string &url)
{
	CURLcode result;
	string response;
	ostringstream stream;

	curl_easy_setopt(curlhandle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, Util::Curl::writeCallback);
	curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, &stream);
	curl_easy_setopt(curlhandle, CURLOPT_HTTPGET, 1L);

	result = curl_easy_perform(curlhandle);
	response = stream.str();
	stream.str(string());

	return {response, result};
}

std::tuple<std::string, CURLcode> Util::Curl::post(CURL* curlhandle, const string &url, const string &data)
{
	CURLcode result;
	string response;
	ostringstream stream;

	curl_easy_setopt(curlhandle, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, Util::Curl::writeCallback);
	curl_easy_setopt(curlhandle, CURLOPT_WRITEDATA, &stream);
	curl_easy_setopt(curlhandle, CURLOPT_POSTREDIR, CURL_REDIR_POST_ALL);
	curl_easy_setopt(curlhandle, CURLOPT_POST, 1L);
	curl_easy_setopt(curlhandle, CURLOPT_POSTFIELDS, data.c_str());

	result = curl_easy_perform(curlhandle);
	response = stream.str();
	stream.str(string());

	return {response, result};
}

std::string Util::Rhash::getHash(const string &str, const unsigned &hash_id)
{
    unsigned char digest[rhash_get_digest_size(hash_id)];
    char result[rhash_get_hash_length(hash_id) + 1];

    int res = rhash_msg(hash_id, str.c_str(), str.length(), digest);
    if (res < 0)
        cerr << "LibRHash error: " << strerror(errno) << endl;
    else
        rhash_print_bytes(result, digest, rhash_get_digest_size(hash_id), RHPR_HEX);

    return string(result);
}

std::string Util::Rhash::md5(const string &str)
{
	return Util::Rhash::getHash(str, RHASH_MD5);
}

std::string Util::Rhash::sha256(const string &str)
{
	return Util::Rhash::getHash(str, RHASH_SHA256);
}

std::string Util::uppercase(const string &str)
{
	string uppercased;
	uppercased.resize(str.size());
	transform(str.begin(), str.end(), uppercased.begin(), ::toupper);
	return uppercased;
}
