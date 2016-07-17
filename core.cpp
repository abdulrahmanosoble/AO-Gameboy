#include<iostream>
#include<fstream>

#include "execute.h"
using namespace std;

int main(){
	// BOOT
	PC = 0;

	ifstream game("tetris.gb");
	unsigned char c;
	for(int i=0;game >> c;i++)
	{
		data[i] = c;
	}
	cout << hex << (int)data[10] << endl;
	cout<<"---------------------"<<endl;
	A = 0X04;
	F = 0X1d;
}