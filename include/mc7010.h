/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details. */

#ifndef MC7010_H
#define MC7010_H

#include <curl/curl.h>
#include <json/json.h>
#include <tuple>

class MC7010
{
	public:
		MC7010(const std::string &router_ip, const std::string &router_pw);
		bool login();
		Json::Value cmdProcess_Set(const std::string &command);
		Json::Value cmdProcess_Get(const std::string &command);
		virtual ~MC7010();
	private:
		std::string getAD();
		std::string ip;
		std::string pw;
		CURL* curlhandle;
};

#endif // MC7010_H
