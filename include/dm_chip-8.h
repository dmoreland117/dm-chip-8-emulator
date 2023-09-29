#pragma once

#include <iostream>

#include "types.h"

#define ROM_LOAD_ADDR 0x200
#define RAM_SIZE  4096
#define FRAME_BUFFER_SIZE 2048
#define FONTSET_SIZE 80

const unsigned char fontset[FONTSET_SIZE] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0,		// 0
	0x20, 0x60, 0x20, 0x20, 0x70,		// 1
	0xF0, 0x10, 0xF0, 0x80, 0xF0,		// 2
	0xF0, 0x10, 0xF0, 0x10, 0xF0,		// 3
	0x90, 0x90, 0xF0, 0x10, 0x10,		// 4
	0xF0, 0x80, 0xF0, 0x10, 0xF0,		// 5
	0xF0, 0x80, 0xF0, 0x90, 0xF0,		// 6
	0xF0, 0x10, 0x20, 0x40, 0x40,		// 7
	0xF0, 0x90, 0xF0, 0x90, 0xF0,		// 8
	0xF0, 0x90, 0xF0, 0x10, 0xF0,		// 9
	0xF0, 0x90, 0xF0, 0x90, 0x90,		// A
	0xE0, 0x90, 0xE0, 0x90, 0xE0,		// B
	0xF0, 0x80, 0x80, 0x80, 0xF0,		// C
	0xE0, 0x90, 0x90, 0x90, 0xE0,		// D
	0xF0, 0x80, 0xF0, 0x80, 0xF0,		// E
	0xF0, 0x80, 0xF0, 0x80, 0x80		// F
};


struct Chip_8_Registers {
    Byte v[16]; // VF (v[16]) is for flags set by some instructions.
    Word i; // Used for storing memory addresses.

    Byte sp; // Stack pointer.
    Word pc; // Program counter.
    
    // when not 0x00 auto decrements at 60hz.
    Byte delay; 
    Byte sound_timer;
};

class DMChip_8 {
public:
    DMChip_8();

public: //cpu functions.
    void reset();
    void clock();

public: // framebuffer functions
    inline Byte* getFrameBuffer() { return &frameBuffer[0]; }

    inline void  framebufferSetPixel(int x, int y)   { frameBuffer[x + (64 * y)] = 1; }
    inline void  framebufferClearPixel(int x, int y) { frameBuffer[x * y] = 0; }

    void frameBufferClear();

public: // rom functions
    bool loadRom(const char* path);

public: // ram functions
    inline Byte ramReadByte  (Word addr) { return ram[addr]; }
    inline Word ramReadWord  (Word addr) 
    {
        Word ret = ram[addr];
        ret <<= 8;
        ret |= ram[addr+1];

        return ret;
    }
    inline Byte ramWrite (Word addr, Byte val) { ram[addr] = val; }

    void ramClear();

private:
    void incPc(int i);
    bool pushPcToStack();
    Word popFromStack();

    Word getOpcodeAddr(Word &opcode);
    Byte getOpcodeVx(Word &opcode);
    Byte getOpcodeVy(Word &opcode);
    Byte getOpcodeByte(Word &opcode);
    Byte getOpcodeNibble(Word &opcode);

private:
    Chip_8_Registers registers;

    // Usally you would make a buss class and ram would be in there but this emulator is very simple,
    Byte ram[RAM_SIZE];

    // chip-8 stack and frameBuffer are seperate from ram.
    Word stack[16]; 
    Byte frameBuffer[64 * 32];
};