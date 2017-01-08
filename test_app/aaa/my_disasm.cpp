#include <iostream>
#include "hde.h"
 
#pragma comment(lib, "D:\\temp\\aaa\\hde.lib")
 
int main()
{
  HDE_STRUCT hs;
  void* code = (void*)0x00401000;  //point to the code
 
  unsigned int len = hde_disasm(code,&hs);
 
  printf("\n\n"
    "  length:  0x%02x\n"
    "  opcode:        0x%02x\n",
    len,hs.opcode);
 
  return 0;
}