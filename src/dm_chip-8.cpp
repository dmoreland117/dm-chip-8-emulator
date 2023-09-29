#include "dm_chip-8.h"

#include <bitset>
#include <fstream>


DMChip_8::DMChip_8()
{
    reset();
}

void DMChip_8::reset()
{
    registers.pc = 0x200;

    ramClear();
    frameBufferClear();
}

void DMChip_8::clock()
{
    std::cout << "PC: " << std::hex << (int)registers.pc << std::endl;

    // get first byte of opcode at pc
    Word opCode = ramReadWord(registers.pc);
    std::cout << std::hex << opCode << std::endl;

    switch (opCode & 0xF000)
    {
    case 0x0000:
    {
        if ((opCode & 0x000f) == 0x00)
        {
            std::cout << "Clear screen" << std::endl;
            frameBufferClear();
        }
        break;
    }

    case 0x1000:
    {
        Word addr = getOpcodeAddr(opCode);
        std::cout << "jmp addr: " << std::hex << addr << std::endl;
        registers.pc = addr;
        break;
    }

    case 0x2000:
    {
        Word addr = getOpcodeAddr(opCode);
        std::cout << "call addr: " << std::hex << addr << std::endl;
        pushPcToStack();
        registers.pc = addr;
        break;
    }

    case 0x3000:
    {
        Byte vx = getOpcodeVx(opCode);
        Byte kk = getOpcodeByte(opCode);
        std::cout << "if v " << std::hex << (int)vx << " != " << std::hex << (int)kk << std::endl;

        if (registers.v[vx] == kk)
            incPc(2);
            std::cout << "True";
        break;
    }

    case 0x4000:
    {
        Byte vx = getOpcodeVx(opCode);
        Byte kk = getOpcodeByte(opCode);
        std::cout << "if v " << std::hex << (int)vx << " != " << std::hex << (int)kk << std::endl;

        if (registers.v[vx] != kk)
            incPc(2);
        break;
    }

    case 0x5000:
    {
        Byte vx = getOpcodeVx(opCode);
        Byte vy = getOpcodeVy(opCode);

        if (registers.v[vx] == registers.v[vy])
            incPc(2);
        break;
    }

    case 0x6000:
    {
        Byte vx = getOpcodeVx(opCode);
        Byte kk = getOpcodeByte(opCode);

        std::cout << "setting register v[" << std::hex << (int)vx << "] = ";
        std::cout << std::hex << (int)kk << std::endl;

        registers.v[vx] = kk;
        break;
    }

    case 0x7000:
    {
        Byte vx = getOpcodeVx(opCode);
        Byte kk = getOpcodeByte(opCode);
        std::cout << "register v[" << std::hex << (int)vx << "] +=";
        std::cout << std::hex << (int)kk << std::endl;

        registers.v[vx] += kk;
        break;
    }

    // math instructions.
    case 0x8000:
    {
        switch (getOpcodeNibble(opCode))
        {
        case 0x00:
        {
            Byte vx = getOpcodeVx(opCode);
            Byte vy = getOpcodeVy(opCode);
            std::cout << "setting v[" << std::hex << (int)vx << "]" << " = ";
            std::cout << "v[" << std::hex << (int)vy << "]" << std::endl;

            registers.v[vx] = registers.v[vy];
            break;
        }

        case 0x01:
        {
            Byte vx = getOpcodeVx(opCode);
            Byte vy = getOpcodeVy(opCode);
            std::cout << "OR v[" << std::hex << (int)vx << "]" << " with ";
            std::cout << "v[" << std::hex << (int)vy << "]" << std::endl;

            registers.v[vx] |= registers.v[vy];
            break;
        }

        case 0x02:
        {
            Byte vx = getOpcodeVx(opCode);
            Byte vy = getOpcodeVy(opCode);
            std::cout << "AND v[" << std::hex << (int)vx << "]" << " with ";
            std::cout << "v[" << std::hex << (int)vy << "]" << std::endl;

            registers.v[vx] &= registers.v[vy];
            break;
        }

        case 0x03:
        {
            Byte vx = getOpcodeVx(opCode);
            Byte vy = getOpcodeVy(opCode);
            std::cout << "XOR v[" << std::hex << (int)vx << "]" << " with ";
            std::cout << "v[" << std::hex << (int)vy << "]" << std::endl;

            registers.v[vx] ^= registers.v[vy];
            break;
        }

        case 0x04:
        {
            Byte vx = getOpcodeVx(opCode);
            Byte vy = getOpcodeVy(opCode);
            Word added;
            std::cout << "ADD set carry v[" << std::hex << (int)vx << "]" << " + ";
            std::cout << "v[" << std::hex << (int)vy << "]" << std::endl;

            added = registers.v[vx] + registers.v[vy];
            if (added > 255) registers.v[0xf] = 1;
            else registers.v[0xf] = 0;
            registers.v[vx] = added & 0x00ff;
            break;
        }

        case 0x05:
        {
            Byte vx = getOpcodeVx(opCode);
            Byte vy = getOpcodeVy(opCode);
            std::cout << "Subtract v[" << std::hex << (int)vx << "]" << " - ";
            std::cout << "v[" << std::hex << (int)vy << "]" << std::endl;


            if (registers.v[vx] > registers.v[vy]) registers.v[0xf] = 1;
            else registers.v[0xf] = 0;

            registers.v[vx] -= registers.v[vy];
            break;
        }

        case 0x06:
        {
            std::cout << "SHR vx with vy carry." << std::endl;
            Byte vx = getOpcodeVx(opCode);
            Byte vy = getOpcodeVy(opCode);

            if ((registers.v[vx] & 0x01) == 1)
                registers.v[0xf] = 1;
            else
                registers.v[0xf] = 0;

            registers.v[vx] /= 2;
            break;
        }

        case 0x07:
        {
            std::cout << "Subtract vy from vx set carry." << std::endl;
            Byte vx = getOpcodeVx(opCode);
            Byte vy = getOpcodeVy(opCode);

            if (registers.v[vy] > registers.v[vx]) registers.v[0xf] = 1;
            else registers.v[0xf] = 0;

            registers.v[vy] -= registers.v[vx];
            break;
        }

        case 0x0E:
        {
            std::cout << "SHL vx with vy carry." << std::endl;
            Byte vx = getOpcodeVx(opCode);
            Byte vy = getOpcodeVy(opCode);

            if ((registers.v[vx] & 0x10) == 1)
                registers.v[0xf] = 1;
            else
                registers.v[0xf] = 0;

            registers.v[vx] *= 2;
            break;
        }

        default:
            break;
        }
    }

    case 0x9000:
    {
        Byte vx = getOpcodeVx(opCode);
        Byte vy = getOpcodeVy(opCode);

        if (registers.v[vx] != registers.v[vy])
            incPc(2);
        break;
    }

    case 0xA000:
    {
        std::cout << "set i" << std::endl;
        registers.i = opCode & 0x0fff;
        break;
    }

    case 0xB000:
    {
        Word nnn = getOpcodeAddr(opCode);

        registers.pc = nnn + registers.v[0];

        break;
    }

    case 0xC000:
    {
        std::cout << "randome numbers not implemented yet." << std::endl;
        break;
    }

    case 0xD000:
    {
        Byte n = getOpcodeNibble(opCode);
        Word addr = registers.i;
        Byte vx = getOpcodeVx(opCode);
        Byte vy = getOpcodeVy(opCode);

        for (size_t i = 0; i < n; i++)
        {
            for (size_t j = 0; j < 8; j++)
            {
                Byte currentBit = (ram[addr + i] << j);
                currentBit &= 0x80;
                if (currentBit)
                {
                    framebufferSetPixel(registers.v[vx] + j, registers.v[vy] + i);
                }
                else
                {
                    //framebufferClearPixel(vx + j, vy + i);
                }
                
            }
            
        }
        

        break;
    }

    default:
    {
        std::cout << "Warning: instruction not implemented yet." << std::endl;
        break;
    }
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
    file.read((char *)(&ram[ROM_LOAD_ADDR]), fileSize);

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

void DMChip_8::incPc(int i = NULL)
{
    if (!i)
    {
        registers.pc += 2;
    }
    registers.pc += i;
}

bool DMChip_8::pushPcToStack()
{
    if (registers.sp >= 0x16)
    {
        return false;
    }
    stack[registers.sp] = registers.pc;
    registers.sp++;
}

Word DMChip_8::popFromStack()
{
    Word ret = stack[registers.sp];
    registers.sp--;
    return ret;
}

Word DMChip_8::getOpcodeAddr(Word &opcode)
{
    return opcode & 0x0fff;
}

Byte DMChip_8::getOpcodeVx(Word &opcode)
{
    return opcode >> 8 & 0x0f;
}

Byte DMChip_8::getOpcodeVy(Word &opcode)
{
    return opcode >> 4;
}

Byte DMChip_8::getOpcodeByte(Word &opcode)
{
    return opcode & 0x00ff;
}

Byte DMChip_8::getOpcodeNibble(Word &opcode)
{
    return opcode & 0x000f;
}
