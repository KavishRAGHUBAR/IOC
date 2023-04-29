#!/bin/sh
module=$1
shift
/sbin/insmod ./$module.ko $* || exit 1
rm -f /dev/$module
major=$(awk "\$2==\"$module\" {print \$1;exit}" /proc/devices)
mknod /dev/$module c $major 0
chmod 666 /dev/$module
echo "=> Device /dev/$module created with major=$major"

module1=$2
shift
/sbin/insmod ./$module1.ko $* || exit 1
rm -f /dev/$module1
major1=$(awk "\$2==\"$module1\" {print \$1;exit}" /proc/devices)
mknod /dev/$module1 c $major1 0
chmod 666 /dev/$module1
echo "=> Device /dev/$module1 created with major=$major1"

module2=$3
shift
/sbin/insmod ./$module2.ko $* || exit 1
rm -f /dev/$module2
major2=$(awk "\$2==\"$module2\" {print \$1;exit}" /proc/devices)
mknod /dev/$module2 c $major2 0
chmod 666 /dev/$module2
echo "=> Device /dev/$module2 created with major=$major2"