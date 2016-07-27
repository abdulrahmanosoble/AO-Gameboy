#include<iostream>
#include<stdlib.h>
#include "execute.h"
#include "mem.h"

using namespace std;

extern RegAF regAF;
extern RegBC regBC;
extern RegDE regDE;
extern RegHL regHL;
extern unsigned short SP;
extern unsigned short PC;
extern short cycles;

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
	if (result == 0)
		regAF.F |= 0x80;
	if (!addition)
		regAF.F |= 0X40;
	if (addition)
	{
		if (eightbit)
		{
			if (result > 255)
				regAF.F |= 0X10;
		}
		else
		{
			if (result > 65535)
				regAF.F |= 0X10;
		}
	}
	else
	{
		if (result < 0)
			regAF.F |= 0X10;
	}
}

//NOP

void NOP(){
	cycles = 4;
}

// LD instructions

void LDregn(unsigned char *reg){
	*reg = mem[PC++];
	if (reg == &mem[regHL.HL] || reg == &mem[PC - 2])
		cycles = 8;
	else
		cycles = 4;
}

void LDregnn(unsigned short *reg){
	unsigned short operand = 0;
	operand = mem[PC++];
	operand += mem[PC++] << 8;
	*reg = operand;
	cycles = 12;
}

void LDnnreg(unsigned char *reg){
	unsigned short operand = 0;
	operand = mem[PC++];
	operand += mem[PC++] << 8;
	mem[operand] = *reg;
	cycles = 12;
}

void LDregAddress(unsigned char *reg1, unsigned short *reg2){
	mem[*reg2] = *reg1;
	cycles = 8;
}

void LDregAddress(unsigned short *reg1, unsigned char *reg2){
	mem[*reg2] = *reg1;
	cycles = 8;
}

void LDAddressreg(unsigned char *reg1, unsigned short *reg2){
	*reg1 = mem[*reg2];
	cycles = 8;
}

void LDnnreg16(unsigned short *reg){
	unsigned short operand = 0;
	operand = mem[PC++];
	operand += mem[PC++] << 8;
	mem[operand] = SP & 0x0F;
	mem[operand + 1] = SP >> 8;
	cycles = 20;
}

void LDregreg8bit(unsigned char *reg1, unsigned char *reg2){
	*reg1 = *reg2;
	cycles = 8;
}

void LDregreg16bit(unsigned short *reg1, unsigned short *reg2){
	*reg1 = *reg2;
	cycles = 12;
}

void LDDHLA(){
	mem[regHL.HL--] = regAF.A;
	cycles = 8;
}

void LDDAHL(){
	regAF.A = mem[regHL.HL--];
	cycles = 8;
}

void LDHCA(){
	mem[0xFF00 + regBC.C] = regAF.A;
	cycles = 8;
}

void LDHnA(){
	mem[0xFF00 + mem[PC++]] = regAF.A;
	cycles = 12;
}

void LDHAn(){
	regAF.A = mem[0xFF00 + mem[PC++]];
	cycles = 12;
}

void LDIAHL(){
	regAF.A = mem[regHL.HL++];
	cycles = 8;
}

void LDIHLA(){
	mem[regHL.HL++] = regAF.A;
	cycles = 8;
}

void LDHLSP(){
	regHL.HL = SP + mem[PC++];
	cycles = 12;
}

void LDAnn(){
	unsigned short operand = 0;
	operand = mem[PC++];
	operand += mem[PC++] << 8;
	regAF.A = mem[operand];
	cycles = 8;
}

void PUSH(unsigned short *reg){
	mem[SP - 1] = (*reg) >> 8;
	mem[SP - 2] = (*reg) & 0XFF;
	SP -= 2;
	cycles = 16;
}

void POP(unsigned short *reg){
	*reg = mem[SP];
	*reg += mem[SP + 1] << 8;
	SP += 2;
	cycles = 12;
}

// INC/DEC instructions

