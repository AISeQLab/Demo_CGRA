#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <bits/stdc++.h>
#include <vector>
#include <cstring> // For strlen
#include <iomanip> // For hex, setw, and setfill
using namespace std;

uint64_t rot(uint64_t x, int y){
	uint64_t z =(((x) << (y)) | ((x) >> ((64) - (y))));
    return z;
}

int r(int x,int y){
    int offsets[5][5];
    offsets[0][0] = 0;
    offsets[0][1] = 36;
    offsets[0][2] = 3;
    offsets[0][3] = 105;
    offsets[0][4] = 210;
    offsets[1][0] = 1;
    offsets[1][1] = 300;
    offsets[1][2] = 10;
    offsets[1][3] = 45;
    offsets[1][4] = 66;
    offsets[2][0] = 190;
    offsets[2][1] = 6;
    offsets[2][2] = 171;
    offsets[2][3] = 15;
    offsets[2][4] = 253;
    offsets[3][0] = 28;
    offsets[3][1] = 55;
    offsets[3][2] = 153;
    offsets[3][3] = 21;
    offsets[3][4] = 120;
    offsets[4][0] = 91;
    offsets[4][1] = 276;
    offsets[4][2] = 231;
    offsets[4][3] = 136;
    offsets[4][4] = 78;
    return offsets[x][y];
}


static const uint64_t RC[24] = {0x0000000000000001, 0x0000000000008082, 
0x800000000000808A, 0x8000000080008000,0x000000000000808B,
0x0000000080000001,0x8000000080008081,

0x8000000000008009,0x000000000000008A,0x0000000000000088,0x0000000080008009,
0x000000008000000A,0x000000008000808B,0x800000000000008B,0x8000000000008089,
0x8000000000008003,0x8000000000008002,0x8000000000000080,0x000000000000800A,
0x800000008000000A,0x8000000080008081,0x8000000000008080,0x0000000080000001,
0x8000000080008008};

void show(uint64_t A[5][5]){
    uint64_t t=0x123456789abcdef;
    for (int x =0; x<5; x++){

        for (int y = 0; y<5; y++){
            cout<<"A["<<y<<"]["<<x<<"]: "<< hex <<setfill('0')<<setw(16)<<A[y][x]<<endl;
           // cout<< hex <<setfill('0')<<setw(16)<<A[y][x];
        }
    }
}

void show1d(uint64_t C[5]){
    for (int i =4; i>=0; i--){
    cout<< hex <<setfill('0')<<setw(16)<<C[i];}
    //cout<<endl;

}


void round(uint64_t A[5][5], uint64_t RC) {
    //cout<<"Vao round\n";
  //# ? step
  uint64_t C[5];
  for (int x = 0; x <5; x++){
        C[x] = A[x][0] ^ A[x][1] ^ A[x][2] ^ A[x][3] ^ A[x][4];    
        
  }
      //cout<<"Show C[x]:\n";
  //show1d(C);
  uint64_t D[5];
  for (int x = 0; x <5; x++){
        D[x] = C[(x+4)%5] ^ rot(C[(x+1)%5],1);                          
  }
      //cout<<"Show D[x]:\n";
  //show1d(D);
  
  uint64_t t = rot(C[4],1);

  for (int x =0; x<5; x++){
    for (int y =0; y<5; y++){
        A[x][y] = A[x][y] ^ D[x];
    }
  }
  //cout<<"Done theta: ";
  //show(A);
  //cout<<endl;
  //# ? & p steps
  uint64_t B[5][5];
  for (int x =0; x<5; x++){
    for (int y =0; y<5; y++){
        B[y][(2*x+3*y)%5] = rot(A[x][y], r(x,y));
    }
  }
  //cout<<"Done rho va pi: ";
  //show(B);
  //cout<<endl;
  //# ? step
  for (int x =0; x<5; x++){
    for (int y =0; y<5; y++){
        A[x][y] = B[x][y] ^ ((~(B[(x+1)%5][y])) & B[(x+2)%5][y]); 
    }
  }
  //cout<<"Done chi: ";
  //show(A);
  //cout<<endl;
 // # ? step
  A[0][0] = A[0][0] ^ RC;
  //cout<<"Done iota: ";
  //show(A);
  //cout<<endl;
}

void f_function(uint64_t A[5][5]){
    //cout<<"Vao ffunction\n";
    for (int i = 0; i <24; i++){
        //cout<<"ROUND "<<dec<<i<<": \n";
        round(A, RC[i]);
    }
}

void Padding(uint8_t input[], int &in_len, int &absorb_times){
    absorb_times = in_len / 72;
    int add_last = in_len % 72;
    if (72 - add_last == 1) input[in_len] = 0x86;
    else{
    input[in_len] = 0x06;
    for (int i = 1; i < (71 - add_last); i++){         
        input[in_len+i] = 0x00;      
        //printf("%02x", input[in_len+i]);
    }   
    input[in_len+(71 - add_last)] = 0x80; 
    in_len = in_len + (72 - add_last);
    }

    absorb_times += 1;
}


void assign_S_xor_p(uint64_t S[5][5], uint64_t *p){
    for (int x =0; x<5; x++){
        for (int y = 0; y<5; y++){
            if ((x+5*y)<9) {
				printf("S[%d][%d] = %016lx ^ p[%d+5*%d] = %016lx\n",x,y,S[x][y],x,y, p[x+5*y]);
                S[x][y] = S[x][y] ^ p[x+5*y];
            }
        }
    }
}

void EndianSwap (uint64_t &x)
{
	x = (x >> 56) |
		((x << 40) & 0x00FF000000000000) |
		((x << 24) & 0x0000FF0000000000) |
		((x << 8)  & 0x000000FF00000000) |
		((x >> 8)  & 0x00000000FF000000) |
		((x >> 24) & 0x0000000000FF0000) |
		((x >> 40) & 0x000000000000FF00) |
		(x << 56);
}

void dm(uint64_t S[5][5]) {
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 5; ++j) {
            if (i == 1 && j > 2) {
                break;
            }
			else{
				cout << hex << setfill('0') << setw(16) << S[j][i];
			}
        }
    }
}

int main(){
    int max_input_length = 4096; // nên để giá trị gì đó cho cái size của mảng input, nếu để rổng dễ gây core dumped T.T
    int block_size = 72;
    uint8_t input[max_input_length + block_size] = {0};
    uint8_t *p_input = input;

    // cout << "Type data: ";
    // cin.getline((char*)input, max_input_length);
    // int len = strlen((char*)input);

    // cout << "Length of data: " << len << endl;
    int absorb_times = 1;
    // Padding(input, len, absorb_times); 
	
	for(int i = 0; i < 72; i++){
		input[i] = i;
		printf("%02x",input[i]);
	}
	printf("\n");
    uint64_t S[5][5] = {0};
	
	for (int i = 0; i < 5; i++){
        for (int j = 0; j < 5; j++){
            S[i][j] = i*5+j+1;
			printf("S[%d][%d] = %016lx\n",i,j,S[i][j]);
        }
    }
	
    for (int i = 0; i < absorb_times; i++ ){
        cout << "Message block " << i + 1 << " of " << absorb_times << endl;
        assign_S_xor_p(S, (uint64_t *)p_input);
        p_input += block_size;
        f_function(S);
    }

    for (int i = 0; i < 5; i++){
        for (int j = 0; j < 5; j++){
            EndianSwap(S[i][j]);
        }
    }

    cout << "Hash value of SHA3-512: ";
    dm(S);
    cout << endl;
    return 0;
}