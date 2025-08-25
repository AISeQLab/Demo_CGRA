gcc CHACHA20_FPGA.c -o main -I. -DVIVADOSIL -DMODE32
./main 
rm -f main
