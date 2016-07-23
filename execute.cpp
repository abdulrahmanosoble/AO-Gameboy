#include<iostream>

#include "execute.h"
#include "mem.h"

using namespace std;

extern RegAF regAF;
extern RegBC regBC;
extern RegDE regDE;
extern RegHL regHL;
extern unsigned short SP;
extern unsigned short PC;

void setCarry(){
	regAF.F |= 0X10;
}

void clearCarry(){
	regAF.F &= 0xEF;
}

void setHalfCarry(){
	regAF.F |= 0X20;
}

void clearHalfCarry(){
	regAF.F &= 0xDF;
}

void setOperation(){
	regAF.F |= 0X40;
}

void clearOperation(){
	regAF.F &= 0xBF;
}

void setZero(){
	regAF.F |= 0X80;
}

void clearZero(){
	regAF.F &= 0x7F;
}

void setFlags(int result, bool eightbit, bool addition){
	if(result == 0)
		regAF.F |= 0x80;
	if(!addition)
		regAF.F |= 0X40;
	if(addition)
	{
		if(eightbit)
		{
			if(result > 255)
				regAF.F |= 0X10;
		}
		else
		{
			if(result > 65535)
				regAF.F |= 0X10;
		}
	}
	else
	{
		if(result < 0)
			regAF.F |= 0X10;
	}
}

//NOP

void NOP(){
	return;
}

// LD instructions

void LDregn(unsigned char *reg){
	*reg = mem[PC++];
}

void LDregnn(unsigned short *reg){
	unsigned short operand = 0;
	operand = mem[PC++];
	operand &= mem[PC++]<<8;
	*reg = operand;
}

void LDnnreg(unsigned char *reg){
	unsigned short operand = 0;
	operand = mem[PC++];
	operand &= mem[PC++]<<8;
	mem[operand] = *reg;
}

void LDregAddress(unsigned char *reg1, unsigned short *reg2){
	mem[*reg2] = *reg1;
}

void LDregAddress(unsigned short *reg1, unsigned char *reg2){
	mem[*reg2] = *reg1;
}

void LDAddressreg(unsigned char *reg1, unsigned short *reg2){
	*reg1 = mem[*reg2];
}

void LDnnreg16(unsigned short *reg){
	unsigned short operand = 0;
	operand = mem[PC++];
	operand &= mem[PC++] << 8;
	mem[operand] = SP&0x0F;
	mem[operand+1] = SP>>8;
}

void LDregreg8bit(unsigned char *reg1, unsigned char *reg2){
	*reg1 = *reg2;
}

void LDregreg16bit(unsigned short *reg1, unsigned short *reg2){
	*reg1 = *reg2;
}

void LDDHLA(){
	mem[regHL.HL--] = regAF.A;
}

void LDDAHL(){
	regAF.A = mem[regHL.HL--];
}

void LDHCA(){
	mem[0xFF00+regBC.C] = regAF.A;
}

void LDHnA(){
	mem[0xFF00+mem[PC++]] = regAF.A;
}

void LDHAn(){
	regAF.A = mem[0xFF00+mem[PC++]];
}

void LDIAHL(){
	regAF.A = mem[regHL.HL++];
}

void LDIHLA(){
	mem[regHL.HL++] = regAF.A;
}

void LDHLSP(){
	regHL.HL = SP + mem[PC++];
}

void LDAnn(){
	unsigned short operand = 0;
	operand = mem[PC++];
	operand &= mem[PC++]<<8;
	regAF.A = mem[operand];
}

void PUSH(unsigned short *reg){
	mem[SP-1] = (*reg)>>8;
	mem[SP-2] = (*reg)&0XFF;
	SP -= 2;
}

void POP(unsigned short *reg){
	*reg = mem[SP];
	*reg &= mem[SP+1]<<8;
	SP += 2;
}

// INC/DEC instructions

