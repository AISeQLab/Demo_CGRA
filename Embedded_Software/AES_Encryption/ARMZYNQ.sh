gcc AES_Encryption_FPGA.c -o main -I. -DARMZYNQ -DMODE32
./main 
rm -f main
