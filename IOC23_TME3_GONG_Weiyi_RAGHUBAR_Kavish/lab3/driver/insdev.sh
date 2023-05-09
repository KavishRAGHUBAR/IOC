#!/bin/sh
module=$1
shift
/sbin/insmod ./$module.ko $* || exit 1
rm -f /dev/$module
major=$(awk "\$2==\"$module\" {print \$1;exit}" /proc/devices)
mknod /dev/$module c $major 0
chmod 666 /dev/$module
echo "=> Device /dev/$module created with major=$major"