void INC(unsigned char *reg){
	(*reg)++;
	if ((*reg) == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	if ((*reg) == 0x00)
		setHalfCarry();
	else
		clearHalfCarry();
	if (reg == &mem[regHL.HL])
		cycles = 12;
	else
		cycles = 4;
}

void DEC(unsigned char *reg){
	(*reg)--;
	if ((*reg) == 0)
		setZero();
	else
		clearZero();
	setOperation();
	if ((*reg) == 0xFF)
		setHalfCarry();
	else
		clearHalfCarry();
	if (reg == &mem[regHL.HL])
		cycles = 12;
	else
		cycles = 4;
}

void INC(unsigned short *reg){
	unsigned short upper = (*reg) >> 8;
	unsigned short lower = (*reg) ^ 0x0F;
	upper++;
	upper &= 0xFF;
	if (upper == 0)
		lower++;
	upper = upper << 8;
	upper += lower;
	*reg = upper;
	cycles = 8;
}

void DEC(unsigned short *reg){
	unsigned short upper = (*reg) >> 8;
	unsigned short lower = (*reg) ^ 0x0F;
	upper--;
	upper &= 0xFF;
	if (upper == 0XFF)
		lower--;
	upper = upper << 8;
	upper += lower;
	*reg = upper;
	cycles = 8;
}

// 8 BIT ARITHMETIC excluding INC/DEC

void ADD(unsigned char *reg){
	short result = regAF.A + *reg;
	regAF.A += *reg;
	if (regAF.A == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	if ((((regAF.A & 0x0F) + ((*reg) & 0x0F)) & 0x10) == 0X10)
		setHalfCarry();
	else
		clearHalfCarry();
	if (result > 0XFF)
		setCarry();
	else
		clearCarry();
	if (reg == &mem[PC - 1] || reg == &mem[regHL.HL])
		cycles = 8;
	else
		cycles = 4;
}

void ADC(unsigned char *reg){
	short result = regAF.A + *reg + (regAF.F >> 4) & 1;
	regAF.A += *reg;
	if (regAF.A == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	if (((regAF.A & 0x0F) + ((*reg) & 0x0F) & 0x10) == 0X10)
		setHalfCarry();
	else
		clearHalfCarry();
	if (result > 0XFF)
		setCarry();
	else
		clearCarry();
	if (reg == &mem[PC - 1] || reg == &mem[regHL.HL])
		cycles = 8;
	else
		cycles = 4;
}

void SUB(unsigned char *reg){
	short result = regAF.A - *reg;
	regAF.A -= *reg;
	if (regAF.A == 0)
		setZero();
	else
		clearZero();
	setOperation();
	if ((regAF.A & 0x0F)<((*reg) & 0x0F))
		setHalfCarry();
	else
		clearHalfCarry();
	if (result < 0)
		setCarry();
	else
		clearCarry();
	if (reg == &mem[PC - 1] || reg == &mem[regHL.HL])
		cycles = 8;
	else
		cycles = 4;
}

void SBC(unsigned char *reg){
	short result = regAF.A - *reg - (regAF.F >> 4) & 1;
	regAF.A -= *reg;
	if (regAF.A == 0)
		setZero();
	else
		clearZero();
	setOperation();
	if ((regAF.A & 0x0F)<((*reg) & 0x0F))
		setHalfCarry();
	else
		clearHalfCarry();
	if (result < 0)
		setCarry();
	else
		clearCarry();
	if (reg == &mem[PC - 1] || reg == &mem[regHL.HL])
		cycles = 8;
	else
		cycles = 4;
}

void AND(unsigned char *reg){
	regAF.A &= *reg;
	if (regAF.A == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	setHalfCarry();
	clearCarry();
	if (reg == &mem[PC - 1] || reg == &mem[regHL.HL])
		cycles = 8;
	else
		cycles = 4;
}

void OR(unsigned char *reg){
	regAF.A |= *reg;
	if (regAF.A == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	clearHalfCarry();
	clearCarry();
	if (reg == &mem[PC - 1] || reg == &mem[regHL.HL])
		cycles = 8;
	else
		cycles = 4;
}

void XOR(unsigned char *reg){
	regAF.A ^= *reg;
	if (regAF.A == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	clearHalfCarry();
	clearCarry();
	if (reg == &mem[PC - 1] || reg == &mem[regHL.HL])
		cycles = 8;
	else
		cycles = 4;
}

void CP(unsigned char *reg){
	short result = regAF.A - *reg;
	if (result == 0)
		setZero();
	else
		clearZero();
	setOperation();
	if ((regAF.A & 0x0F) < ((*reg) & 0x0F))
		setHalfCarry();
	else
		clearHalfCarry();
	if (result<0)
		setCarry();
	else
		clearCarry();
	if (reg == &mem[PC - 1] || reg == &mem[regHL.HL])
		cycles = 8;
	else
		cycles = 4;
}

// 16 BIT ARITHMETIC excluding INC/DEC

void ADDHL(unsigned short *reg){
	int result = regHL.HL + *reg;
	regHL.HL += *reg;
	if (result > 0xFFFF)
		setCarry();
	else
		clearCarry();
	if ((((regHL.HL & 0X0F) + (((*reg) & 0x0F))) & 0x10) == 0x10)
		setHalfCarry();
	else
		clearHalfCarry();
	clearOperation();
	cycles = 8;
}

void ADDSPn(){
	unsigned short operand = mem[PC++];
	if (((operand >> 7) & 1) == 1)
	{
		operand ^= 0x80;
		operand = 0 - operand;
	}
	int result = SP += operand;
	clearZero();
	clearOperation();
	if (result > 0xFFFF)
		setCarry();
	else
		clearCarry();
	if ((((SP & 0X0F) + ((operand & 0x0F))) & 0x10) == 0x10)
		setHalfCarry();
	else
		clearHalfCarry();
	cycles = 16;
}

//ROTATE

void RLCA(){
	unsigned char carry = 0;
	carry = regAF.A >> 7;
	regAF.A = regAF.A << 1;
	if (carry)
		setCarry();
	else
		clearCarry();
	regAF.A |= carry;
	clearHalfCarry();
	clearOperation();
	clearZero();
	cycles = 4;
}

void RLA(){
	unsigned char oldCarry = (regAF.F >> 4) & 1;
	unsigned char carry = 0;
	carry = regAF.A >> 7;
	regAF.A = (regAF.A << 1)&oldCarry;
	if (carry)
		setCarry();
	else
		clearCarry();
	clearHalfCarry();
	clearOperation();
	clearZero();
	cycles = 4;
}

void RRCA(){
	unsigned char carry = 0;
	carry = regAF.A & 1;
	regAF.A = regAF.A >> 1;
	if (carry)
		setCarry();
	else
		clearCarry();
	carry = carry << 7;
	regAF.A |= carry;
	clearHalfCarry();
	clearOperation();
	clearZero();
	cycles = 4;
}

void RRA(){
	unsigned char oldCarry = (regAF.F >> 4) & 1;
	unsigned char carry = 0;
	carry = regAF.A & 1;
	regAF.A = regAF.A >> 1;
	if (carry)
		setCarry();
	else
		clearCarry();
	oldCarry = oldCarry << 7;
	regAF.A |= oldCarry;
	clearHalfCarry();
	clearOperation();
	clearZero();
	cycles = 4;
}

void RLC(unsigned char *reg){
	unsigned char carry = 0;
	carry = (*reg) >> 7;
	(*reg) = (*reg) << 1;
	if (carry)
		setCarry();
	else
		clearCarry();
	(*reg) |= carry;
	clearHalfCarry();
	clearOperation();
	if ((*reg) == 0)
		setZero();
	else
		clearZero();
	if (reg == &mem[regHL.HL])
		cycles = 16;
	else
		cycles = 8;
}

void RL(unsigned char *reg){
	unsigned char carry = 0;
	carry = *reg >> 7;
	*reg = ((*reg) << 1)&carry;
	if (carry)
		setCarry();
	else
		clearCarry();
	clearHalfCarry();
	clearOperation();
	if ((*reg) == 0)
		setZero();
	else
		clearZero();
	if (reg == &mem[regHL.HL])
		cycles = 16;
	else
		cycles = 8;
}

void RRC(unsigned char *reg){
	unsigned char carry = 0;
	carry = (*reg) & 1;
	(*reg) = (*reg) >> 1;
	if (carry)
		setCarry();
	else
		clearCarry();
	carry = carry << 7;
	(*reg) |= carry;
	clearHalfCarry();
	clearOperation();
	if ((*reg) == 0)
		setZero();
	else
		clearZero();
	if (reg == &mem[regHL.HL])
		cycles = 16;
	else
		cycles = 8;
}

void RR(unsigned char *reg){
	unsigned char oldCarry = ((*reg) >> 4) & 1;
	unsigned char carry = 0;
	carry = (*reg) & 1;
	(*reg) = (*reg) >> 1;
	if (carry)
		setCarry();
	else
		clearCarry();
	oldCarry = oldCarry << 7;
	(*reg) |= oldCarry;
	clearHalfCarry();
	clearOperation();
	if ((*reg) == 0)
		setZero();
	else
		clearZero();
	if (reg == &mem[regHL.HL])
		cycles = 16;
	else
		cycles = 8;
}

void SLA(unsigned char *reg){
	short carry = (*reg) >> 7;
	(*reg) = (*reg) << 1;
	if (carry)
		setCarry();
	else
		clearCarry();
	if (*reg == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	clearHalfCarry();
	if (reg == &mem[regHL.HL])
		cycles = 16;
	else
		cycles = 8;
}

void SRA(unsigned char *reg){
	short carry = (*reg) & 1;
	short old = (*reg) & 0x80;;
	(*reg) = (*reg) >> 1;
	(*reg) |= old;
	if (carry)
		setCarry();
	else
		clearCarry();
	if (*reg == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	clearHalfCarry();
	if (reg == &mem[regHL.HL])
		cycles = 16;
	else
		cycles = 8;
}

void SRL(unsigned char *reg){
	short carry = (*reg) & 1;
	(*reg) = (*reg) >> 1;
	if (carry)
		setCarry();
	else
		clearCarry();
	if (*reg == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	clearHalfCarry();
	if (reg == &mem[regHL.HL])
		cycles = 16;
	else
		cycles = 8;
}

// BIT OPCODES
void BIT(short c, unsigned char *reg){
	short s = (*reg) >> c;
	if (c == 0)
		setZero();
	else
		clearZero();
	clearOperation();
	setHalfCarry();
	if (reg == &mem[regHL.HL])
		cycles = 12;
	else
		cycles = 8;
}

void SET(short c, unsigned char *reg){
	short s = 1;
	s = s << c;
	(*reg) |= s;
	if (reg == &mem[regHL.HL])
		cycles = 16;
	else
		cycles = 8;
}

void RES(short c, unsigned char *reg){
	short s = 0xFF;
	short t = 1;
	t = t << c;
	s ^= t;
	(*reg) &= s;
	if (reg == &mem[regHL.HL])
		cycles = 16;
	else
		cycles = 8;
}

void SWAP(unsigned char *reg){
	short higher = *reg >> 4;
	short lower = *reg & 0x0F;
	lower = lower << 4;
	higher += lower;
	*reg = higher;
	if (reg == &mem[regHL.HL])
		cycles = 16;
	else
		cycles = 8;
}
//JUMPS

void JP(){
	unsigned short operand = 0;
	operand = mem[PC++];
	operand += mem[PC++] << 8;
	PC = operand;
	cycles = 16;
}

void JPNZ(){
	if ((regAF.F & 0x80) == 0)
		JP();
	else
	{
		PC += 2;
		cycles = 12;
	}
}

void JPZ(){
	if (regAF.F & 0x80)
		JP();
	else
	{
		PC += 2;
		cycles = 12;
	}
}

void JPNC(){
	if (regAF.F & 0x80)
		JP();
	else
	{
		PC += 2;
		cycles = 12;
	}
}

void JPC(){
	if (regAF.F & 0x80)
		JP();
	else
	{
		PC += 2;
		cycles = 12;
	}
}

void JPHL(){
	PC = mem[regHL.HL];
	cycles = 4;
}

void JRZn(){
	short j = mem[PC++];
	if (j & 0x80)
		j = 0 - (0xFF - j) - 1;
	else
		j--;
	if (regAF.F & 0x80)
	{
		cycles = 12;
		PC += j;
	}
	else cycles = 8;
}

void JRNZn(){
	short j = mem[PC++];
	if (j & 0x80)
		j = 0 - (0xFF - j) - 1;
	else
		j--;
	if ((regAF.F & 0x80) == 0)
	{
		cycles = 12;
		PC += j;
	}
	else cycles = 8;
}

void JRCn(){
	short j = mem[PC++];
	if (j & 0x80)
		j = 0 - (0xFF - j) - 1;
	else
		j--;
	if (regAF.F & 0x10)
	{
		cycles = 12;
		PC += j;
	}
	else cycles = 8;
}


void JRNCn(){
	short j = mem[PC++];
	if (j & 0x80 == 0)
		j = 0 - (0xFF - j) - 1;
	else
		j--;
	if ((regAF.F & 0x10) == 0)
	{
		cycles = 12;
		PC += j;
	}
	else cycles = 8;
}

void RET(){
	PC = mem[SP];
	PC += mem[SP + 1] << 8;
	SP += 2;
	cycles = 16;
}

void RETZ(){
	if (regAF.F & 0x80)
	{
		cycles = 20;
		RET();
	}
	else
		cycles = 8;
}

void RETC(){
	if (regAF.F & 0x10)
	{
		cycles = 20;
		RET();
	}
	else
		cycles = 8;
}
void RETNZ(){
	if ((regAF.F & 0x80) == 0)
	{
		cycles = 20;
		RET();
	}
	else
		cycles = 8;
}

void RETNC(){
	if ((regAF.F & 0x10) == 0)
	{
		cycles = 20;
		RET();
	}
	else
		cycles = 8;
}

void RETI(){
	mem[0xFFFF] = 0xFF;
	RET();
	cycles = 16;
}


//INTERRUPTS

void DI(){
	mem[0xFFFF] = 0x00;
	cycles = 4;
}

void EI(){
	mem[0xFFFF] = 0xFF;
	cycles = 4;
}

//CALL

void CALL(){
	unsigned short operand = 0;
	operand = mem[PC++];
	operand += mem[PC++] << 8;
	mem[SP - 1] = PC >> 8;
	mem[SP - 2] = PC & 0XFF;
	PC = operand;
	SP -= 2;
	cycles = 24;
}

void CALLZ(){
	if (regAF.F & 0x80)
		CALL();
	else
	{
		PC += 2;
		cycles = 12;
	}
}

void CALLNZ(){
	if ((regAF.F & 0x80) == 0)
		CALL();
	else
	{
		PC += 2;
		cycles = 12;
	}
}

void CALLC(){
	if (regAF.F & 0x10)
		CALL();
	else
	{
		PC += 2;
		cycles = 12;
	}
}

void CALLNC(){
	if ((regAF.F & 0x10) == 0)
		CALL();
	else
	{
		PC += 2;
		cycles = 12;
	}
}

void RST(unsigned char s){
	mem[SP - 1] = PC >> 8;
	mem[SP - 2] = PC & 0XFF;
	PC = s;
	SP -= 2;
	cycles = 16;
}

// MISC

void CPL(){
	regAF.A = ~regAF.A;
	cycles = 4;
}

void executeExtend(unsigned char operation){
	switch (operation)
	{
	case 0x00:
		RLC(&regBC.B);
		break;
	case 0x01:
		RLC(&regBC.C);
		break;
	case 0x02:
		RLC(&regDE.D);
		break;
	case 0x03:
		RLC(&regDE.E);
		break;
	case 0x04:
		RLC(&regHL.H);
		break;
	case 0x05:
		RLC(&regHL.L);
		break;
	case 0x06:
		RLC(&mem[regHL.HL]);
		break;
	case 0x07:
		RLC(&regAF.A);
		break;
	case 0x08:
		RRC(&regBC.B);
		break;
	case 0x09:
		RRC(&regBC.C);
		break;
	case 0x0A:
		RRC(&regDE.D);
		break;
	case 0x0B:
		RRC(&regDE.E);
		break;
	case 0x0C:
		RRC(&regHL.H);
		break;
	case 0x0D:
		RRC(&regHL.L);
		break;
	case 0x0E:
		RRC(&mem[regHL.HL]);
		break;
	case 0x0F:
		RRC(&regAF.A);
		break;
	case 0x10:
		RL(&regBC.B);
		break;
	case 0x11:
		RL(&regBC.C);
		break;
	case 0x12:
		RL(&regDE.D);
		break;
	case 0x13:
		RL(&regDE.E);
		break;
	case 0x14:
		RL(&regHL.H);
		break;
	case 0x15:
		RL(&regHL.L);
		break;
	case 0x16:
		RL(&mem[regHL.HL]);
		break;
	case 0x17:
		RL(&regAF.A);
		break;
	case 0x18:
		RR(&regBC.B);
		break;
	case 0x19:
		RR(&regBC.C);
		break;
	case 0x1A:
		RR(&regDE.D);
		break;
	case 0x1B:
		RR(&regDE.E);
		break;
	case 0x1C:
		RR(&regHL.H);
		break;
	case 0x1D:
		RR(&regHL.L);
		break;
	case 0x1E:
		RR(&mem[regHL.HL]);
		break;
	case 0x1F:
		RR(&regAF.A);
		break;
	case 0x20:
		SLA(&regBC.B);
		break;
	case 0x21:
		SLA(&regBC.C);
		break;
	case 0x22:
		SLA(&regDE.D);
		break;
	case 0x23:
		SLA(&regDE.E);
		break;
	case 0x24:
		SLA(&regHL.H);
		break;
	case 0x25:
		SLA(&regHL.L);
		break;
	case 0x26:
		SLA(&mem[regHL.HL]);
		break;
	case 0x27:
		SLA(&regAF.A);
		break;
	case 0x28:
		SRA(&regBC.B);
		break;
	case 0x29:
		SRA(&regBC.C);
		break;
	case 0x2A:
		SRA(&regDE.D);
		break;
	case 0x2B:
		SRA(&regDE.E);
		break;
	case 0x2C:
		SRA(&regHL.H);
		break;
	case 0x2D:
		SRA(&regHL.L);
		break;
	case 0x2E:
		SRA(&mem[regHL.HL]);
		break;
	case 0x2F:
		SRA(&regAF.A);
		break;
	case 0x30:
		SWAP(&regBC.B);
		break;
	case 0x31:
		SWAP(&regBC.C);
		break;
	case 0x32:
		SWAP(&regDE.D);
		break;
	case 0x33:
		SWAP(&regDE.E);
		break;
	case 0x34:
		SWAP(&regHL.H);
		break;
	case 0x35:
		SWAP(&regHL.L);
		break;
	case 0x36:
		SWAP(&mem[regHL.HL]);
		break;
	case 0x37:
		SWAP(&regAF.A);
		break;
	case 0x38:
		SRL(&regBC.B);
		break;
	case 0x39:
		SRL(&regBC.C);
		break;
	case 0x3A:
		SRL(&regDE.D);
		break;
	case 0x3B:
		SRL(&regDE.E);
		break;
	case 0x3C:
		SRL(&regHL.H);
		break;
	case 0x3D:
		SRL(&regHL.L);
		break;
	case 0x3E:
		SRL(&mem[regHL.HL]);
		break;
	case 0x3F:
		SRL(&regAF.A);
		break;
	case 0x40:
		BIT(0, &regBC.B);
		break;
	case 0x41:
		BIT(0, &regBC.C);
		break;
	case 0x42:
		BIT(0, &regDE.D);
		break;
	case 0x43:
		BIT(0, &regDE.E);
		break;
	case 0x44:
		BIT(0, &regHL.H);
		break;
	case 0x45:
		BIT(0, &regHL.L);
		break;
	case 0x46:
		BIT(0, &mem[regHL.HL]);
		break;
	case 0x47:
		BIT(1, &regAF.A);
		break;
	case 0x48:
		BIT(1, &regBC.B);
		break;
	case 0x49:
		BIT(1, &regBC.C);
		break;
	case 0x4A:
		BIT(1, &regDE.D);
		break;
	case 0x4B:
		BIT(1, &regDE.E);
		break;
	case 0x4C:
		BIT(1, &regHL.H);
		break;
	case 0x4D:
		BIT(1, &regHL.L);
		break;
	case 0x4E:
		BIT(1, &mem[regHL.HL]);
		break;
	case 0x4F:
		BIT(1, &regAF.A);
		break;
	case 0x50:
		BIT(2, &regBC.B);
		break;
	case 0x51:
		BIT(2, &regBC.C);
		break;
	case 0x52:
		BIT(2, &regDE.D);
		break;
	case 0x53:
		BIT(2, &regDE.E);
		break;
	case 0x54:
		BIT(2, &regHL.H);
		break;
	case 0x55:
		BIT(2, &regHL.L);
		break;
	case 0x56:
		BIT(2, &mem[regHL.HL]);
		break;
	case 0x57:
		BIT(2, &regAF.A);
		break;
	case 0x58:
		BIT(3, &regBC.B);
		break;
	case 0x59:
		BIT(3, &regBC.C);
		break;
	case 0x5A:
		BIT(3, &regDE.D);
		break;
	case 0x5B:
		BIT(3, &regDE.E);
		break;
	case 0x5C:
		BIT(3, &regHL.H);
		break;
	case 0x5D:
		BIT(3, &regHL.L);
		break;
	case 0x5E:
		BIT(3, &mem[regHL.HL]);
		break;
	case 0x5F:
		BIT(3, &regAF.A);
		break;
	case 0x60:
		BIT(4, &regBC.B);
		break;
	case 0x61:
		BIT(4, &regBC.C);
		break;
	case 0x62:
		BIT(4, &regDE.D);
		break;
	case 0x63:
		BIT(4, &regDE.E);
		break;
	case 0x64:
		BIT(4, &regHL.H);
		break;
	case 0x65:
		BIT(4, &regHL.L);
		break;
	case 0x66:
		BIT(4, &mem[regHL.HL]);
		break;
	case 0x67:
		BIT(4, &regAF.A);
		break;
	case 0x68:
		BIT(4, &regBC.B);
		break;
	case 0x69:
		BIT(5, &regBC.C);
		break;
	case 0x6A:
		BIT(5, &regDE.D);
		break;
	case 0x6B:
		BIT(5, &regDE.E);
		break;
	case 0x6C:
		BIT(5, &regHL.H);
		break;
	case 0x6D:
		BIT(5, &regHL.L);
		break;
	case 0x6E:
		BIT(5, &mem[regHL.HL]);
		break;
	case 0x6F:
		BIT(5, &regAF.A);
		break;
	case 0x70:
		BIT(6, &regBC.B);
		break;
	case 0x71:
		BIT(6, &regBC.C);
		break;
	case 0x72:
		BIT(6, &regDE.D);
		break;
	case 0x73:
		BIT(6, &regDE.E);
		break;
	case 0x74:
		BIT(6, &regHL.H);
		break;
	case 0x75:
		BIT(6, &regHL.L);
		break;
	case 0x76:
		BIT(6, &mem[regHL.HL]);
		break;
	case 0x77:
		BIT(7, &regAF.A);
		break;
	case 0x78:
		BIT(7, &regBC.B);
		break;
	case 0x79:
		BIT(7, &regBC.C);
		break;
	case 0x7A:
		BIT(7, &regDE.D);
		break;
	case 0x7B:
		BIT(7, &regDE.E);
		break;
	case 0x7C:
		BIT(7, &regHL.H);
		break;
	case 0x7D:
		BIT(7, &regHL.L);
		break;
	case 0x7E:
		BIT(7, &mem[regHL.HL]);
		break;
	case 0x7F:
		BIT(7, &regAF.A);
		break;
	case 0x80:
		RES(0, &regBC.B);
		break;
	case 0x81:
		RES(0, &regBC.C);
		break;
	case 0x82:
		RES(0, &regDE.D);
		break;
	case 0x83:
		RES(0, &regDE.E);
		break;
	case 0x84:
		RES(0, &regHL.H);
		break;
	case 0x85:
		RES(0, &regHL.L);
		break;
	case 0x86:
		RES(0, &mem[regHL.HL]);
		break;
	case 0x87:
		RES(1, &regAF.A);
		break;
	case 0x88:
		RES(1, &regBC.B);
		break;
	case 0x89:
		RES(1, &regBC.C);
		break;
	case 0x8A:
		RES(1, &regDE.D);
		break;
	case 0x8B:
		RES(1, &regDE.E);
		break;
	case 0x8C:
		RES(1, &regHL.H);
		break;
	case 0x8D:
		RES(1, &regHL.L);
		break;
	case 0x8E:
		RES(1, &mem[regHL.HL]);
		break;
	case 0x8F:
		RES(1, &regAF.A);
		break;
	case 0x90:
		RES(2, &regBC.B);
		break;
	case 0x91:
		RES(2, &regBC.C);
		break;
	case 0x92:
		RES(2, &regDE.D);
		break;
	case 0x93:
		RES(2, &regDE.E);
		break;
	case 0x94:
		RES(2, &regHL.H);
		break;
	case 0x95:
		RES(2, &regHL.L);
		break;
	case 0x96:
		RES(2, &mem[regHL.HL]);
		break;
	case 0x97:
		RES(2, &regAF.A);
		break;
	case 0x98:
		RES(3, &regBC.B);
		break;
	case 0x99:
		RES(3, &regBC.C);
		break;
	case 0x9A:
		RES(3, &regDE.D);
		break;
	case 0x9B:
		RES(3, &regDE.E);
		break;
	case 0x9C:
		RES(3, &regHL.H);
		break;
	case 0x9D:
		RES(3, &regHL.L);
		break;
	case 0x9E:
		RES(3, &mem[regHL.HL]);
		break;
	case 0x9F:
		RES(3, &regAF.A);
		break;
	case 0xA0:
		RES(4, &regBC.B);
		break;
	case 0xA1:
		RES(4, &regBC.C);
		break;
	case 0xA2:
		RES(4, &regDE.D);
		break;
	case 0xA3:
		RES(4, &regDE.E);
		break;
	case 0xA4:
		RES(4, &regHL.H);
		break;
	case 0xA5:
		RES(4, &regHL.L);
		break;
	case 0xA6:
		RES(4, &mem[regHL.HL]);
		break;
	case 0xA7:
		RES(4, &regAF.A);
		break;
	case 0xA8:
		RES(4, &regBC.B);
		break;
	case 0xA9:
		RES(5, &regBC.C);
		break;
	case 0xAA:
		RES(5, &regDE.D);
		break;
	case 0xAB:
		RES(5, &regDE.E);
		break;
	case 0xAC:
		RES(5, &regHL.H);
		break;
	case 0xAD:
		RES(5, &regHL.L);
		break;
	case 0xAE:
		RES(5, &mem[regHL.HL]);
		break;
	case 0xAF:
		RES(5, &regAF.A);
		break;
	case 0xB0:
		RES(6, &regBC.B);
		break;
	case 0xB1:
		RES(6, &regBC.C);
		break;
	case 0xB2:
		RES(6, &regDE.D);
		break;
	case 0xB3:
		RES(6, &regDE.E);
		break;
	case 0xB4:
		RES(6, &regHL.H);
		break;
	case 0xB5:
		RES(6, &regHL.L);
		break;
	case 0xB6:
		RES(6, &mem[regHL.HL]);
		break;
	case 0xB7:
		RES(7, &regAF.A);
		break;
	case 0xB8:
		RES(7, &regBC.B);
		break;
	case 0xB9:
		RES(7, &regBC.C);
		break;
	case 0xBA:
		RES(7, &regDE.D);
		break;
	case 0xBB:
		RES(7, &regDE.E);
		break;
	case 0xBC:
		RES(7, &regHL.H);
		break;
	case 0xBD:
		RES(7, &regHL.L);
		break;
	case 0xBE:
		RES(7, &mem[regHL.HL]);
		break;
	case 0xBF:
		RES(7, &regAF.A);
		break;
	case 0xC0:
		SET(0, &regBC.B);
		break;
	case 0xC1:
		SET(0, &regBC.C);
		break;
	case 0xC2:
		SET(0, &regDE.D);
		break;
	case 0xC3:
		SET(0, &regDE.E);
		break;
	case 0xC4:
		SET(0, &regHL.H);
		break;
	case 0xC5:
		SET(0, &regHL.L);
		break;
	case 0xC6:
		SET(0, &mem[regHL.HL]);
		break;
	case 0xC7:
		SET(1, &regAF.A);
		break;
	case 0xC8:
		SET(1, &regBC.B);
		break;
	case 0xC9:
		SET(1, &regBC.C);
		break;
	case 0xCA:
		SET(1, &regDE.D);
		break;
	case 0xCB:
		SET(1, &regDE.E);
		break;
	case 0xCC:
		SET(1, &regHL.H);
		break;
	case 0xCD:
		SET(1, &regHL.L);
		break;
	case 0xCE:
		SET(1, &mem[regHL.HL]);
		break;
	case 0xCF:
		SET(1, &regAF.A);
		break;
	case 0xD0:
		SET(2, &regBC.B);
		break;
	case 0xD1:
		SET(2, &regBC.C);
		break;
	case 0xD2:
		SET(2, &regDE.D);
		break;
	case 0xD3:
		SET(2, &regDE.E);
		break;
	case 0xD4:
		SET(2, &regHL.H);
		break;
	case 0xD5:
		SET(2, &regHL.L);
		break;
	case 0xD6:
		SET(2, &mem[regHL.HL]);
		break;
	case 0xD7:
		SET(2, &regAF.A);
		break;
	case 0xD8:
		SET(3, &regBC.B);
		break;
	case 0xD9:
		SET(3, &regBC.C);
		break;
	case 0xDA:
		SET(3, &regDE.D);
		break;
	case 0xDB:
		SET(3, &regDE.E);
		break;
	case 0xDC:
		SET(3, &regHL.H);
		break;
	case 0xDD:
		SET(3, &regHL.L);
		break;
	case 0xDE:
		SET(3, &mem[regHL.HL]);
		break;
	case 0xDF:
		SET(3, &regAF.A);
		break;
	case 0xE0:
		SET(4, &regBC.B);
		break;
	case 0xE1:
		SET(4, &regBC.C);
		break;
	case 0xE2:
		SET(4, &regDE.D);
		break;
	case 0xE3:
		SET(4, &regDE.E);
		break;
	case 0xE4:
		SET(4, &regHL.H);
		break;
	case 0xE5:
		SET(4, &regHL.L);
		break;
	case 0xE6:
		SET(4, &mem[regHL.HL]);
		break;
	case 0xE7:
		SET(4, &regAF.A);
		break;
	case 0xE8:
		SET(4, &regBC.B);
		break;
	case 0xE9:
		SET(5, &regBC.C);
		break;
	case 0xEA:
		SET(5, &regDE.D);
		break;
	case 0xEB:
		SET(5, &regDE.E);
		break;
	case 0xEC:
		SET(5, &regHL.H);
		break;
	case 0xED:
		SET(5, &regHL.L);
		break;
	case 0xEE:
		SET(5, &mem[regHL.HL]);
		break;
	case 0xEF:
		SET(5, &regAF.A);
		break;
	case 0xF0:
		SET(6, &regBC.B);
		break;
	case 0xF1:
		SET(6, &regBC.C);
		break;
	case 0xF2:
		SET(6, &regDE.D);
		break;
	case 0xF3:
		SET(6, &regDE.E);
		break;
	case 0xF4:
		SET(6, &regHL.H);
		break;
	case 0xF5:
		SET(6, &regHL.L);
		break;
	case 0xF6:
		SET(6, &mem[regHL.HL]);
		break;
	case 0xF7:
		SET(7, &regAF.A);
		break;
	case 0xF8:
		SET(7, &regBC.B);
		break;
	case 0xF9:
		SET(7, &regBC.C);
		break;
	case 0xFA:
		SET(7, &regDE.D);
		break;
	case 0xFB:
		SET(7, &regDE.E);
		break;
	case 0xFC:
		SET(7, &regHL.H);
		break;
	case 0xFD:
		SET(7, &regHL.L);
		break;
	case 0xFE:
		SET(7, &mem[regHL.HL]);
		break;
	case 0xFF:
		SET(7, &regAF.A);
		break;
	default:
		cout << "Error: Unkown operand CB " << hex << (short)operation << " at " << hex << (short)PC << endl;
		exit(1);
	}
}

void execute(unsigned char operation){

	switch (operation)
	{
	case 0x00:
		NOP();
		break;
	case 0x01:
		LDregnn(&regBC.BC);
		break;
	case 0x02:
		LDregAddress(&regAF.A, &regBC.BC);
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
		LDAddressreg(&regAF.A, &regBC.BC);
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
		cout << "0x10 STOP called" << endl;
		exit(1);
		break;
	case 0x11:
		LDregnn(&regDE.DE);
		break;
	case 0x12:
		LDregAddress(&regAF.A, &regDE.DE);
		break;
	case 0x1A:
		LDAddressreg(&regAF.A, &regDE.DE);
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
		cout << "Implement 0x27" << endl;
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
		LDregreg8bit(&regBC.B, &regBC.B);
		break;
	case 0x41:
		LDregreg8bit(&regBC.B, &regBC.C);
		break;
	case 0x42:
		LDregreg8bit(&regBC.B, &regDE.D);
		break;
	case 0x43:
		LDregreg8bit(&regBC.B, &regDE.E);
		break;
	case 0x44:
		LDregreg8bit(&regBC.B, &regHL.H);
		break;
	case 0x45:
		LDregreg8bit(&regBC.B, &regHL.L);
		break;
	case 0x46:
		LDAddressreg(&regBC.B, &regHL.HL);
		break;
	case 0x47:
		LDregreg8bit(&regBC.B, &regAF.A);
		break;
	case 0x48:
		LDregreg8bit(&regBC.C, &regBC.B);
		break;
	case 0x49:
		LDregreg8bit(&regBC.C, &regBC.C);
		break;
	case 0x4A:
		LDregreg8bit(&regBC.C, &regDE.D);
		break;
	case 0x4B:
		LDregreg8bit(&regBC.C, &regDE.E);
		break;
	case 0x4C:
		LDregreg8bit(&regBC.C, &regHL.H);
		break;
	case 0x4D:
		LDregreg8bit(&regBC.C, &regHL.L);
		break;
	case 0x4E:
		LDAddressreg(&regBC.B, &regHL.HL);
		break;
	case 0x4F:
		LDregreg8bit(&regBC.C, &regAF.A);
		break;
	case 0x50:
		LDregreg8bit(&regDE.D, &regBC.B);
		break;
	case 0x51:
		LDregreg8bit(&regDE.D, &regBC.C);
		break;
	case 0x52:
		LDregreg8bit(&regDE.D, &regDE.D);
		break;
	case 0x53:
		LDregreg8bit(&regDE.D, &regDE.E);
		break;
	case 0x54:
		LDregreg8bit(&regDE.D, &regHL.H);
		break;
	case 0x55:
		LDregreg8bit(&regDE.D, &regHL.L);
		break;
	case 0x56:
		LDAddressreg(&regDE.D, &regHL.HL);
		break;
	case 0x57:
		LDregreg8bit(&regDE.D, &regAF.A);
		break;
	case 0x58:
		LDregreg8bit(&regDE.E, &regBC.B);
		break;
	case 0x59:
		LDregreg8bit(&regDE.E, &regBC.C);
		break;
	case 0x5A:
		LDregreg8bit(&regDE.E, &regDE.D);
		break;
	case 0x5B:
		LDregreg8bit(&regDE.E, &regDE.E);
		break;
	case 0x5C:
		LDregreg8bit(&regDE.E, &regHL.H);
		break;
	case 0x5D:
		LDregreg8bit(&regDE.E, &regHL.L);
		break;
	case 0x5E:
		LDAddressreg(&regDE.E, &regHL.HL);
		break;
	case 0x5F:
		LDregreg8bit(&regDE.E, &regAF.A);
		break;
	case 0x60:
		LDregreg8bit(&regHL.H, &regBC.B);
		break;
	case 0x61:
		LDregreg8bit(&regHL.H, &regBC.C);
		break;
	case 0x62:
		LDregreg8bit(&regHL.H, &regDE.D);
		break;
	case 0x63:
		LDregreg8bit(&regHL.H, &regDE.E);
		break;
	case 0x64:
		LDregreg8bit(&regHL.H, &regHL.H);
		break;
	case 0x65:
		LDregreg8bit(&regHL.H, &regHL.L);
		break;
	case 0x66:
		LDAddressreg(&regHL.H, &regHL.HL);
		break;
	case 0x67:
		LDregreg8bit(&regHL.H, &regAF.A);
		break;
	case 0x68:
		LDregreg8bit(&regHL.L, &regBC.B);
		break;
	case 0x69:
		LDregreg8bit(&regHL.L, &regBC.C);
		break;
	case 0x6A:
		LDregreg8bit(&regHL.L, &regDE.D);
		break;
	case 0x6B:
		LDregreg8bit(&regHL.L, &regDE.E);
		break;
	case 0x6C:
		LDregreg8bit(&regHL.L, &regHL.H);
		break;
	case 0x6D:
		LDregreg8bit(&regHL.L, &regHL.L);
		break;
	case 0x6E:
		LDAddressreg(&regHL.L, &regHL.HL);
		break;
	case 0x6F:
		LDregreg8bit(&regHL.L, &regAF.A);
		break;
	case 0x70:
		LDregAddress(&regHL.HL, &regBC.B);
		break;
	case 0x71:
		LDregAddress(&regHL.HL, &regBC.C);
		break;
	case 0x72:
		LDregAddress(&regHL.HL, &regDE.D);
		break;
	case 0x73:
		LDregAddress(&regHL.HL, &regDE.E);
		break;
	case 0x74:
		LDregAddress(&regHL.HL, &regHL.H);
		break;
	case 0x75:
		LDregAddress(&regHL.HL, &regHL.L);
		break;
	case 0x76:
		cout << "implement HALT 0x76" << endl;
		exit(1);
		break;
	case 0x77:
		LDregAddress(&regAF.A, &regHL.HL);
		break;
	case 0x78:
		LDregreg8bit(&regAF.A, &regBC.B);
		break;
	case 0x79:
		LDregreg8bit(&regAF.A, &regBC.C);
		break;
	case 0x7A:
		LDregreg8bit(&regAF.A, &regDE.D);
		break;
	case 0x7B:
		LDregreg8bit(&regAF.A, &regDE.E);
		break;
	case 0x7C:
		LDregreg8bit(&regAF.A, &regHL.H);
		break;
	case 0x7D:
		LDregreg8bit(&regAF.A, &regHL.L);
		break;
	case 0x7E:
		LDAddressreg(&regAF.A, &regHL.HL);
		break;
	case 0x7F:
		LDregreg8bit(&regAF.A, &regAF.A);
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
		LDregreg16bit(&SP, &regHL.HL);
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
