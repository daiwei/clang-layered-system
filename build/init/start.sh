#!/bin/bash

# if [ ! -e /usr/lib/libframework.so ] ; then
#     ln -s /gateway/framework/lib/libframework.so /usr/lib/libframework.so
# fi
#
# if [ ! -e /usr/lib/libhardware.so ] ; then
#     ln -s /gateway/hardware/lib/libhardware.so /usr/lib/libhardware.so
# fi
echo "gateway start..."

cd `dirname $0`

if [ ! -e /usr/lib/gateway ] ; then
    ln -s /gateway /usr/lib/gateway
fi

gpsd /dev/ttymxc3 -F /var/run/gpsd.sock
