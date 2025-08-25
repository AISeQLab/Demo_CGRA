gcc AES_Encryption_FPGA.c -o main -I. -DVIVADOSIL -DMODE32
./main 
rm -f main
