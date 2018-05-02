#include <SDL\SDL.h>

#include "Chip8.h"

Chip8::Chip8(SDL_Surface* surface)
{
	mPc = 0x200;
	mSp = 0;
	mSurface = surface;
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
					// SDL_Log("Screen cleared");
					break;
				case 0xEE: // Return from a subroutine 00EE
					mPc = mStack[mSp];
					mSp -= 1;
					// SDL_Log("Returned");
					break;
			}
			break;
		case 0x1: //  1nnn - JP addr
			mPc = NNN(instruction);
			// SDL_Log("Jumped to %x", instruction & 0x0FFF);
			break;
		case 0x2: // 2nnn - CALL addr
			++mSp;
			mStack[mSp] = mPc;
			mPc = NNN(instruction);
			break;
		case 0x3: // 3xkk - SE Vx, byte
			if (mV[X(instruction)] == KK(instruction))
				mPc += 2;
			break;
		case 0x4: // 4xkk - SNE Vx, byte
			if (mV[X(instruction)] != KK(instruction))
				mPc += 2;
			break;
		case 0x5: // 5xy0 - SE Vx, Vy
			if (mV[X(instruction)] == mV[Y(instruction)])
				mPc += 2;
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
		case 0x9:
			break;
		case 0xA:
			break;
		case 0xB:
			break;
		case 0xC:
			break;
		case 0xD:
			break;
		case 0xE:
			break;
		case 0xF:
			break;
	}
	mPc += 2;
	return 0;
}

void Chip8::clearScreen()
{
	Uint32 black = SDL_MapRGB(mSurface->format, 0, 0, 0);
	SDL_FillRect(mSurface, 0, black);
}