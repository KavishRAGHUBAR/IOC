#!/bin/sh
module=$1
/sbin/rmmod $module || exit 1
rm -f /dev/$module
echo "=> Device /dev/$module removed"


module1=$2
/sbin/rmmod $module1 || exit 1
rm -f /dev/$module1
echo "=> Device /dev/$module1 removed"


module2=$3
/sbin/rmmod $module2 || exit 1
rm -f /dev/$module2
echo "=> Device /dev/$module2 removed"