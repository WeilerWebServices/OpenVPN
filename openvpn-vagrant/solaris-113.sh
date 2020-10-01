#!/bin/sh

#!/bin/sh

echo "Preparing OpenVPN build environment"

# make sure tap driver is loaded (tun automatically is)
# (3rd party package)
echo "Checking for tun/tap driver..."

if [ -f /usr/kernel/drv/amd64/tun -a -f /usr/kernel/drv/amd64/tap ]
then
    echo " ... tun/tap driver already there, good"
else
    echo " ... fetching/building/installing tun/tap driver..."
    cd /tmp
    wget -O kaizawa-tuntap.tar.gz https://github.com/kaizawa/tuntap/tarball/master
    tar xvfz kaizawa-tuntap.tar.gz
    if [ -d kaizawa-tuntap-* ] ; then
        cd kaizawa-tuntap-*
        ./configure && make && sudo make install
    else
        echo "ERROR: tun driver retrieval/extraction failed?"
        ls -lrt
        exit 1
    fi
fi


# CSW package puts lzo into /opt/csw/{include,lib} where our configure 
# will not find it -> make symlinks
echo "creating lzo2 convenience symlinks..."
cd /usr/include && ln -s /opt/csw/include/lzo .
cd /usr/lib && ln -s /opt/csw/lib/liblzo2.* .

# we have no mbedtls yet, so if you want, add mbedtls fetch/build
# instructions here...
#
# same thing for mbedtls...
#echo "creating mbedTLS convenience symlinks..."
#cd /usr/include && ln -s ../local/include/mbedtls .
#cd /usr/lib && ln -s ../local/lib/libmbed* .

exit 0
