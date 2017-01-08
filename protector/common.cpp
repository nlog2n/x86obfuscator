/* x86 obfuscator
 * common.cpp
 *    disassembly engine
 *    polymorphism
 *
 */

#include "common.h"

extern BYTE _vm_poly_enc[] = {   // 121 bytes, alse called as a function by vm_protect
0x56,                                           // push  esi
0x50,                                           // push  eax
0x51,                                           // push  ecx
0x8B, 0x74, 0x24, 0x10,                         // mov   esi, dword ptr [esp + 10h]
0x33, 0xC9,                                     // xor   ecx, ecx
0x8A, 0x06,                                     //_loop: mov   al, byte ptr [esi]
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // db    60 DUP (90h) ;commands 2-bytes, +11
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // ;...
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
0x90, 0x90, 0x90, 0x90, 
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // db    30 DUP (90h) ;mingled junks 3-bytes
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, // ;...
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 
0x90, 0x90, 0x90, 0x90,                         // xor   eax, dword ptr [esp + 18h] ; +65, to overwrite
0x88, 0x06,                                     // mov   byte ptr [esi], al
0x41,                                           // inc   ecx
0x46,                                           // inc   esi
0x3B, 0x4C, 0x24, 0x14,                         // cmp   ecx, dword ptr [esp + 14h]
0x75, 0x96,                                     // jnz   _loop
0x59,                                           // pop   ecx
0x58,                                           // pop   eax
0x5E,                                           // pop   esi
0xC2, 0x0C, 0x00                                // ret   0Ch
};

extern BYTE _vm_poly_dec[121] = {0}; // inverse of _vm_poly_enc, 121 bytes
extern polyFunc polyEnc = (polyFunc)(BYTE*)_vm_poly_enc;
extern polyFunc polyDec = (polyFunc)(BYTE*)_vm_poly_dec;

DWORD WINAPI _lde(BYTE* off)
{
  HDE_STRUCT hdeStr;
  hde_disasm(off, &hdeStr); /* decode one instruction */
  return ((hdeStr.p_66 | hdeStr.p_67 | hdeStr.p_lock | hdeStr.p_rep | hdeStr.p_seg) << 8) | hdeStr.len;
}

void genPolyEncDec()
{
  //xor eax, dword ptr [esp+18h]
  //0x18244433
  memmove(_vm_poly_dec, _vm_poly_enc, sizeof(_vm_poly_enc));
  *(DWORD*)(_vm_poly_enc + 0x65) = 0x18244433;
  *(DWORD*)(_vm_poly_dec + 11)   = 0x18244433;

  //XOR val 0x34 val
  //SUB val 0x2C val
  //ADD val 0x04 val
  //XOR CL  0x32 0xC1
  //SUB CL  0x2A 0xC1
  //ADD CL  0x02 0xC1
  //INC     0xFE 0xC0
  //DEC     0xFE 0xC8
  //ROR CL  0xD2 0xC8
  //ROL CL  0xD2 0xC0
  //junk    0xEB 0x01 xx

  int instr = 30;
  int junk  = 10;
  int ptr   = 11;
  while (instr || junk) {
    int w = rand() & 1;
    if (w && junk) {
      _vm_poly_enc[ptr]           = 0xEB;
      _vm_poly_enc[ptr+1]         = 0x01;
      _vm_poly_enc[ptr+2]         = rand();
      _vm_poly_dec[114 - ptr - 1] = 0xEB;
      _vm_poly_dec[114 - ptr]     = 0x01;
      _vm_poly_dec[114 - ptr + 1] = rand();
      ptr += 3;
      junk--;
    }
    else {
      int cinstr = rand() % 10;
      switch (cinstr)
      {
      case 0:
        _vm_poly_enc[ptr]         = 0x34;
        _vm_poly_enc[ptr+1]       = rand();
        _vm_poly_dec[114 - ptr]   = 0x34;
        _vm_poly_dec[114 - ptr+1] = _vm_poly_enc[ptr+1];
        break;
      case 1:
        _vm_poly_enc[ptr]         = 0x2C;
        _vm_poly_enc[ptr+1]       = rand();
        _vm_poly_dec[114 - ptr]   = 0x04;
        _vm_poly_dec[114 - ptr+1] = _vm_poly_enc[ptr+1];
        break;
      case 2:
        _vm_poly_enc[ptr]         = 0x04;
        _vm_poly_enc[ptr+1]       = rand();
        _vm_poly_dec[114 - ptr]   = 0x2C;
        _vm_poly_dec[114 - ptr+1] = _vm_poly_enc[ptr+1];
        break;
      case 3:
        _vm_poly_enc[ptr]         = 0x32;
        _vm_poly_enc[ptr+1]       = 0xC1;
        _vm_poly_dec[114 - ptr]   = 0x32;
        _vm_poly_dec[114 - ptr+1] = 0xC1;
        break;
      case 4:
        _vm_poly_enc[ptr]         = 0x2A;
        _vm_poly_enc[ptr+1]       = 0xC1;
        _vm_poly_dec[114 - ptr]   = 0x02;
        _vm_poly_dec[114 - ptr+1] = 0xC1;
        break;
      case 5:
        _vm_poly_enc[ptr]         = 0x02;
        _vm_poly_enc[ptr+1]       = 0xC1;
        _vm_poly_dec[114 - ptr]   = 0x2A;
        _vm_poly_dec[114 - ptr+1] = 0xC1;
        break;
      case 6:
        _vm_poly_enc[ptr]         = 0xFE;
        _vm_poly_enc[ptr+1]       = 0xC0;
        _vm_poly_dec[114 - ptr]   = 0xFE;
        _vm_poly_dec[114 - ptr+1] = 0xC8;
        break;
      case 7:
        _vm_poly_enc[ptr]         = 0xFE;
        _vm_poly_enc[ptr+1]       = 0xC8;
        _vm_poly_dec[114 - ptr]   = 0xFE;
        _vm_poly_dec[114 - ptr+1] = 0xC0;
        break;
      case 8:
        _vm_poly_enc[ptr]         = 0xD2;
        _vm_poly_enc[ptr+1]       = 0xC8;
        _vm_poly_dec[114 - ptr]   = 0xD2;
        _vm_poly_dec[114 - ptr+1] = 0xC0;
        break;
      case 9:
        _vm_poly_enc[ptr]         = 0xD2;
        _vm_poly_enc[ptr+1]       = 0xC0;
        _vm_poly_dec[114 - ptr]   = 0xD2;
        _vm_poly_dec[114 - ptr+1] = 0xC8;
        break;
      }
      ptr += 2;
      instr--;
    }
  }
}

