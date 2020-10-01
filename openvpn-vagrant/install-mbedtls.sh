#!/bin/sh

VERSION="2.6.0"
BASENAME="mbedtls-$VERSION"
TARBALL="$BASENAME-gpl.tgz"

curl -O https://tls.mbed.org/download/$TARBALL
tar -zxf $TARBALL
cd $BASENAME
make
make install
