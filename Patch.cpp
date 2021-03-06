#include "Patch.h"
#include <Windows.h>
#include "Dumper.h"
#include "WowPointers.h"
#include "Memory.h"
#include <fstream>

//#define _BUILD_ 15595
//#define _BUILD_ 16357
#define _BUILD_ 16135

uint32 HandlerAddress;
uint32 opcode, _this;

#if _BUILD_ == 15595
bool AuthCheck(uint32 opcode)
{
    return (opcode & 0xB3FD) == 320;
}

bool SpecialCheck(uint32 opcode)
{
    return (opcode & 0x92E8) == 4256;
}

bool NormalCheck(uint32 opcode)
{
    return (opcode & 0x90CC) == 4;
}

// 4.3.4 15595
void HandlePacket()
{
    std::ofstream fs("packets.log", std::ios::out);

    fs << "This: " << std::hex << _this << std::endl;

    for (uint32 i = 0; i < 0xFFFF; ++i)
    {
        if (AuthCheck(i) || SpecialCheck(i) || !NormalCheck(i))
            continue;

        opcode = i;

        uint32 condensed = opcode & 3 | ((opcode & 0x30 | ((opcode & 0xF00 | (opcode >> 1) & 0x3000) >> 2)) >> 2);
        uint32 jamOffs, handler = 0;
        if ((jamOffs = *(uint32*)(_this + 4 * condensed + 1376)) != 0)
            handler = jamOffs - ((opcode | (opcode << 16)) ^ 0x62A3A31D);
        // CMSG?
        //else
        //    fs << "Opcode " << std::hex << opcode << " does not fit rule. Jam: " << jamOffs << " condensed: " << condensed << std::endl;

        if (handler)
            fs << std::hex << "Opcode: " << opcode << " jamOffs: " << jamOffs << " handler: " << handler << " handler2: " << *(uint32*)(_this + 4 * condensed + 9568) << std::endl;
    }

    fs.close();
}

// 4.3.4 15595
void __declspec(naked) PacketHook()
{
    __asm
    {
        pushad
        mov opcode, esi
        mov _this, edi
    }

    HandlePacket();

    __asm
    {
        popad
        cmp ecx, 10A0h
        jmp HandlerAddress
    }
}

// 4.3.4 15595
void HookHandler()
{
    uint8 JumpHook[] = {0xE9, 0x0, 0x0, 0x0, 0x0, 0x90};
    *reinterpret_cast<unsigned int*>(&JumpHook[1]) = reinterpret_cast<unsigned int>(PacketHook) - (WoWBase + NetClient__ProcessMessage_15595 + 0x2A + 0x5);
    Write(WoWBase + NetClient__ProcessMessage_15595 + 0x2A, JumpHook, 5);
    HandlerAddress = WoWBase + NetClient__ProcessMessage_15595 + 0x30;
}

#endif

#if _BUILD_ == 16357

bool AuthCheck(uint32 opcode)
{
    return (opcode & 0x97A) == 2090;
}

bool SpecialCheck(uint32 opcode)
{
    return (opcode & 0x12) == 16;
}

bool NormalCheck(uint32 opcode)
{
    return (opcode & 0x12) == 0;
}

// 5.1.0 16357
void HandlePacket()
{
    std::ofstream fs("packets_16357.log", std::ios::out);

    fs << "This: " << std::hex << _this << " wowbase" << WoWBase << " op:" << opcode << std::endl;

    for (uint32 i = 0; i < 0xFFFF; ++i)
    {
        if (AuthCheck(i) || SpecialCheck(i) || !NormalCheck(i))
            continue;

        opcode = i;

        uint32 condensed = opcode & 1 | ((opcode & 0xC | (opcode >> 1) & 0x7FF0) >> 1);
        uint32 jamOffs;
        if ((jamOffs = *(uint32*)(_this + condensed * 4 + 340 * 4)) != 0)
            if (uint32 handler = jamOffs - ((opcode | (opcode << 16)) ^ 0x62A3A31D))
                fs << std::hex << "Opcode: " << opcode << " jamOffs: " << jamOffs << " handler: " << handler << " handler2: " << *(uint32*)(_this + condensed + 2388) << std::endl;
    }

    fs.close();
}

void __declspec(naked) PacketHook()
{
    __asm
    {
        pushad
        mov opcode, esi
        mov _this, edi
    }

    HandlePacket();

    __asm
    {
        popad
        mov ebx, esi
        and ebx, 12h
        //cmp ebx, 10h
        jmp HandlerAddress
    }
}

// 5.1.0 16357
void HookHandler()
{
    uint8 JumpHook[] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };
    *reinterpret_cast<unsigned int*>(&JumpHook[1]) = reinterpret_cast<unsigned int>(PacketHook) - (WoWBase + NetClient__ProcessMessage_16357 + 0x1E + 0x5);
    Write(WoWBase + NetClient__ProcessMessage_16357 + 0x1E, JumpHook, 5);
    //HandlerAddress = WoWBase + NetClient__ProcessMessage_16357 + 0x26;
    HandlerAddress = WoWBase + NetClient__ProcessMessage_16357 + 0x23;
}

#endif

#if _BUILD_ == 16135

bool AuthCheck(uint32 opcode)
{
    return (opcode & 0xCBA) == 1058;
}

bool SpecialCheck(uint32 opcode)
{
    return (opcode & 0xA00) == 2560;
}

bool NormalCheck(uint32 opcode)
{
    return (opcode & 0xA00) == 2048;
}

// 5.0.5 16135
void HandlePacket()
{
    std::ofstream fs("packets_16135.log", std::ios::out);

    fs << "This: " << std::hex << _this << " wowbase" << WoWBase << " op:" << opcode << std::endl;

    for (uint32 i = 0; i < 0xFFFF; ++i)
    {
        if (AuthCheck(i) || SpecialCheck(i) || !NormalCheck(i))
            continue;

        opcode = i;

        uint32 condensed = opcode & 0x1FF | ((opcode & 0x400 | (opcode >> 1) & 0x7800) >> 1);
        uint32 jamOffs;
        if ((jamOffs = *(uint32*)(_this + condensed * 4 + 340 * 4)) != 0)
            if (uint32 handler = jamOffs - ((opcode | (opcode << 16)) ^ 0x62A3A31D))
                fs << std::hex << "Opcode: " << opcode << " jamOffs: " << jamOffs << " handler: " << handler << " handler2: " << *(uint32*)(_this + condensed + 2388) << std::endl;
    }

    fs.close();
}

void __declspec(naked) PacketHook()
{
    __asm
    {
        pushad
        mov opcode, esi
        mov _this, edi
    }

    HandlePacket();

    __asm
    {
        popad
        mov ebx, esi
        and ebx, 0A00h
        //cmp ebx, 0A00h
        jmp HandlerAddress
    }
}

// 5.0.5 16135
void HookHandler()
{
    uint8 JumpHook[] = { 0xE9, 0x0, 0x0, 0x0, 0x0 };
    *reinterpret_cast<unsigned int*>(&JumpHook[1]) = reinterpret_cast<unsigned int>(PacketHook) - (WoWBase + NetClient__ProcessMessage_16135 + 0x1E + 0x5);
    Write(WoWBase + NetClient__ProcessMessage_16135 + 0x1E, JumpHook, 5);
    HandlerAddress = WoWBase + NetClient__ProcessMessage_16135 + 0x26;
}

#endif

void Inject()
{
    HookHandler();
}
