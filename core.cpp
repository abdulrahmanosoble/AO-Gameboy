#include<iostream>
#include<iomanip>
#include<fstream>
#include<chrono>
#include<thread>

using namespace std;

#include "execute.h"
#include "mem.h"
#include "gpu.h"

RegAF regAF;
RegBC regBC;
RegDE regDE;
RegHL regHL;

unsigned short PC = 0;
unsigned short SP = 0;

unsigned char data[10000];
unsigned char mem[65536];
short cycles = 0;

void handleInterrupts(){
	short fired = mem[0xFFFF] & mem[0XFF0F];
	if (fired & 0x01){
		RST(0x40);
	}
	if (fired & 0x02){
		RST(0x48);
	}
	if (fired & 0x04){
		RST(0x50);
	}
	if (fired & 0x08){
		RST(0x58);
	}
	if (fired & 0x10){
		RST(0x60);
	}
}

int main(){

	regAF.AF = 0;
	regBC.BC = 0;
	regDE.DE = 0;
	regHL.HL = 0;

/*	for (int i = 0; i < 65536; i++)
	{
		mem[i] = 0;
	}*/

	ifstream game("tetris.gb", ios::in | ios::binary);
	//ifstream bios("bios.bin", ios::in | ios::binary);
	ofstream debug("debug.txt");

	unsigned char* gameData;
	unsigned char* biosData;
	char* gameTemp;
	char* biosTemp;
	int gameLength;
	int biosLength;

	game.seekg(0, ios::end);
	gameLength = game.tellg();
	gameTemp = new char[gameLength];
	game.seekg(0, ios::beg);
	game.read(gameTemp, gameLength);
/*	bios.seekg(0, ios::end);
	biosLength = bios.tellg();
	biosTemp = new char[biosLength];
	bios.seekg(0, ios::beg);
	bios.read(biosTemp, biosLength);*/

	gameData = new unsigned char[gameLength];
	for (int i = 0; i<gameLength; i++)
		gameData[i] = gameTemp[i];
/*	biosData = new unsigned char[biosLength];
	for (int i = 0; i<biosLength; i++)
		biosData[i] = biosTemp[i];*/

	/*	for(int i=0;i<0xFF;i++){
	mem[i] = biosData[i];
	}

	for(int i=0XFF;i<(gameLength-0xFF);i++)
	{
	mem[i] = gameData[i];
	}
	*/
	for (int i = 0; i<gameLength; i++)
	{
		mem[i] = gameData[i];
	}

	std::thread gpu(GPU);

	int i = 0;
	while (i<1000)
	{
		if (true)
		{
			cout << endl << hex << "Next Operand; " << (short)mem[PC] << "(" << (short)mem[PC + 1] << " " << (short)mem[PC + 2] << ")" << endl;
			if (mem[PC] == 0XCB)
				cout << hex << (short)mem[PC + 1];
			execute(mem[PC++]);
			//getchar();
			cout << endl;
			cout << "AF: " << regAF.AF << endl;
			cout << "BC: " << regBC.BC << endl;
			cout << "DE: " << regDE.DE << endl;
			cout << "HL: " << regHL.HL << endl;
			cout << "SP: " << SP << endl;
			cout << "PC: " << PC << endl;
			handleInterrupts();
		}
		else
		{
			cycles--;
			if (mem[0xFF07] & 0X04)
			{
				if (mem[0xFF05]++ == 0XFF)
				mem[0xFF05] = mem[0xFF06];
			}
			switch (mem[0xFF07] & 0x03)
			{
			case 0:
				this_thread::sleep_for(chrono::nanoseconds(244141));
				break;
			case 1:
				this_thread::sleep_for(chrono::nanoseconds(3815));
				break;
			case 2:
				this_thread::sleep_for(chrono::nanoseconds(15259));
				break;
			case 3:
				this_thread::sleep_for(chrono::nanoseconds(61035));
				break;
			}
		}
	}
}