void INC(unsigned char *reg){
	*reg = (*reg)++;
	if((*reg) == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	if(((*reg)&0x0F) == 0x0F)
		setHalfCarry();
	else
		clearHalfCarry();
}

void DEC(unsigned char *reg){
	*reg = (*reg)--;
	if((*reg) == 0)
		setZero();
	else
		clearZero();
	setOperation();
	if(((*reg)&0x0F) == 0x00)
		setHalfCarry();
	else
		clearHalfCarry();
}

void INC(unsigned short *reg){
	unsigned short upper = (*reg)>>8;
	unsigned short lower = (*reg)^0x0F;
	upper++;
	upper &= 0xFF;
	if(upper == 0)
		lower++;
	upper = upper << 8;
	upper += lower;
	*reg = upper;
}

void DEC(unsigned short *reg){
	unsigned short upper = (*reg)>>8;
	unsigned short lower = (*reg)^0x0F;
	upper--;
	upper &= 0xFF;
	if(upper == 0XFF)
		lower--;
	upper = upper << 8;
	upper += lower;
	*reg = upper;

}

// 8 BIT ARITHMETIC excluding INC/DEC

void ADD(unsigned char *reg){
	short result = regAF.A + *reg;
	regAF.A += *reg;
	if(regAF.A == 0)
		setZero();
	else 
		clearZero();
	clearOperation();
	if((((regAF.A&0x0F)+((*reg)&0x0F))&0x10) == 0X10)
		setHalfCarry();
	else
		clearHalfCarry();
	if(result > 0XFF)
		setCarry();
	else
		clearCarry();
}

void ADC(unsigned char *reg){
	short result = regAF.A + *reg + (regAF.F>>4)&1;
	regAF.A += *reg;
	if(regAF.A == 0)
		setZero();
	else 
		clearZero();
	clearOperation();
	if(((regAF.A&0x0F)+((*reg)&0x0F)&0x10) == 0X10)
		setHalfCarry();
	else
		clearHalfCarry();
	if(result > 0XFF)
		setCarry();
	else
		clearCarry();
}

void SUB(unsigned char *reg){
	short result = regAF.A - *reg;
	regAF.A -= *reg;
	if(regAF.A == 0)
		setZero();
	else 
		clearZero();
	setOperation();
	if((regAF.A&0x0F)<((*reg)&0x0F))
		setHalfCarry();
	else
		clearHalfCarry();
	if(result < 0)
		setCarry();
	else
		clearCarry();
}

void SBC(unsigned char *reg){
	short result = regAF.A - *reg - (regAF.F>>4)&1;
	regAF.A -= *reg;
	if(regAF.A == 0)
		setZero();
	else 
		clearZero();
	setOperation();
	if((regAF.A&0x0F)<((*reg)&0x0F))
		setHalfCarry();
	else
		clearHalfCarry();
	if(result < 0)
		setCarry();
	else
		clearCarry();
}

void AND(unsigned char *reg){
	regAF.A &= *reg;
	if(regAF.A == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	setHalfCarry();
	clearCarry();
}

void OR(unsigned char *reg){
	regAF.A |= *reg;
	if(regAF.A == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	clearHalfCarry();
	clearCarry();
}

void XOR(unsigned char *reg){
	regAF.A ^= *reg;
	if(regAF.A == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	clearHalfCarry();
	clearCarry();
}

void CP(unsigned char *reg){
	short result = regAF.A - *reg;
	if(result == 0)
		setZero();
	else
		clearZero();
	setOperation();
	if((regAF.A&0x0F) < ((*reg)&0x0F))
		setHalfCarry();
	else
		clearHalfCarry();
	if(result<0)
		setCarry();
	else
		clearCarry();
}

// 16 BIT ARITHMETIC excluding INC/DEC

void ADDHL(unsigned short *reg){
	int result = regHL.HL + *reg;
	regHL.HL += *reg;
	if(result > 0xFFFF)
		setCarry();
	else
		clearCarry();
	if((((regHL.HL&0X0F) + (((*reg)&0x0F)))&0x10) == 0x10)
		setHalfCarry();
	else
		clearHalfCarry();
	clearOperation();
}

void ADDSPn(){
	unsigned short operand = mem[PC++];
	if(((operand>>7)&1) == 1)
	{
		operand ^= 0x80;
		operand = 0 - operand;
	}
	int result = SP += operand;
	clearZero();
	clearOperation();
	if(result > 0xFFFF)
		setCarry();
	else
		clearCarry();
	if((((SP&0X0F) + ((operand&0x0F)))&0x10) == 0x10)
		setHalfCarry();
	else
		clearHalfCarry();
}

//ROTATE

void RLCA(){
	unsigned char carry = 0;
	carry = regAF.A >> 7;
	regAF.A = regAF.A << 1;
	if(carry)
		setCarry();
	else
		clearCarry();
	regAF.A |= carry;
	clearHalfCarry();
	clearOperation();
	clearZero();
}

void RLA(){
	unsigned char oldCarry = (regAF.F>>4)&1;
	unsigned char carry = 0;
	carry = regAF.A >> 7;
	regAF.A = (regAF.A << 1)&oldCarry;
	if(carry)
		setCarry();
	else
		clearCarry();
	clearHalfCarry();
	clearOperation();
	clearZero();
}

void RRCA(){
	unsigned char carry = 0;
	carry = regAF.A&1;
	regAF.A = regAF.A >> 1;
	if(carry)
		setCarry();
	else
		clearCarry();
	carry = carry << 7;
	regAF.A |= carry;
	clearHalfCarry();
	clearOperation();
	clearZero();
}

void RRA(){
	unsigned char oldCarry = (regAF.F>>4)&1;
	unsigned char carry = 0;
	carry = regAF.A&1;
	regAF.A = regAF.A >> 1;
	if(carry)
		setCarry();
	else
		clearCarry();
	oldCarry = oldCarry << 7;
	regAF.A |= oldCarry;
	clearHalfCarry();
	clearOperation();
	clearZero();
}

void RLCs(unsigned char *reg){
	unsigned char carry = 0;
	carry = (*reg) >> 7;
	(*reg) = (*reg) << 1;
	if(carry)
		setCarry();
	else
		clearCarry();
	(*reg) |= carry;
	clearHalfCarry();
	clearOperation();
	if((*reg) == 0)
		setZero();
	else
		clearZero();
}

void RLs(unsigned char *reg){
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

void RRCs(unsigned char *reg){
	unsigned char carry = 0;
	carry = (*reg)&1;
	(*reg) = (*reg) >> 1;
	if(carry)
		setCarry();
	else
		clearCarry();
	carry = carry << 7;
	(*reg) |= carry;
	clearHalfCarry();
	clearOperation();
	if((*reg) == 0)
		setZero();
	else
		clearZero();
}

void RRs(unsigned char *reg){
	unsigned char oldCarry = ((*reg)>>4)&1;
	unsigned char carry = 0;
	carry = (*reg)&1;
	(*reg) = (*reg) >> 1;
	if(carry)
		setCarry();
	else
		clearCarry();
	oldCarry = oldCarry << 7;
	(*reg) |= oldCarry;
	clearHalfCarry();
	clearOperation();
	if((*reg) == 0)
		setZero();
	else
		clearZero();
}

// BIT OPCODES
void BIT(short c, unsigned char *reg){
	short s = (*reg)>>c;
	if(c == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	setHalfCarry();
}

//JUMPS

void JP(){
	unsigned short operand = 0;
	operand = mem[PC++];
	operand &= mem[PC++]<<8;
	PC = operand;
}

void JPNZ(){
	if((regAF.F&0x80) == 0)
		JP();
	else
		PC += 2;
}

void JPZ(){
	if(regAF.F&0x80)
		JP();
	else
		PC += 2;
}

void JPNC(){
	if(regAF.F&0x80)
		JP();
	else
		PC += 2;
}

void JPC(){
	if(regAF.F&0x80)
		JP();
	else
		PC += 2;
}

void JPHL(){
	PC = mem[regHL.HL];
}

void JRZn(){
	short j = mem[PC++];
	if(((j>>7)&1) == 1)
	{
		j ^= 0x80;
		j = 0 - j;
	}
	if((regAF.F&0x80) == 0)
		PC += j;
}

void JRNZn(){
	short j = mem[PC++];
	if(((j>>7)&1) == 1)
	{
		j ^= 0x80;
		j = 0 - j;
	}
	if((regAF.F&0x80) == 0x80)
		PC += j;
}

void JRCn(){
	short j = mem[PC++];
	if(((j>>7)&1) == 1)
	{
		j ^= 0x80;
		j = 0 - j;
	}
	if((regAF.F&0x10) == 0)
		PC += j;
}


void JRNCn(){
	short j = mem[PC++];
	if(((j>>7)&1) == 1)
	{
		j ^= 0x80;
		j = 0 - j;
	}
	if((regAF.F&0x10) == 0)
		PC += j;
}

void RET(){
	PC = mem[SP];
	PC &= mem[SP+1] << 8;
	SP += 2;
}

void RETZ(){
	if(regAF.F&0x80)
		RET();
}

void RETC(){
	if(regAF.F&0x10)
		RET();
}
void RETNZ(){
	if((regAF.F&0x80) == 0)
		RET();
}

void RETNC(){
	if((regAF.F&0x10) == 0)
		RET();
}

void RETI(){
	mem[0xFFFF] = 0xFF;
	RET();
}


//INTERRUPTS

void DI(){
	mem[0xFFFF] = 0x00;
}

void EI(){
	mem[0xFFFF] = 0xFF;
}

//CALL

void CALL(){
	unsigned short operand = 0;
	operand = mem[PC++];
	operand &= mem[PC++] << 8;
	mem[SP-1] = PC >> 8;
	mem[SP-2] = PC & 0XFF;
	PC = operand;
	SP -= 2;
}

void CALLZ(){
	if(regAF.F&0x80)
		CALL();
	else
		PC+=2;
}

void CALLNZ(){
	if((regAF.F&0x80) == 0)
		CALL();
	else
		PC+=2;
}

void CALLC(){
	if(regAF.F&0x10)
		CALL();
	else
		PC+=2;
}

void CALLNC(){
	if((regAF.F&0x10) == 0)
		CALL();
	else
		PC+=2;
}

void RST(unsigned char s){
	mem[SP-1] = PC >> 8;
	mem[SP-2] = PC & 0XFF;
	PC = s;
	SP -= 2;
}

// MISC

void CPL(){
	regAF.A = ~regAF.A;
}

void executeExtend(unsigned char operation){
	switch(operation)
	{
		case 0X7C:
			BIT(7,&regHL.H);
			break;
		default:
			cout << "Error: Unkown operand CB " << hex << (short)operation << " at " << hex << (short)PC << endl;
			exit(1); 
	}
}

void execute(unsigned char operation){
	switch(operation)
	{
		case 0x00:
			NOP();
			break;
		case 0x01:
			LDregnn(&regBC.BC);
			break;
		case 0x02:
			LDregAddress(&regAF.A,&regBC.BC);
			break;
		case 0x03:
			INC(&regBC.BC);
			break;
		case 0x04:
			INC(&regBC.B);
			break;
		case 0x05:
			DEC(&regBC.B);
			break;
		case 0x06:
			LDregn(&regBC.B);
			break;
		case 0x07:
			RLCA();
			break;
		case 0x08:
			LDnnreg16(&SP);
			break;
		case 0x09:
			ADDHL(&regBC.BC);
			break;
		case 0x0A:
			LDAddressreg(&regAF.A,&regBC.BC);
			break;
		case 0x0B:
			DEC(&regBC.BC);
			break;
		case 0x0C:
			INC(&regBC.C);
			break;
		case 0x0D:
			DEC(&regBC.C);
			break;
		case 0x0E:
			LDregn(&regBC.C);
			break;
		case 0x0F:
			RRCA();
			break;
		case 0x10:
			cout<<"0x10 STOP called"<<endl;
			exit(1);
			break;
		case 0x11:
			LDregnn(&regDE.DE);
			break;
		case 0x12:
			LDregAddress(&regAF.A,&regDE.DE);
			break;
		case 0x1A:
			LDAddressreg(&regAF.A,&regDE.DE);
			break;
		case 0x1B:
			DEC(&regDE.DE);
			break;
		case 0x1C:
			INC(&regDE.E);
			break;
		case 0x1D:
			DEC(&regDE.D);
			break;
		case 0x1E:
			LDregn(&regDE.E);
			break;
		case 0x1F:
			RRA();
			break;
		case 0x20:
			JRNZn();
			break;
		case 0x21:
			LDregnn(&regHL.HL);
			break;
		case 0x22:
			LDIHLA();
			break;
		case 0x23:
			INC(&regHL.HL);
			break;
		case 0x24:
			INC(&regHL.H);
			break;
		case 0x25:
			DEC(&regHL.H);
			break;
		case 0x26:
			LDregn(&regHL.H);
			break;
		case 0x27:
			cout<<"Implement 0x27"<<endl;
			exit(1);
		case 0x28:
			JRZn();
			break;
		case 0x29:
			ADDHL(&regHL.HL);
			break;
		case 0x2A:
			LDIAHL();
			break;
		case 0x2B:
			DEC(&regHL.HL);
			break;
		case 0x2C:
			INC(&regHL.L);
			break;
		case 0x2D:
			DEC(&regHL.L);
			break;
		case 0x2E:
			LDregn(&regHL.L);
			break;
		case 0x2F:
			CPL();
			break;
		case 0x30:
			JRNCn();
			break;
		case 0x31:
			LDregnn(&SP);
			break;
		case 0x32:
			LDDHLA();
			break;
		case 0x33:
			INC(&SP);
			break;
		case 0x34:
			INC(&mem[regHL.HL]);
			break;
		case 0x35:
			DEC(&mem[regHL.HL]);
			break;
		case 0x36:
			LDregn(&mem[regHL.HL]);
			break;
		case 0x37:
			setCarry();
			break;
		case 0x38:
			JRCn();
			break;
		case 0x39:
			ADDHL(&SP);
			break;
		case 0x3A:
			LDDAHL();
			break;
		case 0x3B:
			DEC(&SP);
			break;
		case 0x3C:
			INC(&regAF.A);
			break;
		case 0x3D:
			DEC(&regAF.A);
			break;
		case 0x3E:
			LDregn(&regAF.A);
			break;
		case 0x3F:
			clearCarry();
		case 0x40:
			LDregreg8bit(&regBC.B,&regBC.B);
			break;
		case 0x41:
			LDregreg8bit(&regBC.B,&regBC.C);
			break;
		case 0x42:
			LDregreg8bit(&regBC.B,&regDE.D);
			break;
		case 0x43:
			LDregreg8bit(&regBC.B,&regDE.E);
			break;
		case 0x44:
			LDregreg8bit(&regBC.B,&regHL.H);
			break;
		case 0x45:
			LDregreg8bit(&regBC.B,&regHL.L);
			break;
		case 0x46:
			LDAddressreg(&regBC.B,&regHL.HL);
			break;
		case 0x47:
			LDregreg8bit(&regBC.B,&regAF.A);
			break;
		case 0x48:
			LDregreg8bit(&regBC.C,&regBC.B);
			break;
		case 0x49:
			LDregreg8bit(&regBC.C,&regBC.C);
			break;
		case 0x4A:
			LDregreg8bit(&regBC.C,&regDE.D);
			break;
		case 0x4B:
			LDregreg8bit(&regBC.C,&regDE.E);
			break;
		case 0x4C:
			LDregreg8bit(&regBC.C,&regHL.H);
			break;
		case 0x4D:
			LDregreg8bit(&regBC.C,&regHL.L);
			break;
		case 0x4E:
			LDAddressreg(&regBC.B,&regHL.HL);
			break;
		case 0x4F:
			LDregreg8bit(&regBC.C,&regAF.A);
			break;
		case 0x50:
			LDregreg8bit(&regDE.D,&regBC.B);
			break;
		case 0x51:
			LDregreg8bit(&regDE.D,&regBC.C);
			break;
		case 0x52:
			LDregreg8bit(&regDE.D,&regDE.D);
			break;
		case 0x53:
			LDregreg8bit(&regDE.D,&regDE.E);
			break;
		case 0x54:
			LDregreg8bit(&regDE.D,&regHL.H);
			break;
		case 0x55:
			LDregreg8bit(&regDE.D,&regHL.L);
			break;
		case 0x56:
			LDAddressreg(&regDE.D,&regHL.HL);
			break;
		case 0x57:
			LDregreg8bit(&regDE.D,&regAF.A);
			break;
		case 0x58:
			LDregreg8bit(&regDE.E,&regBC.B);
			break;
		case 0x59:
			LDregreg8bit(&regDE.E,&regBC.C);
			break;
		case 0x5A:
			LDregreg8bit(&regDE.E,&regDE.D);
			break;
		case 0x5B:
			LDregreg8bit(&regDE.E,&regDE.E);
			break;
		case 0x5C:
			LDregreg8bit(&regDE.E,&regHL.H);
			break;
		case 0x5D:
			LDregreg8bit(&regDE.E,&regHL.L);
			break;
		case 0x5E:
			LDAddressreg(&regDE.E,&regHL.HL);
			break;
		case 0x5F:
			LDregreg8bit(&regDE.E,&regAF.A);
			break;
		case 0x60:
			LDregreg8bit(&regHL.H,&regBC.B);
			break;
		case 0x61:
			LDregreg8bit(&regHL.H,&regBC.C);
			break;
		case 0x62:
			LDregreg8bit(&regHL.H,&regDE.D);
			break;
		case 0x63:
			LDregreg8bit(&regHL.H,&regDE.E);
			break;
		case 0x64:
			LDregreg8bit(&regHL.H,&regHL.H);
			break;
		case 0x65:
			LDregreg8bit(&regHL.H,&regHL.L);
			break;
		case 0x66:
			LDAddressreg(&regHL.H,&regHL.HL);
			break;
		case 0x67:
			LDregreg8bit(&regHL.H,&regAF.A);
			break;
		case 0x68:
			LDregreg8bit(&regHL.L,&regBC.B);
			break;
		case 0x69:
			LDregreg8bit(&regHL.L,&regBC.C);
			break;
		case 0x6A:
			LDregreg8bit(&regHL.L,&regDE.D);
			break;
		case 0x6B:
			LDregreg8bit(&regHL.L,&regDE.E);
			break;
		case 0x6C:
			LDregreg8bit(&regHL.L,&regHL.H);
			break;
		case 0x6D:
			LDregreg8bit(&regHL.L,&regHL.L);
			break;
		case 0x6E:
			LDAddressreg(&regHL.L,&regHL.HL);
			break;
		case 0x6F:
			LDregreg8bit(&regHL.L,&regAF.A);
			break;
		case 0x70:
			LDregAddress(&regHL.HL,&regBC.B);
			break;
		case 0x71:
			LDregAddress(&regHL.HL,&regBC.C);
			break;
		case 0x72:
			LDregAddress(&regHL.HL,&regDE.D);
			break;
		case 0x73:
			LDregAddress(&regHL.HL,&regDE.E);
			break;
		case 0x74:
			LDregAddress(&regHL.HL,&regHL.H);
			break;
		case 0x75:
			LDregAddress(&regHL.HL,&regHL.L);
			break;
		case 0x76:
			cout<<"implement HALT 0x76"<<endl;
			exit(1);
			break;
		case 0x77:
			LDregAddress(&regAF.A,&regHL.HL);
			break;
		case 0x78:
			LDregreg8bit(&regAF.A,&regBC.B);
			break;
		case 0x79:
			LDregreg8bit(&regAF.A,&regBC.C);
			break;
		case 0x7A:
			LDregreg8bit(&regAF.A,&regDE.D);
			break;
		case 0x7B:
			LDregreg8bit(&regAF.A,&regDE.E);
			break;
		case 0x7C:
			LDregreg8bit(&regAF.A,&regHL.H);
			break;
		case 0x7D:
			LDregreg8bit(&regAF.A,&regHL.L);
			break;
		case 0x7E:
			LDAddressreg(&regAF.A,&regHL.HL);
			break;
		case 0x7F:
			LDregreg8bit(&regAF.A,&regAF.A);
			break;
		case 0x80:
			ADD(&regBC.B);
			break;
		case 0x81:
			ADD(&regBC.C);
			break;
		case 0x82:
			ADD(&regDE.D);
			break;
		case 0x83:
			ADD(&regDE.E);
			break;
		case 0x84:
			ADD(&regHL.H);
			break;
		case 0x85:
			ADD(&regHL.L);
			break;
		case 0x86:
			ADD(&mem[regHL.HL]);
			break;
		case 0x87:
			ADD(&regAF.A);
			break;
		case 0x88:
			ADC(&regBC.B);
			break;
		case 0x89:
			ADC(&regBC.C);
			break;
		case 0x8A:
			ADC(&regDE.D);
			break;
		case 0x8B:
			ADC(&regDE.E);
			break;
		case 0x8C:
			ADC(&regHL.H);
			break;
		case 0x8D:
			ADC(&regHL.L);
			break;
		case 0x8E:
			ADC(&mem[regHL.HL]);
			break;
		case 0x8F:
			ADC(&regAF.A);
			break;
		case 0x90:
			SUB(&regBC.B);
			break;
		case 0x91:
			SUB(&regBC.C);
			break;
		case 0x92:
			SUB(&regDE.D);
			break;
		case 0x93:
			SUB(&regDE.E);
			break;
		case 0x94:
			SUB(&regHL.H);
			break;
		case 0x95:
			SUB(&regHL.L);
			break;
		case 0x96:
			SUB(&mem[regHL.HL]);
			break;
		case 0x97:
			SUB(&regAF.A);
			break;
		case 0x98:
			SBC(&regBC.B);
			break;
		case 0x99:
			SBC(&regBC.C);
			break;
		case 0x9A:
			SBC(&regDE.D);
			break;
		case 0x9B:
			SBC(&regDE.E);
			break;
		case 0x9C:
			SBC(&regHL.H);
			break;
		case 0x9D:
			SBC(&regHL.L);
			break;
		case 0x9E:
			SBC(&mem[regHL.HL]);
			break;
		case 0x9F:
			SBC(&regAF.A);
			break;
		case 0xA0:
			AND(&regBC.B);
			break;
		case 0xA1:
			AND(&regBC.C);
			break;
		case 0xA2:
			AND(&regDE.D);
			break;
		case 0xA3:
			AND(&regDE.E);
			break;
		case 0xA4:
			AND(&regHL.H);
			break;
		case 0xA5:
			AND(&regHL.L);
			break;
		case 0xA6:
			AND(&mem[regHL.HL]);
			break;
		case 0xA7:
			AND(&regAF.A);
			break;
		case 0xA8:
			XOR(&regBC.B);
			break;
		case 0xA9:
			XOR(&regBC.C);
			break;
		case 0xAA:
			XOR(&regDE.D);
			break;
		case 0xAB:
			XOR(&regDE.E);
			break;
		case 0xAC:
			XOR(&regHL.H);
			break;
		case 0xAD:
			XOR(&regHL.L);
			break;
		case 0xAE:
			XOR(&mem[regHL.HL]);
			break;
		case 0xAF:
			XOR(&regAF.A);
			break;
		case 0xB1:
			OR(&regBC.C);
			break;
		case 0xB2:
			OR(&regDE.D);
			break;
		case 0xB3:
			OR(&regDE.E);
			break;
		case 0xB4:
			OR(&regHL.H);
			break;
		case 0xB5:
			OR(&regHL.L);
			break;
		case 0xB6:
			OR(&mem[regHL.HL]);
			break;
		case 0xB7:
			OR(&regAF.A);
			break;
		case 0xB8:
			CP(&regBC.B);
			break;
		case 0xB9:
			CP(&regBC.C);
			break;
		case 0xBA:
			CP(&regDE.D);
			break;
		case 0xBB:
			CP(&regDE.E);
			break;
		case 0xBC:
			CP(&regHL.H);
			break;
		case 0xBD:
			CP(&regHL.L);
			break;
		case 0xBE:
			CP(&mem[regHL.HL]);
			break;
		case 0xBF:
			CP(&regAF.A);
			break;
		case 0xC0:
			RETNZ();
			break;
		case 0xC1:
			POP(&regBC.BC);
			break;
		case 0xC2:
			JPNZ();
			break;
		case 0xC3:
			JP();
			break;
		case 0xC4:
			CALLNZ();
			break;
		case 0xC5:
			PUSH(&regBC.BC);
			break;
		case 0xC6:
			ADD(&mem[PC++]);
			break;
		case 0xC7:
			RST(0);
			break;
		case 0xC8:
			RETZ();
			break;
		case 0xC9:
			RET();
			break;
		case 0xCA:
			JPZ();
			break;
		case 0xCB:
			executeExtend(mem[PC++]);
			break;
		case 0xCC:
			CALLZ();
			break;
		case 0xCD:
			CALL();
			break;
		case 0xCE:
			ADC(&mem[PC++]);		
			break;
		case 0xCF:
			RST(0X8);
			break;
		case 0xD0:
			RETNC();
			break;
		case 0xD1:
			POP(&regDE.DE);
			break;
		case 0xD2:
			JPNC();
			break;
		case 0xD4:
			CALLNC();
			break;
		case 0xD5:
			PUSH(&regDE.DE);
			break;
		case 0xD6:
			SUB(&mem[PC++]);
			break;
		case 0xD7:
			RST(0X10);
			break;
		case 0xD8:
			RETC();
			break;
		case 0xD9:
			RETI();
			break;
		case 0xDA:
			JPC();
			break;
		case 0xDC:
			CALLC();
			break;
		case 0xDE:
			SBC(&mem[PC++]);
			break;
		case 0xDF:
			RST(0x18);
			break;
		case 0xE0:
			LDHnA();
			break;
		case 0xE1:
			POP(&regHL.HL);
			break;
		case 0xE2:
			LDHCA();
			break;
		case 0xE5:
			PUSH(&regHL.HL);
			break;
		case 0xE6:
			AND(&mem[PC++]);
			break;
		case 0xE7:
			RST(0x20);
			break;
		case 0xE8:
			ADDSPn();
			break;
		case 0xE9:
			JPHL();
			break;
		case 0xEA:
			LDnnreg(&regAF.A);
			break;
		case 0xEE:
			XOR(&mem[PC++]);
			break;
		case 0xEF:
			RST(0x28);
			break;
		case 0xF0:
			LDHAn();
			break;
		case 0xF1:
			POP(&regAF.AF);
			break;
		case 0xF3:
			DI();
			break;
		case 0xF5:
			PUSH(&regAF.AF);
			break;
		case 0xF6:
			OR(&mem[PC++]);
			break;
		case 0xF7:
			RST(0x30);
			break;
		case 0xF8:
			LDHLSP();
			break;
		case 0xF9:
			LDregreg16bit(&SP,&regHL.HL);
			break;
		case 0xFA:
			LDAnn();
			break;
		case 0xFB:
			EI();
			break;
		case 0xFE:
			CP(&mem[PC++]);
			break;
		case 0xFF:
			RST(0X38);
			break;
		default:
			cout << "Error: Unkown operand " << hex << (short)operation << " at " << hex << (short)PC << endl;
			exit(1); 
	}
}