// Func:    disassemble code to generate an instruction map
// Input:   codeBase, codeSize
// Output:  codeMap if not null, pointing to offset of instruction in codeBase
// Return:  # of instructions
int genCodeMap(BYTE* codeBase, int codeSize, DWORD* codeMap)
{
  int curPos = 0;
  int instrCount = 0;
  //disasm_struct dis;
  struct {
    DWORD disasm_len;
  } dis;
  while (curPos != codeSize) {    
    // compute length of current instruction
    if (  (curPos - 3 < codeSize) 
      && (    (((*(DWORD*)(codeBase + curPos)) & 0xFFFFFF) == 0x05848D) 
           || (((*(DWORD*)(codeBase + curPos)) & 0xFFFFFF) == 0x15948D) 
         || (((*(DWORD*)(codeBase + curPos)) & 0xFFFFFF) == 0x0D8C8D) 
         || (((*(DWORD*)(codeBase + curPos)) & 0xFFFFFF) == 0x1D9C8D)
         )
      ) 
    {
       dis.disasm_len = 7;
    }
    else {
       dis.disasm_len = lde(codeBase + curPos) & 0xFF;    
    }
    
    if (!dis.disasm_len) return -1;

    if (codeMap) codeMap[instrCount] = curPos; /* write codeMap */

    instrCount++;
    curPos += dis.disasm_len;
  }
  return instrCount;
}

void genPermutation(BYTE* buf, int size)
{
  memset(buf, 0, size);
  int i = 0;
  while (i < size) {
    BYTE rnd = rand() % size;
    if (!buf[rnd]){
      buf[rnd] = i;
      i++;
    }
  }
}

void invPerm256(BYTE* buf)
{
  BYTE tmp[256];
  for (int i = 0; i < 256; i++) {
    tmp[buf[i]] = i; 
  }
  memmove(buf, tmp, 256);
}

void invPerm16(BYTE* buf)
{
  BYTE tmp[16];
  for (int i = 0; i < 16; i++) {
    tmp[buf[i]] = i; 
  }
  memmove(buf, tmp, 16);
}


// Func:
// Input:   buf, elemCount
// Output:  permutation
// Called by: vm_init in protect.cpp
void permutateJcc(WORD* buf, int elemCount, BYTE* permutation)
{
  WORD temp[16];
  for (int i = 0; i < elemCount; i++) {
    temp[i] = buf[permutation[i]];
    if (i > permutation[i]) {
      WORD tmp = i - permutation[i];
      tmp    <<= 9;
      temp[i] -= tmp;
    }
    else {
      WORD tmp = permutation[i] - i;
      tmp    <<= 9;
      temp[i] += tmp;
    }
  }
  memmove(buf, temp, 2*16);
}


