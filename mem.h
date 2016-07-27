#ifndef MEM_H

#define MEM_H
struct RegAF{
	union{
		struct{
			unsigned char F;
			unsigned char A;
		};
		unsigned short AF;
	};
};

struct RegBC{
	union{
		struct{
			unsigned char C;
			unsigned char B;
		};
		unsigned short BC;
	};
};


struct RegDE{
	union{
		struct{
			unsigned char E;
			unsigned char D;
		};
		unsigned short DE;
	};
};


struct RegHL{
	union{
		struct{
			unsigned char L;
			unsigned char H;
		};
		unsigned short HL;
	};
};

extern unsigned short PC;
extern unsigned short SP;

extern unsigned char data[10000];
extern unsigned char mem[65536];

extern short cycles;

#endif