#!/bin/sh

echo "Preparing OpenVPN build environment"
pkg_add -I sudo-- git autoconf-2.69p2 automake-1.15.1 \
		fping lzo2 lz4 libtool mbedtls cmake

# OpenBSD wants to be told which auto* version to run (can have multiple)
echo "setting up AUTO_* environment variables (/etc/profile)"
echo "export AUTOCONF_VERSION=2.69" >>/etc/profile
echo "export AUTOMAKE_VERSION=1.15" >>/etc/profile

# OpenBSD puts lzo into /usr/local/{include,lib} where our configure 
# will not find it -> make symlinks
echo "creating lzo2 convenience symlinks..."
cd /usr/include && ln -s ../local/include/lzo .
cd /usr/lib && ln -s ../local/lib/liblzo2.* .

# same thing for mbedtls...
echo "creating mbedTLS convenience symlinks..."
cd /usr/include && ln -s ../local/include/mbedtls .
cd /usr/lib && ln -s ../local/lib/libmbed* .

echo "done!"
exit 0
