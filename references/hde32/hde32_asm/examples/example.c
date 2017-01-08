#include <stdio.h>
#include "hde32.h"

char fmt[] = "\n"
  " cmp dword [edi+0x12],0x12345678\n\n"
  "  length of command:  0x%02x\n"
  "  immediate32:        0x%08x\n"
  "  displacement8:      0x%02x\n";

unsigned char code[] = {0x81,0x7f,0x12,0x78,0x56,0x34,0x12};

int main(void)
{
    hde32s hs;

    unsigned int length = hde32_disasm(code,&hs);

    if (hs.flags & F_ERROR)
        printf("Invalid instruction !\n");
    else
        printf(fmt,length,hs.imm.imm32,hs.disp.disp8);

    return 0;
}
