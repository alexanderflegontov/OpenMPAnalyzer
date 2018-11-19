#!/bin/sh
echo "Netbeans INSTALL SCRIPT"

NETBEANS_INSTALLER=jdk-8u161-nb-8_2-linux-x64.sh
CURRENT_DIR=$PWD


if [ ! -f "$NETBEANS_INSTALLER" ]; then
  echo $NETBEANS_INSTALLER  " - is not found! Let's try to find it in net!"
  wget https://download.netbeans.org/netbeans/8.2/final/bundles/netbeans-8.2-linux.sh
  NETBEANS_INSTALLER=netbeans-8.2-linux.sh
  #exit 1
fi


if [ ! -f "$NETBEANS_INSTALLER" ]; then
  echo $NETBEANS_INSTALLER  " - is not found anywhere!"
  #exit 1
else
  echo "Install " $NETBEANS_INSTALLER " .. "
  $CURRENT_DIR/$NETBEANS_INSTALLER
  cd $CURRENT_DIR

  echo "Copy " NetBeansProjects ".. "
  mkdir ~/NetBeansProjects
  cp -r NetBeansProjects ~/     #NetBeansProjects/  #JavaFXApplication2
fi

echo "Netbeans INSTALL SCRIPT END!"

