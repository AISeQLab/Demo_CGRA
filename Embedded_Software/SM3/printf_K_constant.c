#include <stdio.h>

#define ROTL(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

int main() {
    unsigned int T[64];
    unsigned int K[64];
    int j;

    // Initialize T
    for (j = 0; j < 16; j++) {
        T[j] = 0x79CC4519;
    }
    for (j = 16; j < 64; j++) {
        T[j] = 0x7A879D8A;
    }

    // Calculate K
    for (j = 0; j < 64; j++) {
        // You would need to use a different expression for K[j] when j is between 0 and 15
        // if there's supposed to be a different calculation or initialization there.
        K[j] = ROTL(T[j], j);
    }

    // Print K
    for (j = 0; j < 64; j++) {
        printf("K[%d] = %08X\n", j, K[j]);
    }

    return 0;
}
