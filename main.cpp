/* This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar. See
 * http://www.wtfpl.net/ for more details. */

#include "mc7010.h"
#include <boost/program_options.hpp>
#include <rhash.h>
#include <signal.h>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
namespace bpo = boost::program_options;

int main(int argc, char *argv[])
{
    // Disable SIGPIPE
    struct sigaction act;
    act.sa_handler = SIG_IGN;
    act.sa_flags = SA_RESTART;
    sigemptyset(&act.sa_mask);
    if (sigaction(SIGPIPE, &act, NULL) < 0)
        return 1;

    std::string program_name = argv[0];
    std::string usage_examples_text = string("Usage examples:\n\n") + \
        "Get single value\n" + \
        program_name +  " --router-ip 192.168.1.1 --router-pw password --get \"network_provider\"\n\n" + \
        "Get multiple values at the same time\n" + \
        program_name +  " --get \"network_provider,network_type\"\n\n" + \
        "Set LTE band mask\n" + \
        program_name +  " --set \"goformId=BAND_SELECT&is_gw_band=0&gw_band_mask=0&is_lte_band=1&lte_band_mask=0x02000000045\"\n\n" + \
        "Set antenna mode to manual\n" + \
        program_name +  " --set \"goformId=WAN_ANT_SWITCH_SET&ant_switch_enable=0\"\n\n" + \
        "Multiple get commands with 2 second wait time between requests\n" + \
        program_name +  " --get \"network_provider\" --get \"network_type\" --wait 2000";

    string router_ip;
    string router_pw;
    int wait_ms;
    vector<string> set_command;
    vector<string> get_command;

    vector<string> unrecognized_options;
    bpo::variables_map vm;
    bpo::options_description options("Options");
    try
    {
        options.add_options()
            ("help,h", "Print help message")
            ("version", "Print version string")
            ("router-ip", bpo::value<string>(&router_ip)->default_value("192.168.8.1"), "Router ip address")
            ("router-pw", bpo::value<string>(&router_pw)->default_value("1234"), "Router password")
            ("get", bpo::value< vector<string> >(&get_command)->composing(), "Get command")
            ("set", bpo::value< vector<string> >(&set_command)->composing(), "Set command")
            ("wait", bpo::value<int>(&wait_ms)->default_value(0), "Time to wait between commands in milliseconds")
        ;

        bpo::parsed_options parsed = bpo::parse_command_line(argc, argv, options);
        bpo::store(parsed, vm);
        unrecognized_options = bpo::collect_unrecognized(parsed.options, bpo::include_positional);
        bpo::notify(vm);

        if (vm.count("help") || argc < 2)
        {
            cout << VERSION_STRING << endl << endl << options << endl;
            cout << endl << usage_examples_text << endl;
            return 0;
        }

        if (vm.count("version"))
        {
            cout << VERSION_STRING << endl;
            return 0;
        }
    }
    catch (exception& e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    catch (...)
    {
        cerr << "Exception of unknown type!" << endl;
        return 1;
    }

    if (!unrecognized_options.empty())
    {
        cout << "Unrecognized option" << endl;
        for (auto option : unrecognized_options)
            cout << option << endl;
    }

    // Init curl and rhash
    curl_global_init(CURL_GLOBAL_ALL);
    rhash_library_init();

    MC7010 mc7010(router_ip, router_pw);

    if (!mc7010.login())
    {
        cerr << "Login failed" << endl;
        curl_global_cleanup();
        return 1;
    }

    int res = 0;
    Json::Value json;

    if (!get_command.empty())
    {
        for (auto cmd : get_command)
        {
            if (wait_ms > 0)
                this_thread::sleep_for(chrono::milliseconds(wait_ms));
            json = mc7010.cmdProcess_Get(cmd);
            if (json.empty())
            {
                res += 1;
                cerr << "Get: " << cmd << endl << "Failed" << endl;
                continue;
            }
            cout << "Get: " << cmd << endl << json << endl;
        }
    }

    if (!set_command.empty())
    {
        for (auto cmd : set_command)
        {
            if (wait_ms > 0)
                this_thread::sleep_for(chrono::milliseconds(wait_ms));
            json = mc7010.cmdProcess_Set(cmd);
            if (json.empty())
            {
                res += 1;
                cerr << "Set: " << cmd << endl << "Failed" << endl;
                continue;
            }
            cout << "Set: " << cmd << endl << json << endl;
        }
    }

    // Cleanup curl
    curl_global_cleanup();

    return res;
}
