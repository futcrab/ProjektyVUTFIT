#preklad zdrojoveho souboru
mpic++ --prefix /usr/local/share/OpenMPI -o life life.cpp 

#spusteni programu
mpirun --prefix /usr/local/share/OpenMPI -np 4 life $1 $2

#uklid
rm -f life