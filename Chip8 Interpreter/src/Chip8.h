#ifndef CHIP8_H
#define CHIP8_H

#define CODE_SIZE 0xDFF
#include <SDL/SDL.h>

// Window size (128, 64)is multiplied by the Ratio

#define RATIO 3
#define WINDOW_WIDTH 128 * RATIO
#define WINDOW_HEIGHT 64 * RATIO
#define WINDOW_FLAGS 0

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
	unsigned char mMemory[0xFFF] = {};
	unsigned short mStack[16];
	unsigned char mKeys[16];
	unsigned char mLevel[WINDOW_WIDTH / RATIO][WINDOW_HEIGHT / RATIO];
	SDL_Surface* mSurface;
	void clearScreen();
	void drawSprite(short instruction); // Transform drawing into the level array
	void renderLevel();
	
	// instruction variables
	#define OP(instruction) ((instruction & 0xF000) >> 12)
	#define X(instruction) ((instruction & 0x0F00) >> 8)
	#define Y(instruction) ((instruction & 0x00F0) >> 4)
	#define NNN(instruction) (instruction & 0x0FFF)
	#define KK(instruction) (instruction & 0x00FF)
	#define N(instruction) (instruction & 0x000F)
	
public:
	Chip8(SDL_Surface* surface);
	int loadFile(const char* file);
	unsigned int emulateCycle();
	
};

#endif // CHIP8_H