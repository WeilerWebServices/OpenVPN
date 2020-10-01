#!/bin/sh

echo "Preparing OpenVPN 2 build environment"
echo
yum -y install lzo-devel openssl-devel pam-devel pkcs11-helper-devel gnutls-devel autoconf libtool make cmake git net-tools
echo
echo "Preparing OpenVPN 3 build enviroment"
echo
yum install -y epel-release
yum install -y mbedtls-devel glib2-devel jsoncpp-devel libuuid-devel lz4-devel gcc-c++ git autoconf automake make pkgconfig fping

