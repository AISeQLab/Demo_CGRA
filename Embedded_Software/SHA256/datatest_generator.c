
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BLOCK_SIZE 64 // 512 bits or 64 bytes
#define NUM_MESSAGES 8000 // Number of messages to generate
#define MAX_MESSAGE_LENGTH 55 // Maximum message length in bytes

// Helper function to write a 64-bit integer to the end of the block
void write_message_length(unsigned char *block, size_t length) {
    // The length should be in bits, so multiply by 8
    unsigned long long bit_length = length * 8;
    // Write the length into the last 8 bytes of the block big endian style
    for (int i = 0; i < 8; i++) {
        block[BLOCK_SIZE - 1 - i] = (unsigned char)(bit_length >> (i * 8));
    }
}

void print_padded_message(FILE *file, const char *message, size_t message_len) {
    unsigned char block[BLOCK_SIZE] = {0}; // Initialize block with zeros

    // Copy message to the block
    memcpy(block, message, message_len);

    // Pad the first bit after the message with 0x80 (10000000 in binary)
    block[message_len] = 0x80;

    // Write the message length in bits at the end of the block
    write_message_length(block, message_len);

    // Print the padded block in hexadecimal to the file
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        fprintf(file, "%02x", block[i]);
    }
    fprintf(file, "\n");
}

int main() {
    FILE *file = fopen("datatest_input.txt", "w");
    if (file == NULL) {
        perror("Could not open file to write");
        return EXIT_FAILURE;
    }

    // Seed the random number generator
    srand((unsigned int)time(NULL));

    // Generate and write the padded messages
    for (int i = 0; i < NUM_MESSAGES; i++) {
        size_t message_len = rand() % (MAX_MESSAGE_LENGTH + 1); // Random length up to MAX_MESSAGE_LENGTH
        char message[MAX_MESSAGE_LENGTH + 1]; // +1 for null terminator

        // Generate a random message of random length
        for (size_t j = 0; j < message_len; j++) {
            message[j] = (char)(rand() % (126 - 33 + 1)) + 33; // Printable characters range
        }
        message[message_len] = '\0'; // Null-terminate the string

        print_padded_message(file, message, message_len);
    }

    fclose(file);
    return 0;
}