#include <SDL\SDL.h>
#include <time.h>
#include <stdlib.h>
#include "Chip8.h"

Chip8::Chip8(SDL_Surface* surface)
{
	mPc = 0x200;
	mSp = 0;
	mSurface = surface;
	srand(time(0));
}

// If 0 is returned, then everything went OK
// If -1 is returned, then the file was not found
int Chip8::loadFile(const char* filePath)
{
	SDL_RWops* file = SDL_RWFromFile(filePath, "rb");
	if (file == NULL)
		return -1;
	unsigned char code[CODE_SIZE] = {0};
	SDL_RWread(file, code, CODE_SIZE, 1);
	SDL_RWclose(file);
	
	for (int x = 0; x < CODE_SIZE; x++)
		mMemory[mPc + x] = code[x]; // Copy code to the memory
	
	return 0;
}

unsigned int Chip8::emulateCycle()
{
	if (mPc == 0xFFE)
		return -1; // Max length for code has been reached, end of program

	short instruction = (mMemory[mPc] << 8) | (mMemory[mPc + 1]);
	switch(OP(instruction))
	{
		case 0x0:
			switch (instruction) // SYS addr is not supported
			{
				case 0xE0: // Clear the display 00E0
					clearScreen();
					break;
				case 0xEE: // Return from a subroutine 00EE
					mPc = mStack[mSp];
					mSp -= 1;
					break;
			}
			break;
		case 0x1: //  1nnn - JP addr
			mPc = NNN(instruction);
			break;
		case 0x2: // 2nnn - CALL addr
			++mSp;
			mStack[mSp] = mPc;
			mPc = NNN(instruction);
			break;
		case 0x3: // 3xkk - SE Vx, byte
			mPc += (mV[X(instruction)] == KK(instruction)) ? 2 : 0;
			break;
		case 0x4: // 4xkk - SNE Vx, byte
			mPc += (mV[X(instruction)] != KK(instruction)) ? 2 : 0;
			break;
		case 0x5: // 5xy0 - SE Vx, Vy
			mPc += (mV[X(instruction)] == mV[Y(instruction)]) ? 2 : 0;
			break;
		case 0x6: // 6xkk - LD Vx, byte
			mV[X(instruction)] = KK(instruction);
			break;
		case 0x7: // ADD Vx, byte
			mV[X(instruction)] += KK(instruction);
			break;
		case 0x8:
			switch(N(instruction))
			{
				case 0: // 8xy0 - LD Vx, Vy
					mV[X(instruction)] = mV[Y(instruction)];
					break;
				case 1: // 8xy1 - OR Vx, Vy
					mV[X(instruction)] = mV[X(instruction)] | mV[Y(instruction)];
					break;
				case 2: // 8xy2 - AND Vx, Vy
					mV[X(instruction)] = mV[X(instruction)] & mV[Y(instruction)];
					break;
				case 3: // 8xy3 - XOR Vx, Vy
					mV[X(instruction)] = mV[X(instruction)] ^ mV[Y(instruction)];
					break;
				case 4: // 8xy4 - ADD Vx, Vy
					mV[0xF] = (mV[X(instruction)] + mV[Y(instruction)] > 0xFF);
					mV[X(instruction)] = (mV[X(instruction)] + mV[Y(instruction)]) & 0xFF;
					break;
				case 5: // 8xy5 - SUB Vx, Vy
					mV[0xF] = (mV[X(instruction)] > mV[Y(instruction)]);
					mV[X(instruction)] -= mV[Y(instruction)];
					break;
				case 6: // 8xy6 - SHR Vx {, Vy}
					mV[0xF] = N(mV[X(instruction)]);
					mV[X(instruction)] /= 2;
					break;
				case 7: // 8xy7 - SUBN Vx, Vy
					mV[0xF] = (mV[Y(instruction)] > mV[X(instruction)]);
					mV[X(instruction)] = mV[Y(instruction)] - mV[X(instruction)];
					break;
				case 0xE: // 8xyE - SHL Vx {, Vy}
					mV[0xF] = OP(mV[X(instruction)]);
					mV[X(instruction)] *= 2;
					break;
			}
			break;
		case 0x9: // 9xy0 - SNE Vx, Vy
			mPc += (mV[X(instruction)] != mV[Y(instruction)]) ? 2 : 0;
			break;
		case 0xA: // Annn - LD I, addr
			mI = NNN(instruction);
			break;
		case 0xB: // Bnnn - JP V0, addr
			mPc = NNN(instruction) + mV[0];
			break;
		case 0xC: // Cxkk - RND Vx, byte
			mV[X(instruction)] = (rand() % 0xFF + 1) & KK(instruction);
			break;
		case 0xD: // Dxyn - DRW Vx, Vy, nibble
			makeLevel(instruction);
			break;
		case 0xE:
			break;
		case 0xF:
			break;
		default:
			SDL_Log("Instruction is non-existant");
	}
	mPc += 2;
	return 0;
}

// Clear the screen
void Chip8::clearScreen() 
{
	mLevel = {0};
	Uint32 black = SDL_MapRGB(mSurface->format, 0, 0, 0);
	SDL_FillRect(mSurface, 0, black);
}

// Size of a sprite: 8xbytes (WidthxHeight)
void Chip8::drawSprite(short instruction)
{
	char rows = N(instruction);
	char xpos = X(instruction);
	int ypos = Y(instruction);
	mV[0xF] = 0;
	for (char row = 0; row < rows; row++) // Cycle through each row
	{
		mV[0xF] = mLevel[xpos][ypos] ? 1 : 0;
		mLevel[xpos][ypos] = mMemory[mI + row * 8] ? 1 : 0;
	}
}

void Chip8::renderLevel()
{
	
}