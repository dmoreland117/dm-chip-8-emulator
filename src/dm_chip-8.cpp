#include "dm_chip-8.h"

#include <iostream>
#include <bitset>
#include <fstream>

DMChip_8::DMChip_8() {
        for (size_t i = 0; i < 64 * 32; i++)
        {
            frameBuffer[i] = 0;
        }
        
    }

void DMChip_8::reset()
{
    registers.pc = 0x200;

    ramClear();
    frameBufferClear();
}

void DMChip_8::clock()
{
    std::cout << "PC: " << std::hex << registers.pc << std::endl;
    
    // get first byte of opcode at pc
    Word opCode = ramReadWord(registers.pc);
    
    // debuf stuff
    std::bitset<16> x(opCode);
    std::cout << x << std::endl;
    std::cout << std::hex << opCode << std::endl;


    switch (opCode & 0xF000) 
    {
    case 0x0000:
        
        if ((opCode & 0x000f) == 0x00)
        {
            std::cout << "Clear screen" << std::endl;
            frameBufferClear();
        } 
        break;

    case 0xA000:
        std::cout << "set i" << std::endl;
        registers.i = opCode & 0x0fff;
        break;

    case 0x6000:
        std::cout << "set a v register." << std::endl;
        registers.v[opCode & 0x0f] = opCode &0x00ff;
        break;

    default:
        std::cout << "Warning: instruction not implemented yet." << std::endl;
        break;
    }


    // increment pc
    registers.pc += 2;
}

void DMChip_8::frameBufferClear()
{
    std::cout << "Clearing screen." << std::endl;

    for (size_t i = 0; i < FRAME_BUFFER_SIZE; i++)
    {
        frameBuffer[i] = 0;
    }
}

bool DMChip_8::loadRom(const char *path)
{
    // set up the file object and check for errors.
    std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);
    if (!file.is_open()) 
    {
        return false;
    }

    // get the file size and check for errors.
    std::ifstream::pos_type fileSize = file.tellg();
    if (fileSize < 0)
    {
        return false;
    }

    // if no errors seek to beginning of file and read file to ram[0x200]
    file.seekg(0, std::ios::beg);
    file.read((char*)(&ram[ROM_LOAD_ADDR]), fileSize);

    for (Word i = 0; i <= 4096; i += 16)
    {
       std::cout << i << ": ";
       for (size_t j = 0; j < 16; j++)
       {
        std::cout << (int)ramReadByte(i + j);
        std::cout << " ";
       }
       std::cout << std::endl;
       
    }


    return true;
}

void DMChip_8::ramClear()
{
    for (size_t i = 0; i < RAM_SIZE; i++)
    {
        ram[i] = 0;
    }

    // load the fontset.
    for (size_t i = 0; i < FONTSET_SIZE; i++)
    {
        ram[i] = fontset[i];
    }
    
}
