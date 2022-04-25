#include "../hook.h"
#include "../assembly.h"

#include <stdio.h>

#ifdef CAKE_WINDOWS
#include <windows.h>


cake_bool cake_attach_hookrelay(Cake_HookRelay *relay, void *destFunc, void *targetFunc, ushort pageSize, ushort bytesRequired) {
    DWORD oldProtect;
    if(!VirtualProtect(targetFunc, bytesRequired, PAGE_EXECUTE_READWRITE, &oldProtect))
        return cake_false;

    ulonglong pageTargetFunc = CAKE_GET_PAGE_OF_ADDR(targetFunc, pageSize);
    ulonglong minAddr = pageTargetFunc - 0x7FFFFF00;    // L'instruction jmp ne peut pas aller au delà de 2 go
    ulonglong startPage = (pageTargetFunc - (pageTargetFunc % pageSize));
    ulonglong byteOffset = pageSize;
    ulonglong lowAddr;
    relay->ptr = NULL;
    relay->originaleBytes = NULL;
    relay->originaleBytesLength = bytesRequired;
    relay->targetFunc = targetFunc;

    // On cherche à allouer de la mémoire assez proche de la fonction ciblée pour pouvoir mettre un jump vers le relay,
    // qui lui redirigera vers la fonction destination.
    while(1) {
        lowAddr = startPage - byteOffset;
        relay->ptr = (uchar *) VirtualAlloc((void *) lowAddr, pageSize, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
        if(relay->ptr != NULL) {
            ulonglong destValue = (ulonglong) destFunc;
            relay->ptr[0] = 0x49; relay->ptr[1] = 0xBA;     // mov r10, ...
            relay->ptr[2] = destValue & 0xFF;
            relay->ptr[3] = (destValue >> 8)  & 0xFF;
            relay->ptr[4] = (destValue >> 16) & 0xFF;
            relay->ptr[5] = (destValue >> 24) & 0xFF;
            relay->ptr[6] = (destValue >> 32) & 0xFF;
            relay->ptr[7] = (destValue >> 40) & 0xFF;
            relay->ptr[8] = (destValue >> 48) & 0xFF;
            relay->ptr[9] = (destValue >> 56) & 0xFF;
            relay->ptr[10] = 0x41; relay->ptr[11] = 0xFF; relay->ptr[12] = 0xE2; // jmp r10

            uint relativeAddr = (ulonglong) relay->ptr - (ulonglong) targetFunc - 5;    // L'instruction jmp prend 5 octets
            relay->originaleBytes = (uchar *) malloc(bytesRequired * sizeof(uchar));
            memcpy(relay->originaleBytes, targetFunc, bytesRequired * sizeof(uchar));
            
            uchar *ptr = (uchar *) targetFunc;
            ptr[0] = 0xE9;  // jmp ...
            ptr[1] = relativeAddr & 0xFF;
            ptr[2] = (relativeAddr >> 8)  & 0xFF;
            ptr[3] = (relativeAddr >> 16) & 0xFF;
            ptr[4] = (relativeAddr >> 24) & 0xFF;
            ushort i;
            for(i = 5; i < bytesRequired; ++i)
                ptr[i] = CAKE_ASM_X64_NOP;
            VirtualProtect(targetFunc, bytesRequired, oldProtect, &oldProtect);
            return cake_true;
        }
        if(lowAddr < minAddr)
            return cake_false;
        byteOffset += pageSize;
    }
    return cake_false;
}

cake_bool cake_remove_hookrelay(Cake_HookRelay *relay) {
    DWORD oldProtect;
    if(!VirtualProtect(relay->targetFunc, sizeof(relay->originaleBytes), PAGE_EXECUTE_READWRITE, &oldProtect))
        return cake_false;
    uchar *ptr = (uchar *) relay->targetFunc;
    ushort i;
    for(i = 0; i < relay->originaleBytesLength; ++i)
        ptr[i] = relay->originaleBytes[i];
    VirtualProtect(relay->targetFunc, relay->originaleBytesLength, oldProtect, &oldProtect);
    return cake_true;
}

void cake_free_hookrelay(Cake_HookRelay *relay) {
    VirtualFree(relay->ptr, 0, MEM_RELEASE);
    free(relay->originaleBytes);
}

#endif