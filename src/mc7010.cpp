/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details. */

#include "mc7010.h"
#include "util.h"
#include <iostream>

using namespace std;

MC7010::MC7010(const string &router_ip, const string &router_pw)
{
	this->ip = router_ip;
	this->pw = router_pw;
	string cookielist = "#HttpOnly_" + router_ip;

	// Initialize curl handle and set default options
	curlhandle = curl_easy_init();
	curl_easy_setopt(curlhandle, CURLOPT_NOPROGRESS, 1L);
	curl_easy_setopt(curlhandle, CURLOPT_WRITEFUNCTION, Util::Curl::writeCallback);
	curl_easy_setopt(curlhandle, CURLOPT_NOSIGNAL, 1L);
	curl_easy_setopt(curlhandle, CURLOPT_FAILONERROR, 1L);
	curl_easy_setopt(curlhandle, CURLOPT_COOKIELIST, cookielist.c_str());
	curl_easy_setopt(curlhandle, CURLOPT_TRANSFER_ENCODING, 1L);
	curl_easy_setopt(curlhandle, CURLOPT_ACCEPT_ENCODING, "");
	curl_easy_setopt(curlhandle, CURLOPT_FOLLOWLOCATION, 1L);
	//curl_easy_setopt(curlhandle, CURLOPT_VERBOSE, 1L);
}

MC7010::~MC7010()
{
	curl_easy_cleanup(curlhandle);
}

bool MC7010::login()
{
	using namespace Util::Rhash;
	using Util::uppercase;
	bool res = false;

	// Get LD value and create hashed password
	Json::Value json = this->cmdProcess_Get("LD");

	string LD = json["LD"].asString();
	string pw_ld = uppercase(sha256(this->pw) + LD);
	string pw_ld_sha256 = uppercase(sha256(pw_ld));

	// Perform login
	string command = "goformId=LOGIN&password=" + pw_ld_sha256;
	json = this->cmdProcess_Set(command);

	if (json["result"].asString() == "0")
		res = true;

	return res;
}

Json::Value MC7010::cmdProcess_Set(const string &command)
{
	Json::Value json;
	CURLcode result;
	string response;
	string url = "http://" + this->ip + "/goform/goform_set_cmd_process";
	string cmd = "isTest=false&" + command + "&AD=" + this->getAD();

	// Set header
	struct curl_slist *header = NULL;
	string referer = "Referer: http://" + this->ip + "/index.html";
	string content_type = "Content-Type: application/x-www-form-urlencoded; charset=UTF-8";
	header = curl_slist_append(header, referer.c_str());
	header = curl_slist_append(header, content_type.c_str());
	curl_easy_setopt(curlhandle, CURLOPT_HTTPHEADER, header);

	tie(response, result) = Util::Curl::post(curlhandle, url, cmd);

	// Free header
	curl_slist_free_all(header);

	if (result != CURLE_OK)
	{
		cerr << "MC7010::cmdProcess_Set result != CURLE_OK" << endl;
		exit(1);
	}

	// Parse JSON
    std::istringstream json_stream(response);

    try
    {
        json_stream >> json;
    }
    catch(const Json::Exception& exc)
    {
        cerr << exc.what() << endl;
    }

	return json;
}

Json::Value MC7010::cmdProcess_Get(const string &command)
{
	// Check if we request multiple data values with the same command
	int multi_data = 0;
	if (command.find(",") != string::npos)
		multi_data = 1;

	Json::Value json;
	string response;
	CURLcode result;
	string cmd = "multi_data=" + to_string(multi_data) + "&cmd=" + command;
	string url = "http://" + this->ip + "/goform/goform_get_cmd_process?" + cmd;

	// Set header
	struct curl_slist *header = NULL;
	string referer = "Referer: http://" + this->ip + "/index.html";
	header = curl_slist_append(header, referer.c_str());
	curl_easy_setopt(curlhandle, CURLOPT_HTTPHEADER, header);

	tie(response, result) = Util::Curl::get(curlhandle, url);

	// Free header
	curl_slist_free_all(header);

	if (result != CURLE_OK)
	{
		cerr << "MC7010::cmdProcess_Get result != CURLE_OK" << endl;
		exit(1);
	}

	// Parse JSON
    std::istringstream json_stream(response);

    try
    {
        json_stream >> json;
    }
    catch(const Json::Exception& exc)
    {
        cerr << exc.what() << endl;
    }

	return json;
}

// Create AD value needed for set commands
// AD = md5( md5(wa_inner_version+cr_version) + RD )
string MC7010::getAD()
{
	using namespace Util::Rhash;

	string AD;
	string command = "RD,wa_inner_version,cr_version";
	Json::Value json = this->cmdProcess_Get(command);

	string RD = json["RD"].asString();
	string wa_inner_version = json["wa_inner_version"].asString();
	string cr_version = json["cr_version"].asString();

	AD = md5(md5(wa_inner_version + cr_version) + RD);

	return AD;
}
