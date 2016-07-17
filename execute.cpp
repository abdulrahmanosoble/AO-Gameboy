#include "mem.h"

void setCarry(){
	F |= 0X10;
}

void clearCarry(){
	F &= 0xEF;
}

void setHalfCarry(){
	F |= 0X20;
}

void clearHalfCarry(){
	F &= 0xDF;
}

void setOperation(){
	F |= 0X40;
}

void clearOperation(){
	F &= 0xBF;
}

void setZero(){
	F |= 0X80;
}

void clearZero(){
	F &= 0x7F;
}

void setFlags(int result, bool eightbit, bool addition){
	if(result == 0)
		F |= 0x80;
	if(!addition)
		F |= 0X40;
	if(addition)
	{
		if(eightbit)
		{
			if(result > 255)
				F |= 0X10;
		}
		else
		{
			if(result > 65535)
				F |= 0X10;
		}
	}
	else
	{
		if(result < 0)
			F |= 0X10;
	}
}

//NOP

void NOP(){
	F = 0;
	return;
}

// LD instructions

void LDregn(unsigned char *reg){
	PC++;
	*reg = mem[PC];
}

void LDregnn(unsigned short *reg){
	unsigned short operand = 0;
	PC++;
	operand = mem[PC] << 8;
	PC++;
	operand &= mem[PC];
	*reg = operand;
}

void LDregAddress(unsigned char *reg1, unsigned short *reg2){
	mem[*reg2] = *reg1;
}

void LDAddressreg(unsigned char *reg1, unsigned short *reg2){
	*reg1 = mem[*reg2];
}

void LDnnreg16(unsigned short *reg){
	unsigned short operand = 0;
	PC++;
	operand = mem[PC] << 8;
	PC++;
	operand &= mem[PC];
	mem[operand] = SP>>8;
	mem[operand+1] = SP&0X0F;
}

void LDregreg8bit(unsigned char *reg1, unsigned char *reg2){
	*reg1 = *reg2;
}

void LDregreg16bit(unsigned short *reg1, unsigned short *reg2){
	*reg1 = *reg2;
}

// INC/DEC instructions

void INC(unsigned char *reg){
	*reg = (*reg)++;
}

void DEC(unsigned char *reg){
	*reg = (*reg)--;
}

void INC(unsigned short *reg){
	*reg = (*reg)++;
}

void DEC(unsigned short *reg){
	*reg = (*reg)--;
}

//ROTATE

void RLC16(unsigned short *reg){
	unsigned char carry = 0;
	carry = *reg >> 15;
	*reg = (*reg) << 1;
	if(carry)
		setCarry();
	else
		clearCarry();
	clearHalfCarry();
	clearOperation();
	if((*reg) == 0)
		setZero();
	else
		clearZero();
}

void RL16(unsigned short *reg){
	unsigned char carry = 0;
	carry = *reg >> 15;
	*reg = ((*reg) << 1)&carry;
	if(carry)
		setCarry();
	else
		clearCarry();
	clearHalfCarry();
	clearOperation();
	if((*reg) == 0)
		setZero();
	else
		clearZero();
}

void RLC8(unsigned char *reg){
	unsigned char carry = 0;
	carry = *reg >> 7;
	*reg = (*reg) << 1;
	if(carry)
		setCarry();
	else
		clearCarry();
	clearHalfCarry();
	clearOperation();
	if((*reg) == 0)
		setZero();
	else
		clearZero();
}

void RL8(unsigned char *reg){
	unsigned char carry = 0;
	carry = *reg >> 7;
	*reg = ((*reg) << 1)&carry;
	if(carry)
		setCarry();
	else
		clearCarry();
	clearHalfCarry();
	clearOperation();
	if((*reg) == 0)
		setZero();
	else
		clearZero();
}







void execute(){

}

void executeExtend(){
	switch(mem[PC])
	{

	}
}