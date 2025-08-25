#include <fcntl.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

uint64_t getopt_integer(char *optarg)
{
	int rc;
	uint64_t value;

	rc = sscanf(optarg, "0x%lx", &value);
	if (rc <= 0)
		rc = sscanf(optarg, "%lu", &value);
	//printf("sscanf() = %d, value = 0x%lx\n", rc, value);

	return value;
}

static struct option const long_opts[] = {
	{"Number of datatests", required_argument, NULL, 'N'},
	{"Message length", required_argument, NULL, 'L'},
	{0, 0, 0, 0}
};

int main(int argc, char *argv[])
{
	int cmd_opt;
	int N, L , L_byte;
	
	if( argc > 5 | argc < 5) {
		printf("//***********************************************************************************//\n");
		printf("//* Command Line Arguments should follow:                                           *//\n");
		printf("//* ./main -N <Number of datatests> -L <Message length:512 bits, 1024 bits, etc>   *　//\n");
		printf("//* For example:    ./main -N 100 -L 512                                    　       *//\n");
		printf("//***********************************************************************************//\n");
		return -1;
    }
	else {
		while ((cmd_opt =
			getopt_long(argc, argv, "N:L:", long_opts,
					NULL)) != -1) {
			switch (cmd_opt) {
			case 0:
				printf("//***********************************************************************************//\n");
				printf("//* Command Line Arguments should follow:                                           *//\n");
				printf("//* ./main -N <Number of datatests> -L <Message length:512 bits, 1024 bits, etc> 　  *//\n");
				printf("//* For example:    ./main -N 100 -L 512        　                                   *//\n");
				printf("//***********************************************************************************//\n");
				break;
			case 'N':
				/* device node name */
				//fprintf(stdout, "'%s'\n", optarg);
				N = getopt_integer(optarg);
				break;
			case 'L':
				/* RAM address on the AXI bus in bytes */
				L = getopt_integer(optarg);
				break;
			default:
				printf("//***********************************************************************************//\n");
				printf("//* Command Line Arguments should follow:                                           *//\n");
				printf("//* ./main -N <Number of datatests> -L <Message length:512 bits, 1024 bits, etc>  　 *//\n");
				printf("//* For example:    ./main -N 100 -L 512   　                                        *//\n");
				printf("//***********************************************************************************//\n");
				break;
			}
		}
	}
	

   
	// the name of datatest file
    char *filename = "datatest.txt";
	
    // open the file for writing
    FILE *fp = fopen(filename, "w");
	
    if (fp == NULL){
        printf("Error opening the file %s", filename);
        return -1;
    }
	
	// list of random characters
	static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!";
	
	// Intializes random number generator
    srand(time(NULL)); 
   
    L_byte = L/8;
	char *randomString = malloc(L_byte);
    int length;
	
    // write to the text file
    for (int i = 0; i < N; i++){
		length = (rand() % ((L_byte-9) - (L_byte-50)) +1) + (L_byte-50);
		if (length) {
			if (randomString) {
				int l = (int) (sizeof(charset) -1);
				for (int n = 0;n < length;n++) {
					int key = rand() % l;    
					randomString[n] = charset[key];
				}
				randomString[length] = '\0';
			}
		}
        fprintf(fp, "%s\n", randomString);
	}
	
    // close the file
    fclose(fp);
	printf("Completed generating %d random %d-bit data vectors !\n",N,L);
    return 0;
}