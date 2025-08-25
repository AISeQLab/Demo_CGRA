gcc BLAKE256.c -o main -I. -DCHECKRULE -DMODE32
./main 
rm -f main
