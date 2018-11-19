#!/bin/sh
echo "PIN ENVIRONMENT INSTALL SCRIPT"
#cd $INSTALL_PATH/omp_prog
#MYPROG=$PWD
#PATH=$PATH:$MYPROG

#cd $INSTALL_PATH/pin-3.5-97503-gac534ca30-gcc-linux/source/tools/ManualExamples
#MYPIN=$PWD
#PATH=$PATH:$MYPIN



CURRENT_DIR=$PWD
INSTALL_PATH=~/Documents/

GXX=g++
PINTOOL_TRIGGER=pintool_trigger
OBJ_INTEL64=obj-intel64
MYPINTOOL=myPinTool

PINTOOL=pin-3.5-97503-gac534ca30-gcc-linux
PINTOOL_SRC_DIR=src
BENCHMARKS_DIR=benchmarks
OMP_PROGRAMS_DIR=omp_prog
NETBEANS_INSTALLER=jdk-8u161-nb-8_2-linux-x64.sh

echo "Install g++"
sudo apt-get install GXX


if [ ! -f "$PINTOOL.tar.gz" ]; then
  echo $PINTOOL.tar.gz  " - is not found!"
  exit 1
fi

if [ ! -f "$PINTOOL_SRC_DIR/pintool_trigger.cpp" ]; then
  echo $PINTOOL_SRC_DIR/$PINTOOL_TRIGGER.cpp  " - is not found!"
  exit 1
fi

if [ ! -d "$OMP_PROGRAMS_DIR" ]; then
  echo $OMP_PROGRAMS_DIR  " - is not found!"
  exit 1
fi

if [ ! -d "$BENCHMARKS_DIR" ]; then
  echo $BENCHMARKS_DIR  " - is not found!"
  exit 1
fi

echo "ALL checks are Ok start install.."
exit 1 # Comment this!

# It need to check! maybe it need to do in seporate terminal before
./setpinenv.sh
bash

echo "Copy " $PINTOOL.tar.gz ".. "
cp    $PINTOOL.tar.gz                           $INSTALL_PATH
echo "Copy " $PINTOOL_TRIGGER.cpp ".. "
cp    $PINTOOL_SRC_DIR/$PINTOOL_TRIGGER.cpp     $INSTALL_PATH
echo "Copy " $OMP_PROGRAMS_DIR ".. "
cp -r $OMP_PROGRAMS_DIR                         $INSTALL_PATH/$OMP_PROGRAMS_DIR
echo "Copy " $BENCHMARKS_DIR ".. "
cp -r $BENCHMARKS_DIR                           $INSTALL_PATH

echo "Install " $PINTOOL_TRIGGER ".. "
cd $INSTALL_PATH
$GXX $PINTOOL_TRIGGER.cpp -o $PINTOOL_TRIGGER.exe
tar -xf $PINTOOL.tar.gz


cd $INSTALL_PATH/$BENCHMARKS_DIR
echo "Install " openssl package: libssl-dev ".. "
sudo apt-get install libssl-dev
echo "Compile " benchmarks ".. "
$GXX benchmark1.cpp -o benchmark1 -lcrypto -fopenmp                              ^
$GXX benchmark2.cpp -o benchmark2 -lcrypto -fopenmp 
$GXX gethash.cpp    -o gethash    -lcrypto 


cd $INSTALL_PATH/$OMP_PROGRAMS_DIR
echo "Compile " omp_prog ".. "
$GXX Simpson_OMP.cpp -o Simpson -fopenmp
$GXX Cannon.cpp      -o Cannon -fopenmp
$GXX Bicgm.cpp -o Bicgm -fopenmp
cd optimal_split_graph
$GXX *.cpp -o Graph -fopenmp 
cd ..


cd $CURRENT_DIR
cp $PINTOOL_SRC_DIR/$MYPINTOOL.cpp   $MYPIN
cp $PINTOOL_SRC_DIR/MyPinToolDefs.h  $MYPIN


echo "Make " $MYPINTOOL.so " .. "
cd $MYPIN
mkdir $OBJ_INTEL64
make $OBJ_INTEL64/$MYPINTOOL.so

if [ ! -d "INFO" ]; then
  echo INFO  " - is not found!"
  mkdir INFO
  echo INFO  " - is added!"
fi


if [ ! -f "$NETBEANS_INSTALLER" ]; then
  echo $NETBEANS_INSTALLER  " - is not found!"
  #exit 1
else
  if [ -f "netbeans_installer.sh" ]; then
    ./netbeans_installer.sh
  else
    echo "Install " $NETBEANS_INSTALLER " .. "
    $CURRENT_DIR/$NETBEANS_INSTALLER
    cd $CURRENT_DIR

    echo "Copy " NetBeansProjects ".. "
    mkdir ~/NetBeansProjects
    cp -r NetBeansProjects ~/     #NetBeansProjects/  #JavaFXApplication2
fi

echo "PIN ENVIRONMENT IS INSTALLED !"

