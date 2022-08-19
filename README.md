# MC7010Tool

mc7010tool is a simple tool to use with ZTE MC7010 5G router.
This was created and tested using MC7010_DNA3_B09 firmware.
I guess it also works with other firmware versions although I haven't been able to test that.

Hopefully this tool is useful for someone.
The router has been buggy and only caused problems for me.
I'll probably change to some other router later and thus won't be updating this tool a lot so feel free to fork and improve this.

## Dependencies

* [libcurl](https://curl.haxx.se/libcurl/) >= 7.32.0
* [librhash](https://github.com/rhash/RHash)
* [jsoncpp](https://github.com/open-source-parsers/jsoncpp)
* [boost](http://www.boost.org/) (program-options)

## Make dependencies
* [cmake](https://cmake.org/) >= 3.0.0
* [pkg-config](https://www.freedesktop.org/wiki/Software/pkg-config/)

### Debian/Ubuntu

    # apt install build-essential libcurl4-openssl-dev libboost-program-options-dev \
    libjsoncpp-dev librhash-dev cmake pkg-config

## Build

    $ mkdir build
    $ cd build
    $ cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release
    $ make

## Options
    -h [ --help ]                  Print help message
    --version                      Print version string
    --router-ip arg (=192.168.8.1) Router ip address
    --router-pw arg (=1234)        Router password
    --get arg                      Get command
    --set arg                      Set command
    --wait arg (=0)                Time to wait between commands in milliseconds


## Usage examples

- **Get single value**

	  mc7010tool --router-ip 192.168.1.1 --router-pw password --get "network_provider"

- **Get multiple values at the same time**

	  mc7010tool --get "network_provider,network_type"

- **Set LTE band mask**

	  mc7010tool --set "goformId=BAND_SELECT&is_gw_band=0&gw_band_mask=0&is_lte_band=1&lte_band_mask=0x02000000045"

- **Set antenna mode to manual**

	  mc7010tool --set "goformId=WAN_ANT_SWITCH_SET&ant_switch_enable=0"

- **Multiple get commands with 2 second wait time between requests**

	  mc7010tool --get "network_provider" --get "network_type" --wait 2000
