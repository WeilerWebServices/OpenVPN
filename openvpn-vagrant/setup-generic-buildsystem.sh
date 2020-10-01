#!/bin/sh
#
# Script to setup the environment for openvpn-build/generic and openvpn-build/windows-nsis 
#
# This script works on Ubuntu 16.04 and 18.04.

CODENAME=`lsb_release -cs`
BUILD_DEPS="libpam0g-dev mingw-w64 man2html dos2unix nsis unzip wget curl autoconf libtool gcc-arm-linux-gnueabi python3-docutils"
OSSLSIGNCODE_URL="https://github.com/mtrojnar/osslsigncode.git"
OPENVPN_BUILD_URL="https://github.com/OpenVPN/openvpn-build.git"
MINGW_PACKAGES="mingw-w64 mingw-w64-common mingw-w64-i686-dev mingw-w64-x86-64-dev"
NSIS_PACKAGES="nsis nsis-common nsis-doc nsis-pluginapi"
GIT_PKG="git"
OSSLSIGNCODE_DEPS="build-essential autoconf libtool libssl-dev python3-pkgconfig libcurl4-openssl-dev libgsf-1-dev"

check_if_root() {
    if ! [ `whoami` = "root" ]; then
            echo "ERROR: you must run this script as root!"
            exit 1
    fi
}

install_packages() {
    apt-get update
    apt-get -y install $BUILD_DEPS $GIT_PKG $GNUEABI_PKG $MINGW_PACKAGES $NSIS_PACKAGES
}

# osslsigncode is required for signing the binaries and installers
install_osslsigncode() {
    apt-get -y install $OSSLSIGNCODE_DEPS
    git clone $OSSLSIGNCODE_URL
    cd osslsigncode
    ./autogen.sh
    ./configure
    make
    make install
    cd ..
}

clone_openvpn_build() {
    if ! [ -d "openvpn-build" ]; then
        git clone $OPENVPN_BUILD_URL
    fi
}

# Main script
check_if_root
install_packages
install_osslsigncode
clone_openvpn_build
