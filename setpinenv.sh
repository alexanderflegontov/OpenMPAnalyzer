echo "Set .bashrc"
cd ~
cat << EOF >> .bashrc

MYPIN='/home/hp/Documents/pin-3.5-97503-gac534ca30-gcc-linux/source/tools/ManualExamples'
export MYPIN
export PATH=\$PATH:\$MYPIN

MYPROG='/home/hp/Documents/omp_prog'
export MYPROG
export PATH=\$PATH:\$MYPROG
#export PATH=\$PATH:"/home/hp/Cmake/cmake-3.12.4-Linux-x86_64/bin"

MYBM='/home/hp/Documents/benchmarks'
export MYBM
export PATH=\$PATH:\$MYBM

EOF
echo "Setting .bashrc end"
