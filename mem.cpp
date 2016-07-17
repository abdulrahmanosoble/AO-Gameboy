static union {
	struct{
		unsigned char F;
		unsigned char A;
	};
	unsigned short AF;
};

static union {
	struct{
		unsigned char C;
		unsigned char B;
	};
	unsigned short BC;
};

static union {
	struct{
		unsigned char E;
		unsigned char D;
	};
	unsigned short DE;
};

static union {
	struct{
		unsigned char L;
		unsigned char H;
	};
	unsigned short HL;
};

unsigned short PC;
unsigned short SP;

unsigned char data[1000000];
unsigned char mem[65536];
