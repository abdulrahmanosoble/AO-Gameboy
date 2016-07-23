#include<iostream>
#include<iomanip>
#include<fstream>

using namespace std;

#include "execute.h"
#include "mem.h"

RegAF regAF;
RegBC regBC;
RegDE regDE;
RegHL regHL;

unsigned short PC = 0;
unsigned short SP = 0;

unsigned char data[10000];
unsigned char mem[65536];

int main(){
	
	regAF.AF = 0;
	regBC.BC = 0;
	regDE.DE = 0;
	regHL.HL = 0;

	for(int i=0;i < 65536;i++)
	{
		mem[i] = 0;
	}

	ifstream game("tetris2.gb", ios::in|ios::binary);
	ifstream bios("bios.bin", ios::in|ios::binary);
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
	game.read(gameTemp,gameLength);
	bios.seekg(0, ios::end);
	biosLength = bios.tellg();
	biosTemp = new char[biosLength];
	bios.seekg(0, ios::beg);
	bios.read(biosTemp,biosLength);

	gameData = new unsigned char[gameLength];
	for(int i=0;i<gameLength;i++)
		gameData[i] = gameTemp[i];
	biosData = new unsigned char[biosLength];
	for(int i=0;i<biosLength;i++)
		biosData[i] = biosTemp[i];

	for(int i=0;i<0xFF;i++){
		mem[i] = biosData[i];
	}

	for(int i=0XFF;i<(gameLength-0xFF);i++)
	{
		mem[i] = gameData[i];
	}

	while(true)
	{	
		debug << hex << (short)mem[PC];
		if(mem[PC] == 0XCB)
			debug << hex << (short)mem[PC+1] << " ";
		else
			debug << " ";
		execute(mem[PC++]);
	}
}