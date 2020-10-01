#!/bin/sh

echo "Preparing OpenVPN build environment"
echo "installing packages... this will take a while if your network is slow"

# https://www.netbsd.org/docs/pkgsrc/using.html
PATH="/usr/pkg/sbin:$PATH"
PKG_PATH="ftp://ftp.NetBSD.org/pub/pkgsrc/packages/NetBSD/amd64/7.0.2/All/"
export PATH PKG_PATH

# "-U" is needed to get cmake (because libarchive needs to be updated)
pkg_add -U automake autoconf libtool fping git lzo lz4 mbedtls cmake

# the fping package is stupid and is missing the "fping6" symlink
echo "fixing missing fping6 symlink..."
cd /usr/pkg/sbin && ln -s fping fping6

# NetBSD puts lzo into /usr/pkg/{include,lib} where our configure
# will not find it -> make symlinks
echo "creating lzo2 convenience symlinks..."
cd /usr/include && ln -s ../pkg/include/lzo .
cd /usr/lib && ln -s ../pkg/lib/liblzo2.* .

# same thing for mbedtls...
echo "creating mbedTLS convenience symlinks..."
cd /usr/include && ln -s ../pkg/include/mbedtls .
cd /usr/lib && ln -s ../pkg/lib/libmbed* .

exit 0
