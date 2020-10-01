#!/bin/sh

echo "Preparing OpenVPN build environment"
pkg update
pkg install --yes sudo automake autoconf fping git lzo2 liblz4 \
		libtool mbedtls cmake

# make sure tap driver is loaded (tun automatically is)
kldload if_tap
if grep if_tap /boot/loader.conf
then
    echo "loader.conf already has if_tap"
else
    echo 'if_tap_load="YES"' >>/boot/loader.conf
fi

# FreeBSD puts lzo into /usr/local/{include,lib} where our configure 
# will not find it -> make symlinks
echo "creating lzo2 convenience symlinks..."
cd /usr/include && ln -s ../local/include/lzo .
cd /usr/lib && ln -s ../local/lib/liblzo2.* .

# same thing for mbedtls...
echo "creating mbedTLS convenience symlinks..."
cd /usr/include && ln -s ../local/include/mbedtls .
cd /usr/lib && ln -s ../local/lib/libmbed* .


exit 0
