#ifndef CHIP8_H
#define CHIP8_H

#define CODE_SIZE 0xDFF

class Chip8
{
private:
	// PSUEDO-REGISTERS
	unsigned short mPc;
	unsigned char mSp;
	
	// REGISTERS
	unsigned char mV[16]; // V[0xF] would be a flag
	unsigned short mI;
	unsigned char mDelayTimer;
	unsigned char mSoundTimer;
	
	// EXTRAS
	unsigned char mMemory[0xFFF];
	unsigned short mStack[16];
	unsigned char mKeys[16];
	SDL_Surface* mSurface;
	
	// instruction variables
	#define OP(instruction) ((instruction & 0xF000) >> 12)
	#define X(instruction) ((instruction & 0x0F00) >> 8)
	#define Y(instruction) ((instruction & 0x00F0) >> 4)
	#define KK(instruction) (instruction & 0x00FF)
	#define NNN(instruction) (instruction & 0x0FFF)
	
public:
	Chip8(SDL_Surface* surface);
	int loadFile(const char* file);
	unsigned int emulateCycle();
	void clearScreen();
	
};

#endif // CHIP8_H