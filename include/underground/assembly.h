#ifndef __CAKE_ASSEMBLY_H__
#define __CAKE_ASSEMBLY_H__

/*
        REX est le préfix précédent l'opcode, il est optionnel mais permet de :
        - utiliser une opérande de 64 bits lorsque le mode par défaut de l'instruction n'est pas 64 bits.
        - utiliser un des registres étendus (R8 à R15, XMM8 à XMM15, YMM8 à YMM15, CR8 à CR15 et DR8 à DR15)
        - utiliser l'un des registres d'octets uniformes (SPL, BPL, SIL ou DIL)

        La forme du préfix est comme telle :

          7                           0
        +---+---+---+---+---+---+---+---+
        | 0   1   0   0 | W | R | X | B |
        +---+---+---+---+---+---+---+---+

        +------+
        | 0100 | Modèle fixe, indique que c'est le préfix REX
        +------+
        |   W  | Quand à 1, utilise une opérande de 64 bits.
        +------+
        |   R  | Quand à 1, extension de ModR/M.reg
        +------+
        |   X  | Quand à 1, extension de SIB.index
        +------+
        |   B  | Quand à 1, extension de ModR/M.rm ou de SIB.base. Plus d'infos plus bas.
        +------+

        Quand B est à 1, ModR/M.rm est une extension de l'opcode qui indique quel registre entre R8 et R15 utiliser.
*/


#define CAKE_ASM_X64_REX_PREFIX 0x40
#define CAKE_ASM_X64_REX_W      0x08
#define CAKE_ASM_X64_REX_R      0x04
#define CAKE_ASM_X64_REX_X      0x02
#define CAKE_ASM_X64_REX_B      0x01

#define CAKE_ASM_X64_IS_R8(opcode)  ((opcode & 0x07) == 0x00)
#define CAKE_ASM_X64_IS_R9(opcode)  ((opcode & 0x07) == 0x01)
#define CAKE_ASM_X64_IS_R10(opcode) ((opcode & 0x07) == 0x02)
#define CAKE_ASM_X64_IS_R11(opcode) ((opcode & 0x07) == 0x03)
#define CAKE_ASM_X64_IS_R12(opcode) ((opcode & 0x07) == 0x04)
#define CAKE_ASM_X64_IS_R13(opcode) ((opcode & 0x07) == 0x05)
#define CAKE_ASM_X64_IS_R14(opcode) ((opcode & 0x07) == 0x06)
#define CAKE_ASM_X64_IS_R15(opcode) ((opcode & 0x07) == 0x07)


// PUSH utilise par défaut une opérande en 64 bits
#define CAKE_ASM_X64_PUSH_RAX 0x50      // R8
#define CAKE_ASM_X64_PUSH_RCX 0x51      // R9
#define CAKE_ASM_X64_PUSH_RDX 0x52      // R10
#define CAKE_ASM_X64_PUSH_RBX 0x53      // R11
#define CAKE_ASM_X64_PUSH_RSP 0x54      // R12
#define CAKE_ASM_X64_PUSH_RBP 0x55      // R13
#define CAKE_ASM_X64_PUSH_RSI 0x56      // R14
#define CAKE_ASM_X64_PUSH_RDI 0x57      // R15

#define CAKE_ASM_X64_NOP 0x90

#endif