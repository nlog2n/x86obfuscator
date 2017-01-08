/* created by: fanghui  28 August, 2010  */
/* x86.Virtualizer
 *   rev 20101005:
 *          vm_init(), vm_protect()
 */


#include "protect.h"


DWORD rva2raw(WORD NumOfSections, IMAGE_SECTION_HEADER* ish, DWORD rva)
{
  for (int i = NumOfSections-1; i >= 0; i--)
    if (ish[i].VirtualAddress <= rva) 
      return ish[i].PointerToRawData + rva - ish[i].VirtualAddress;
  return 0xFFFFFFFF;
}


DWORD search_import_func(BYTE* exeMem, char* func_name, char* lib_name)
{
  IMAGE_NT_HEADERS* inh = (IMAGE_NT_HEADERS*)(exeMem + ((IMAGE_DOS_HEADER*)exeMem)->e_lfanew);
  IMAGE_SECTION_HEADER* ish = (IMAGE_SECTION_HEADER*)(exeMem + ((IMAGE_DOS_HEADER*)exeMem)->e_lfanew + inh->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + 4);

  IMAGE_IMPORT_DESCRIPTOR* imports = (IMAGE_IMPORT_DESCRIPTOR*)inh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
  imports = (IMAGE_IMPORT_DESCRIPTOR*)(exeMem + rva2raw(inh->FileHeader.NumberOfSections, ish, (DWORD)imports));
  bool found = false;
  while (!found && imports->Name)
  {
    char* libName = (char*)exeMem + rva2raw(inh->FileHeader.NumberOfSections, ish, imports->Name);
    if (!stricmp(libName, lib_name))   // match library name
    {
      found = true;
      DWORD* thunks = (DWORD*)(imports->FirstThunk);
      thunks = (DWORD*)(exeMem + rva2raw(inh->FileHeader.NumberOfSections, ish, imports->FirstThunk));
      bool found2 = false;
      int k = 0;
      while (!found2 && *thunks)
      {
        char* curName = (char*)exeMem + rva2raw(inh->FileHeader.NumberOfSections, ish, *thunks);
        if (!stricmp(curName + 2, func_name))  // match function name
        {
          return imports->FirstThunk + k*4;
          found2 = true;
        }
        k++;
        thunks++;
      }
    }
    imports++;
  }

}


// 20101012
// usage:   get_import_func_rva( inh, "MessageBoxA", "user32.dll" )
//          get_import_func_rva( inh, "VirtualAlloc", "kernel32.dll")
DWORD get_import_func_rva( IMAGE_NT_HEADERS* inh, char* func_name, char* lib_name )
{
    // get memory address of function MessageBox
    HINSTANCE gLibMsg=LoadLibrary(lib_name);
    DWORD     func_va = (DWORD)GetProcAddress(gLibMsg,func_name);

    // compute rva for function MessageBoxA
  return func_va - inh->OptionalHeader.ImageBase;

}



// Func:     transform base relocations to simple table of RVAs
// Output:   fill relocMap if not null
// Return:   # of relocations 
// Called by: vm_protect() only
int genRelocMap(BYTE* relocSeg, DWORD funcRVA, int funcSize, DWORD* relocMap)
{
  BYTE* relocPtr = relocSeg;
  //DWORD delta = (DWORD)newImageBase - inh->OptionalHeader.ImageBase;
  int relCnt = 0;
  while (*(DWORD*)relocPtr) {
    DWORD relocRVA  = ((DWORD*)relocPtr)[0];
    DWORD blockSize = ((DWORD*)relocPtr)[1];    
    for (int i = 0; i < (blockSize - 8) / 2; i++) {
      //if (((WORD*)(relocPtr + 8))[i] & 0xF000)
      //{
        //*(DWORD*)(newImageBase + relocRVA + (((WORD*)(relocPtr + 8))[i] & 0xFFF)) += delta;
      //}

      // For x86 executables, base relocation type = IMAGE_REL_BASED_HIGHLOW=3
      DWORD relocRVA_i = relocRVA + (((WORD*)(relocPtr + 8))[i] & 0xFFF);
      if (  (relocRVA_i >= funcRVA) && (relocRVA_i < funcRVA + funcSize) ) {
        if (relocMap) {
          relocMap[relCnt] = relocRVA_i; 
        }
        relCnt ++;

      }
    }
    relocPtr += blockSize;
  }
  return relCnt;
}






#define TRUNC(a, b) (a + (b - ((a % b) ? (a % b) : b)))




MyObfuscator::MyObfuscator( char* fileName )
{
  m_errno = 0;

  /* read input file to memory */
  HANDLE hFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if (hFile == INVALID_HANDLE_VALUE) {
    m_errno = 1; //err = "Cannot open input file.";
    return;
  }
  m_fSize  = GetFileSize(hFile, 0);
  m_hInMem = (BYTE*)GlobalAlloc(GMEM_FIXED, m_fSize);
  if (!m_hInMem) {
    m_errno = 2; //error = "Cannot allocate memory.";
    return;
  }
  DWORD tmp;
  ReadFile(hFile, m_hInMem, m_fSize, &tmp, 0);
  CloseHandle(hFile);

  /* get a new obf-ed file name */
  strcpy(m_inFileName, fileName);  
  strcpy(m_outFileName,fileName);
  m_outFileName[strlen(m_outFileName) - 4]   = 0;
  strcat(m_outFileName, "_obf.exe");


  /* init VM */
  m_vmImage = 0;

  // insert vm core here ?

}



MyObfuscator::~MyObfuscator()
{
  GlobalFree(m_vmImage);
  GlobalFree(m_hInMem);
}


// Func:      create a memory for VM, ...
// Output:    vmCore  - address of _vm_jump in memory, not whole m_vmImage
//            vmInit
//            vmStart
//            m_vmImage (implicitly)
//            m_vmCoreSize (implicitly, = return value
//            ...
// Return:    vm core size, starting from _vm_jump            
// Called by: do_protect() and vm_protect_vm()
int MyObfuscator::vm_init(BYTE* &_vmCore, DWORD &_vmInit, DWORD &_vmStart)  
{ 
  //load vm image: "D:\\temp\\obfuscator_x86win32\\Debug\\loader.exe"
  HANDLE hVMFile = CreateFile(TEXT("loader.exe"), GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if ( hVMFile == INVALID_HANDLE_VALUE )
  {
    m_errno = 8; // "cannot open loader.exe file" //GetLastError();
    return 0;
  }

  DWORD vmFileSize = GetFileSize(hVMFile, 0) - 0x400;

  if ( m_vmImage ) GlobalFree( m_vmImage );   // re-init
  m_vmImage = (BYTE*)GlobalAlloc(GMEM_FIXED, vmFileSize);
  if( m_vmImage == NULL )
  {
    m_errno = 9; // "GlobalAlloc failed \n"
    return 0;
  }

  SetFilePointer(hVMFile, 0x400, 0, FILE_BEGIN);
  DWORD tmp;
  ReadFile(hVMFile, m_vmImage, vmFileSize, &tmp, 0);
  CloseHandle(hVMFile);

  m_vmCoreSize      = *(DWORD*) m_vmImage;                    // counted by vm_end_label - _vm_jump, refer to loader.asm
  DWORD vmCodeStart = *(DWORD*)(m_vmImage + 4);              // from label _vm_jump
  DWORD _ssss       = (*(DWORD*)(m_vmImage + 28))*4 + (*(DWORD*)(m_vmImage + 32))*8 + 4;    //  8 * 4 + 22*8 + 4, the data part of loader.asm code section
  _vmInit           = *(DWORD*)(m_vmImage + 8) - _ssss;     // 287h
  _vmStart          = *(DWORD*)(m_vmImage + 12) - _ssss;    // 3d0h
  DWORD vmPoly      = *(DWORD*)(m_vmImage + 16);            // 20eh
  DWORD vmPrefix    = *(DWORD*)(m_vmImage + 20);           // 2c8h
  DWORD vmOpcodeTab = *(DWORD*)(m_vmImage + 24);           // 2d0h

  _vmCore           = m_vmImage + _ssss;  

  genPolyEncDec();
  memmove(m_vmImage + vmPoly, _vm_poly_dec, sizeof(_vm_poly_dec));
  genPermutation(condTab, 16);
  BYTE invCondTab[16];
  memmove(invCondTab, condTab, 16);
  invPerm16(invCondTab);
  permutateJcc((WORD*)(m_vmImage + vmCodeStart + 17), 16, invCondTab);
  genPermutation(opcodeTab, VM_INSTR_COUNT);
  memmove(m_vmImage + vmOpcodeTab, opcodeTab, VM_INSTR_COUNT);
  invPerm256(m_vmImage + vmOpcodeTab);
  *(WORD*)(m_vmImage + vmPrefix) = VM_INSTR_PREFIX;

  return m_vmCoreSize;
}


// Func:      transform protected code into VM byte-code
// Input:     codeBase, codeSize, inExeFuncRVA,relocBuf, imgBase
// Output:    outCodeBuf
// Return:    size of output codes
// Called by: do_protect(), vm_protect_vm()
int MyObfuscator::vm_protect(BYTE* codeBase, int codeSize, BYTE* outCodeBuf, DWORD inExeFuncRVA, BYTE* relocBuf, DWORD imgBase)
{
  //relocations
  int relocs;
  DWORD* relocMap = 0;  /* init to zero for checking, by fanghui */
  if (relocBuf) {
    relocs = genRelocMap(relocBuf, inExeFuncRVA, codeSize, 0);
    relocMap = (DWORD*)GlobalAlloc(GMEM_FIXED, 4*relocs);
    genRelocMap(relocBuf, inExeFuncRVA, codeSize, relocMap);
  }

  //disasm_struct dis;
  struct{
    DWORD disasm_len;
  } dis; 

  int instrCnt = genCodeMap(codeBase, codeSize, 0);
  if (instrCnt == -1) return -1;

  DWORD* codeMap    = (DWORD*)GlobalAlloc(GMEM_FIXED, 4*instrCnt + 4);
  DWORD* outCodeMap = (DWORD*)GlobalAlloc(GMEM_FIXED, 4*instrCnt + 8);  //one "byte-code" more for vm_end
  genCodeMap(codeBase, codeSize, codeMap);
  codeMap[instrCnt]        = 0;
  outCodeMap[instrCnt + 1] = 0;

  int relocPtr = 0;
  int curPos   = 0;
  int outPos   = 0;
  DWORD index  = 0;
  while (curPos != codeSize)
  {   
    int opSplitSize = 0;
    //patch for LDE engine bug ;p (kurwa?jego ma?
    //BYTE ldeExcept[3] = {0x8D, 0x84, 0x05};
    if ((curPos - 3 < codeSize) && 
      ((((*(DWORD*)(codeBase + curPos)) & 0xFFFFFF) == 0x05848D) ||
      (((*(DWORD*)(codeBase + curPos))  & 0xFFFFFF) == 0x15948D) ||
      (((*(DWORD*)(codeBase + curPos))  & 0xFFFFFF) == 0x0D8C8D) ||
      (((*(DWORD*)(codeBase + curPos))  & 0xFFFFFF) == 0x1D9C8D)
      )) dis.disasm_len = 7;
    else dis.disasm_len = lde(codeBase + curPos);   
    //
    int prefSize = dis.disasm_len >> 8;
    dis.disasm_len &= 0xFF;
    if (!dis.disasm_len) {
      GlobalFree(outCodeMap);
      GlobalFree(codeMap);
      GlobalFree(relocMap);
      return -1;
    }

    outCodeMap[index] = outPos;

    if (relocBuf && (codeMap[index]     < relocMap[relocPtr] - inExeFuncRVA) 
                 && (codeMap[index + 1] > relocMap[relocPtr] - inExeFuncRVA))
    {     
      if (outCodeBuf) { 
        PUT_VM_OP_SIZE(dis.disasm_len + 5);
        PUT_VM_PREFIX;
        PUT_VM_OPCODE(I_VM_RELOC);
        *(outCodeBuf + outPos + 4) = relocMap[relocPtr] - inExeFuncRVA - codeMap[index];
        *(outCodeBuf + outPos + 5) = dis.disasm_len; 
        memmove(outCodeBuf + outPos + 6, codeBase + curPos, dis.disasm_len);
        (*(DWORD*)(outCodeBuf + outPos + 6 + relocMap[relocPtr] - inExeFuncRVA - codeMap[index])) -= imgBase;
      } 
      outPos += dis.disasm_len + 6;
      relocPtr++;
    }
    else if (!prefSize) {
      if ((((codeBase + curPos + prefSize)[0] & 0xF0) == 0x70) || (((codeBase + curPos + prefSize)[0] == 0x0F) && (((codeBase + curPos + prefSize)[1] & 0xF0) == 0x80)))
      {
        if (outCodeBuf) {
          //conditional jumps correct and generate
          bool shortjmp = true;
          if ((codeBase + curPos + prefSize)[0] == 0x0F) shortjmp = false;
          PUT_VM_OP_SIZE(8);
          PUT_VM_PREFIX;
          if (shortjmp) PUT_VM_OPCODE(I_COND_JMP_SHORT);  //        
          else          PUT_VM_OPCODE(I_COND_JMP_LONG);      //put Jcc opcode
          BYTE condition;
          if (shortjmp) condition = (codeBase + curPos + prefSize)[0] & 0xF;
          else          condition = (codeBase + curPos + prefSize)[1] & 0xF;
          *(outCodeBuf + outPos + 4) = condTab[condition];    //put condition
          DWORD delta;
          if (shortjmp) delta = (int)*(char*)(codeBase + curPos + 1);   //byte extended to dword with sign
          else          delta = *(DWORD*)(codeBase + curPos + 2);
          *(DWORD*)(outCodeBuf + outPos + 5) = delta;
        }
        outPos += 9;  //fixed length for all conditional jumps (no short/long)

      }
      OPCODE_BEGIN_MAP_B
      OPCODE_MAP_ENTRY(0xE9)
      OPCODE_MAP_ENTRY(0xEB)
      OPCODE_BEGIN_MAP_E
      {
        if (outCodeBuf) {
          PUT_VM_OP_SIZE(7);
          PUT_VM_PREFIX;
          //PUT_VM_OPCODE(I_JMP);
          if ((codeBase + curPos + prefSize)[0] == 0xE9) {
            PUT_VM_OPCODE(I_JMP_LONG);
            *(DWORD*)(outCodeBuf + outPos + 4) = *(DWORD*)(codeBase + curPos + 1);
          }
          else {
            /*if ((codeBase + curPos + prefSize)[1] == 1)
            {
              PUT_VM_OPCODE(I_VM_NOP);
              *(DWORD*)(outCodeBuf + outPos + 4) = rand();
              curPos++;
            }
            else*/
            {
              PUT_VM_OPCODE(I_JMP_SHORT);
              *(DWORD*)(outCodeBuf + outPos + 4) = (int)*(char*)(codeBase + curPos + 1);
            }
          }
        }
      }
      OPCODE_END(8)
        OPCODE_BEGIN(0xE3)  //JECXZ/JCXZ
      {
        PUT_VM_OP_SIZE(7);
        PUT_VM_PREFIX;
        PUT_VM_OPCODE(I_JECX);
        *(DWORD*)(outCodeBuf + outPos + 5) = (int)*(char*)(codeBase + curPos + 1);
      }
      OPCODE_END(8)
        OPCODE_BEGIN(0xE8)  //relative direct calls
      {
        PUT_VM_OP_SIZE(7);
        PUT_VM_PREFIX;
        if (*(DWORD*)(codeBase + curPos + 1)) PUT_VM_OPCODE(I_CALL_REL);          
        else PUT_VM_OPCODE(I_VM_FAKE_CALL);
        *(DWORD*)(outCodeBuf + outPos + 4) = inExeFuncRVA + *(DWORD*)(codeBase + curPos + 1) + curPos + 5;
      }   
      OPCODE_END(8)
        OPCODE_BEGIN(0xC2)  //ret xxxx
      {
        PUT_VM_OP_SIZE(5);
        PUT_VM_PREFIX;
        PUT_VM_OPCODE(I_RET);
        *(WORD*)(outCodeBuf + outPos + 4) = *(WORD*)(codeBase + curPos + 1);
      }
      OPCODE_END(6)
        OPCODE_BEGIN(0xC3)  //ret
      {
        PUT_VM_OP_SIZE(5);
        PUT_VM_PREFIX;
        PUT_VM_OPCODE(I_RET);
        *(WORD*)(outCodeBuf + outPos + 4) = 0;
      }
      OPCODE_END(6)
        OPCODE_BEGIN_3(0xE0, 0xE1, 0xE2)  //loop, loope, loopne
      {
        PUT_VM_OP_SIZE(8);
        PUT_VM_PREFIX;
        PUT_VM_OPCODE(I_LOOPxx);
        *(outCodeBuf + outPos + 4) = (codeBase + curPos + prefSize)[0] & 0x0F;
        *(WORD*)(outCodeBuf + outPos + 5) = (int)*(char*)(codeBase + curPos + 1);
      }
      OPCODE_END(9)
        OPCODE_BEGIN_MAP_B
        OPCODE_MAP_ENTRY(0x01)  //ADD   mem, r32
        OPCODE_MAP_ENTRY(0x03)  //ADD   r32, mem
        OPCODE_MAP_ENTRY(0x09)  //OR    mem, r32
        OPCODE_MAP_ENTRY(0x0B)  //OR    r32, mem
        OPCODE_MAP_ENTRY(0x21)  //AND   mem, r32
        OPCODE_MAP_ENTRY(0x23)  //AND   r32, mem
        OPCODE_MAP_ENTRY(0x29)  //SUB   mem, r32
        OPCODE_MAP_ENTRY(0x2B)  //SUB   r32, mem
        OPCODE_MAP_ENTRY(0x31)  //XOR   mem, r32
        OPCODE_MAP_ENTRY(0x33)  //XOR   r32, mem
        OPCODE_MAP_ENTRY(0x39)  //CMP   mem, r32
        OPCODE_MAP_ENTRY(0x3B)  //CMP   r32, mem
        OPCODE_MAP_ENTRY(0x85)  //TEST  r32, mem
        OPCODE_MAP_ENTRY(0x89)  //MOV   mem, r32
        OPCODE_MAP_ENTRY(0x8B)  //MOV   r32, mem
        OPCODE_MAP_ENTRY(0x8D)  //LEA   r32, mem
        OPCODE_BEGIN_MAP_E
      {
        int _mod, _reg, _rm, _scale, _base, _index;
        _mod = ((codeBase + curPos + prefSize)[1] & 0xC0) >> 6;
        _reg = ((codeBase + curPos + prefSize)[1] & 0x38) >> 3;
        _rm  = (codeBase + curPos + prefSize)[1] & 7;
        BYTE* instr = codeBase + curPos + prefSize;
        if ((instr[1] & 7) == 0x4) {
          _scale = (instr[2] & 0xC0) >> 6;
          _index = (instr[2] & 0x38) >> 3;
          _base  = instr[2] & 7;
        }

        switch (_mod) {
        case 0:
          if (_rm == 4) { //SIB
            if (_index != 4) {
              MAKE_MOV_REG(_index);
              MAKE_SHL_IMM(_scale);
            }
            else {
              MAKE_MOV_IMM(0);
            }
            MAKE_ADD_REG(_base);
            MAKE_REAL_INSTR;
          }
          else if (_rm == 5) {
            MAKE_MOV_IMM(*(DWORD*)(instr + 2));
            MAKE_REAL_INSTR;
          }
          else {
            MAKE_MOV_REG(_rm);
            MAKE_REAL_INSTR;
          }
          break;
        case 1:
          if (_rm == 4) {
            if (_index != 4) {
              MAKE_MOV_REG(_index);
              MAKE_SHL_IMM(_scale);
            }
            else {
              MAKE_MOV_IMM(0);
            }
            MAKE_ADD_REG(_base);
            MAKE_ADD_IMM((int)*((char*)instr + 3));
            MAKE_REAL_INSTR;
          }
          else {
            MAKE_MOV_REG(_rm);
            MAKE_ADD_IMM((int)*((char*)instr + 2));
            MAKE_REAL_INSTR;
          }
          break;
        case 2:
          if (_rm == 4) {
            if (_index != 4) {
              MAKE_MOV_REG(_index);
              MAKE_SHL_IMM(_scale);
            }
            else {
              MAKE_MOV_IMM(0);
            }
            MAKE_ADD_REG(_base);
            MAKE_ADD_IMM(*(DWORD*)(instr + 3));
            MAKE_REAL_INSTR;
          }
          else {         
            MAKE_MOV_REG(_rm);
            MAKE_ADD_IMM(*(DWORD*)(instr + 2));
            MAKE_REAL_INSTR;
          }
          break;
        case 3: //original instruction processing
          MAKE_ORIG_INSTR;
          break;
        }
      }
      OPCODE_END(0)
        OPCODE_BEGIN_MAP_B
        OPCODE_MAP_ENTRY(0x81)    //INSTR    Mem, IMM32 (Rej 0..7)(add, or, adc, sbb, and, sub, xor, cmp)
        OPCODE_MAP_ENTRY(0x8F)    //POP      Mem        (Rej 0)(pop)
        OPCODE_MAP_ENTRY(0xC1)    //INSTR    Mem, Db    (Rej 0..7)(rol, ror, rcl, rcr, sal, shl, shr, sar)
        OPCODE_MAP_ENTRY(0xC7)    //MOV      Mem, IMM32 (Rej 0)(mov)
        OPCODE_MAP_ENTRY(0xD1)    //INSTR    Mem, 1     (Rej 0..7)(rol, ror, rcl, rcr, sal, shl, shr, sar)
        OPCODE_MAP_ENTRY(0xFF)    //INSTR    Mem        (Rej 2, 4, 6)(call, jmp, push)
        OPCODE_BEGIN_MAP_E
      {
        BYTE tmp_imm8;
        int _mod, _reg, _rm, _scale, _index, _base;
        BYTE* instr = codeBase + curPos + prefSize;
        _mod = (instr[1] & 0xC0) >> 6;
        _reg = (instr[1] & 0x38) >> 3;
        _rm = instr[1] & 7;     
        if (_rm == 0x4) {
          _scale = (instr[2] & 0xC0) >> 6;
          _index = (instr[2] & 0x38) >> 3;
          _base = instr[2] & 7;
        }
        switch (_mod) {
        case 0:
          if (_rm == 4) {//SIB
            if (_index != 4) {
              MAKE_MOV_REG(_index);
              MAKE_SHL_IMM(_scale);
            }
            else {
              MAKE_MOV_IMM(0);
            }
            MAKE_ADD_REG(_base);
            //MAKE_REAL_INSTR;
            tmp_imm8 = 3;
          }
          else if (_rm == 5) {
            MAKE_MOV_IMM(*(DWORD*)(instr + 2));
            //MAKE_REAL_INSTR;
            tmp_imm8 = 6;
          }
          else {
            MAKE_MOV_REG(_rm);
            //MAKE_REAL_INSTR;
            tmp_imm8 = 2;
          }
          break;
        case 1:
          if (_rm == 4) {
            if (_index != 4) {
              MAKE_MOV_REG(_index);
              MAKE_SHL_IMM(_scale);
            }
            else {
              MAKE_MOV_IMM(0);
            }
            MAKE_ADD_REG(_base);
            MAKE_ADD_IMM((int)*((char*)instr + 3));
            //MAKE_REAL_INSTR;
            tmp_imm8 = 4;
          }
          else {
            MAKE_MOV_REG(_rm);
            MAKE_ADD_IMM((int)*((char*)instr + 2));
            //MAKE_REAL_INSTR;
            tmp_imm8 = 3;
          }
          break;
        case 2:
          if (_rm == 4) {
            if (_index != 4) {
              MAKE_MOV_REG(_index);
              MAKE_SHL_IMM(_scale);
            }
            else {
              MAKE_MOV_IMM(0);
            }
            MAKE_ADD_REG(_base);
            MAKE_ADD_IMM(*(DWORD*)(instr + 3));
            //MAKE_REAL_INSTR;
            tmp_imm8 = 7;
          }
          else {         
            MAKE_MOV_REG(_rm);
            MAKE_ADD_IMM(*(DWORD*)(instr + 2));
            //MAKE_REAL_INSTR;
            tmp_imm8 = 6;
          }
          break;
        case 3:
          //original instruction processing
          MAKE_ORIG_INSTR;
          break;
        }

        if (_mod < 3) {
          if ((instr[0] == 0xC1) || (instr[0] == 0xD1)) {
            if (instr[0] == 0xC1) tmp_imm8 = instr[tmp_imm8];
            else tmp_imm8 = 1;
            BYTE __op[8] = {I_VM_ROL, I_VM_ROR, I_VM_RCL, I_VM_RCR, I_VM_SHL, I_VM_SHR, I_VM_SAL, I_VM_SAR};          
            MAKE_XXX_REG(tmp_imm8, __op[_reg]);
          }
          else if (instr[0] == 0x81) {
            BYTE __op[8] = {I_VM_ADD, I_VM_OR, I_VM_ADC, I_VM_SBB, I_VM_AND, I_VM_SUB, I_VM_XOR, I_VM_CMP};
            MAKE_XXX_IMM((*(DWORD*)(instr + tmp_imm8)), __op[_reg]);
          }
          else if ((instr[0] == 0xC7) && (!_reg)) { MAKE_XXX_IMM((*(DWORD*)(instr + tmp_imm8)), I_VM_MOV); }
          else if ((instr[0] == 0x8F) && (!_reg)) { MAKE_VM_PURE(I_VM_POP); }       
          else if ((instr[0] == 0xFF) && (_reg == 2)) { MAKE_VM_PURE(I_VM_CALL); }
          else if ((instr[0] == 0xFF) && (_reg == 4)) { MAKE_VM_PURE(I_VM_JMP); }
          else if ((instr[0] == 0xFF) && (_reg == 6)) { MAKE_VM_PURE(I_VM_PUSH); }
          else { MAKE_ORIG_INSTR; }
        }

      }
      OPCODE_END(0)
      else { MAKE_ORIG_INSTR; }   
    }
    else { MAKE_ORIG_INSTR; }  // end if (relocBuf...)

    curPos += dis.disasm_len;
    index++;
    if (curPos == codeSize) {  // reach vm end
      if (outCodeBuf) {
        PUT_VM_OP_SIZE(3);
        PUT_VM_PREFIX;
        PUT_VM_OPCODE(I_VM_END);
      }
      outCodeMap[index] = outPos;
      outPos += 4;
      index++;
    }
  } // end while

  outCodeMap[index] = outPos;

  //cipher loop
  if (outCodeBuf) {
    for (int i = 0; i < instrCnt + 1; i++) {
      //Jcc correction 
      if (*(WORD*)(outCodeBuf + outCodeMap[i] + 1) == VM_INSTR_PREFIX ) {
        //test for Jcc
        if (  (*(outCodeBuf + outCodeMap[i] + 3) == opcodeTab[I_COND_JMP_SHORT]) 
            ||(*(outCodeBuf + outCodeMap[i] + 3) == opcodeTab[I_COND_JMP_LONG]) 
            ||(*(outCodeBuf + outCodeMap[i] + 3) == opcodeTab[I_JECX]) 
            ||(*(outCodeBuf + outCodeMap[i] + 3) == opcodeTab[I_LOOPxx]) 
            ||(*(outCodeBuf + outCodeMap[i] + 3) == opcodeTab[I_JMP_LONG]) 
            ||(*(outCodeBuf + outCodeMap[i] + 3) == opcodeTab[I_JMP_SHORT]) )
        {
          int ttt = 0;
          int jecxcorr = 0;
          if (*(outCodeBuf + outCodeMap[i] + 3) == opcodeTab[I_COND_JMP_LONG]) ttt = 4;
          if (*(outCodeBuf + outCodeMap[i] + 3) == opcodeTab[I_JECX]) jecxcorr = 1;
          if (*(outCodeBuf + outCodeMap[i] + 3) == opcodeTab[I_JMP_LONG]) {
            jecxcorr = 1;
            ttt = 3;
          }
          if (*(outCodeBuf + outCodeMap[i] + 3) == opcodeTab[I_JMP_SHORT]) jecxcorr = 1;
          DWORD outDest = codeMap[i] + *(DWORD*)(outCodeBuf + outCodeMap[i] + 5 - jecxcorr) + 2 + ttt;
          //search outDest in codeMap
          for (int j = 0; j < instrCnt; j++) {
            if (outDest == codeMap[j]) {
              *(DWORD*)(outCodeBuf + outCodeMap[i] + 5 - jecxcorr) = outCodeMap[j] - outCodeMap[i];
              break;
            }
          }
        } // end if
      }  // end if
         
      // poly encryption
      int tmpChr = 0;
      do {
        polyEnc(outCodeBuf + outCodeMap[i] + tmpChr + 1, *(outCodeBuf + outCodeMap[i] + tmpChr), outCodeMap[i] + tmpChr);
        BYTE __tt = *(outCodeBuf + outCodeMap[i] + tmpChr);           
        *(outCodeBuf + outCodeMap[i] + tmpChr) ^= *(outCodeBuf + outCodeMap[i] + tmpChr + 1);
        tmpChr += __tt + 1;
      } while (outCodeMap[i + 1] != outCodeMap[i] + tmpChr);
      
    } // end for
  }   // end if

  if (relocMap) {  /* add check by fanghui */
    GlobalFree(relocMap);
  }

  GlobalFree(codeMap);
  GlobalFree(outCodeMap);
  return outPos;
}


//Called by: do_protect()
int MyObfuscator::vm_protect_vm(BYTE* vm_in_exe, BYTE* outBuf, DWORD imgBase, DWORD vmRVA, DWORD newRVA)
{
  BYTE* hVMCore;
  DWORD vmInit;
  DWORD vmStart;
  int vmSize;
  DWORD* hVMImg;
  if (!outBuf) {
    vmSize  = vm_init( hVMCore, vmInit, vmStart);  // fanghui: difference in creating again?
    hVMImg  = (DWORD*) m_vmImage; // convert to (DW*)
  }
  else {
    hVMImg  = (DWORD*) m_vmImage; // conver to (DW*)
    vmSize  = m_vmCoreSize;  
    DWORD _ssss = (*(DWORD*)(hVMImg + 7))*4 + (*(DWORD*)(hVMImg + 8))*8 + 4;
    vmInit  = *(DWORD*)(hVMImg + 2) - _ssss;
    vmStart = *(DWORD*)(hVMImg + 3) - _ssss;
    hVMCore = (BYTE*)hVMImg + _ssss;
  }

  int cnt   = hVMImg[7];
  cnt       = hVMImg[cnt];
  int curOutPos = 0;

  if (outBuf) memmove(outBuf, hVMCore, vmSize);
  curOutPos += vmSize;  

  for (int i = 0; i < cnt; i++) {
    int cur = hVMImg[7] + 2*i + 1;
    if (!outBuf) {
      hVMImg[cur]     -= (0x401000 + hVMImg[7]*4 + cnt*8 + 4);
      hVMImg[cur + 1] -= (0x401000 + hVMImg[7]*4 + cnt*8 + 4);
    }

    int lalala = curOutPos;
    BYTE* __outBuf;
    if (outBuf) __outBuf = outBuf + curOutPos;
    else __outBuf = 0;
    curOutPos += vm_protect(vm_in_exe + hVMImg[cur], hVMImg[cur + 1] - hVMImg[cur], __outBuf, vmRVA + hVMImg[cur], 0, imgBase);
    //MAKE_VM_CALL(imgBase, vm_in_exe + hVMImg[cur], vmRVA + hVMImg[cur], imgBase + newRVA + lalala, hVMImg[cur + 1] - hVMImg[cur], newRVA + vmStart);
    if (outBuf) {
      MAKE_VM_CALL2(imgBase, vm_in_exe + hVMImg[cur], vmRVA + hVMImg[cur], newRVA + lalala, hVMImg[cur + 1] - hVMImg[cur], newRVA + vmStart, outBuf + curOutPos, newRVA + curOutPos);
    }
    curOutPos += VM_CALL_SIZE;
  }

  return curOutPos;
}


// Called by: do_protect() only
// Input:     ListBox, PE header
// Output:    RVA ranges, and another format in (offset, size)
// Return:    number of blocks/ranges to protect
int  MyObfuscator::get_block_ranges( HWND                   listBox, 
                   IMAGE_NT_HEADERS*      inh, 
                   IMAGE_SECTION_HEADER*  ish, 
                   DWORD*                &RVA_ranges, 
                   DWORD*                &Raw_offset_sizes )
{
  int itemsCnt    = SendMessage(listBox, LB_GETCOUNT, 0, 0);
  if (!itemsCnt){
    m_errno = 4; // error = "Nothing to protect (add at least one range).";
    return 0;
  }

  RVA_ranges         = (DWORD*)GlobalAlloc(GMEM_FIXED, itemsCnt*8);
  Raw_offset_sizes   = (DWORD*)GlobalAlloc(GMEM_FIXED, itemsCnt*8);
  for (int i = 0; i < itemsCnt; i++)
  {
    char temp[25];
    SendMessage(listBox, LB_GETTEXT, i, (LPARAM)temp);  // get one RVA range (in memory not file)
    temp[8]         = 0;
    sscanf(temp,      "%x", &RVA_ranges[i*2]);
    sscanf(temp + 11, "%x", &RVA_ranges[i*2 + 1]);
    RVA_ranges[i*2]     -= inh->OptionalHeader.ImageBase;    // start rva
    RVA_ranges[i*2 + 1] -= inh->OptionalHeader.ImageBase;    // end rva

    Raw_offset_sizes[i*2] = rva2raw(inh->FileHeader.NumberOfSections, ish, RVA_ranges[i*2]);  // turn into raw offset
    if (Raw_offset_sizes[i*2] == 0xFFFFFFFF) 
    {
      m_errno = 5; // error = "Invalid range start"; 
      GlobalFree(RVA_ranges); GlobalFree(Raw_offset_sizes); 
      return 0; 
    }
    Raw_offset_sizes[i*2 + 1] = RVA_ranges[i*2 + 1] - RVA_ranges[i*2];                       // turn into size

  }
    return itemsCnt;
}


void MyObfuscator::do_protect(HWND listBox, bool vmovervm)
{
  DWORD tmp;
  if ( !m_hInMem ){
    // error = "cannot open input file";
    return;
  }

  IMAGE_NT_HEADERS* inh     = (IMAGE_NT_HEADERS*)(m_hInMem + ((IMAGE_DOS_HEADER*)m_hInMem)->e_lfanew);
  IMAGE_SECTION_HEADER* ish = (IMAGE_SECTION_HEADER*)(m_hInMem + ((IMAGE_DOS_HEADER*)m_hInMem)->e_lfanew + inh->FileHeader.SizeOfOptionalHeader + sizeof(IMAGE_FILE_HEADER) + 4);



  DWORD vAlloc = search_import_func(m_hInMem, "VirtualAlloc", "kernel32.dll"); // require the target program contains VirtualAlloc in its import table
  if (vAlloc == 1){
    m_errno = 7; //  this function is not in import table of protected program
    return;
  }

/*
  DWORD vAlloc2 = get_import_func_rva( inh, "VirtualAlloc", "kernel32.dll");
  if ( vAlloc2 == NULL ){
    m_errno = 7; //  this function is not in import table of protected program
    return;
  }
*/

  //relocs check
  DWORD rel = 0;
  if (inh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress)  {
    rel = rva2raw(inh->FileHeader.NumberOfSections, ish, inh->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
    if (rel == 0xFFFFFFFF) {
      m_errno = 3; // err = "Invalid relocations RVA.";
      return;
    }
    rel += (DWORD)m_hInMem;
  }

  //vm init
  BYTE* hVMCore;   // VM core
  DWORD vmInit;
  DWORD vmStart;
  srand(time(0));
  int vmSize = vm_init( hVMCore, vmInit, vmStart);  


  //build table for protected range:
  int itemsCnt;
  DWORD* RVA_ranges;
  DWORD* Raw_offset_sizes;
  itemsCnt = get_block_ranges(listBox, inh, ish, RVA_ranges, Raw_offset_sizes);
  if (!itemsCnt)  {
    return;
  }

  // first run to compute protSize
  int protSize    = 0;
  for (int i = 0; i < itemsCnt; i++) {
    int t  = vm_protect(m_hInMem + Raw_offset_sizes[i*2], Raw_offset_sizes[i*2 + 1], 0, RVA_ranges[i*2], (BYTE*)rel, inh->OptionalHeader.ImageBase);
    if (t == -1){
      m_errno = 6; // error = "[SIZE] Protection failed."; 
      GlobalFree(RVA_ranges); GlobalFree(Raw_offset_sizes); 
      return;
    }
    protSize += t;
  }

  //loader size = 0x3C = 60 bytes
  static char m_loaderAlloc[] = {
    0xE8, 0x00, 0x00, 0x00, 0x00,       //41B928: CALL    test_app.0041B92D
    0x5B,                               //41B92D: POP     EBX
    0x81, 0xEB, 0x2D, 0xB9, 0x01, 0x00, //        SUB     EBX,1B92D ; 1B92D replaced by newRVA +vmSize +5 = 1B92D
    0x6A, 0x40,                         //        PUSH    40
    0x68, 0x00, 0x10, 0x00, 0x00,       //        PUSH    1000
    0x68, 0x00, 0x10, 0x00, 0x00,       //        PUSH    1000
    0x6A, 0x00,                         //        PUSH    0
    0xB8, 0x34, 0x12, 0x00, 0x00,       //        MOV     EAX,1234  ; 1234 replaced by vAlloc
    0x03, 0xC3,                         //        ADD     EAX,EBX
    0xFF, 0x10,                         //        CALL    [EAX]     ; -----------------------------
    0x53,                               //        PUSH    EBX
    0x05, 0x00, 0x10, 0x00, 0x00,       //        ADD     EAX,1000
    0x50,                               //        PUSH    EAX
    0xB8, 0x34, 0x12, 0x00, 0x00,       //        MOV     EAX,1234  ; 1234 replaced by newRVA + vmInit
    0x03, 0xC3,                         //        ADD     EAX,EBX
    0xFF, 0xD0,                         //        CALL    EAX       ; -----------------------------
    0xB8, 0x34, 0x12, 0x00, 0x00,       //        MOV     EAX,1234  ; 1234 replace by oldEntry
    0x03, 0xC3,                         //        ADD     EAX,EBX
    0xFF, 0xE0                          //        JMP     EAX
  };



  /* create VM section */
  DWORD newRVA = (ish + inh->FileHeader.NumberOfSections - 1)->VirtualAddress + TRUNC((ish + inh->FileHeader.NumberOfSections - 1)->Misc.VirtualSize, inh->OptionalHeader.SectionAlignment);  
  DWORD vmSectionSize = vmSize + protSize + sizeof(m_loaderAlloc) + itemsCnt*VM_CALL_SIZE;
  BYTE* hNewMem       = (BYTE*)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, vmSectionSize);
  int curPos          = 0;
  memmove(hNewMem, hVMCore, vmSize);
  curPos             += vmSize;

  DWORD oldEntry = inh->OptionalHeader.AddressOfEntryPoint;   
  inh->OptionalHeader.AddressOfEntryPoint = newRVA + vmSize; //setting new entry point

  //correcting loader:
  *(DWORD*)(m_loaderAlloc + 8)  = newRVA + vmSize + 5;
  *(DWORD*)(m_loaderAlloc + 27) = vAlloc;   //VirtualAlloc at 0xA0F8
  *(DWORD*)(m_loaderAlloc + 43) = newRVA + vmInit;
  *(DWORD*)(m_loaderAlloc + 52) = oldEntry;
  memmove(hNewMem + vmSize, m_loaderAlloc, sizeof(m_loaderAlloc));
  curPos += sizeof(m_loaderAlloc);


  // second run to write outCodeBuf
  for (int i = 0; i < itemsCnt; i++) {
    int _tts = vm_protect(m_hInMem + Raw_offset_sizes[i*2], Raw_offset_sizes[i*2 + 1], hNewMem + curPos, RVA_ranges[i*2], (BYTE*)rel, inh->OptionalHeader.ImageBase);
    MAKE_VM_CALL2(inh->OptionalHeader.ImageBase, m_hInMem + Raw_offset_sizes[i*2], RVA_ranges[i*2], newRVA + curPos, Raw_offset_sizes[i*2 + 1], newRVA + vmStart, hNewMem + curPos + _tts, newRVA + curPos + _tts);
    curPos += _tts + VM_CALL_SIZE;
  }

  if (vmovervm) {
    //chuj wi czemu ;p
    //newSecSize -= 5;
    //
    int vmovmSize   = vm_protect_vm(hNewMem, 0, inh->OptionalHeader.ImageBase, newRVA, newRVA + curPos);
    //BYTE* tmpMemPtr = (BYTE*)GlobalReAlloc(hNewMem, newSecSize + vmovmSize, GMEM_FIXED | GMEM_ZEROINIT);    
    BYTE* tmpMemPtr = (BYTE*)GlobalAlloc(GMEM_FIXED | GMEM_ZEROINIT, vmSectionSize + vmovmSize + sizeof(m_loaderAlloc));
    memmove(tmpMemPtr, hNewMem, vmSectionSize);
    GlobalFree(hNewMem);
    hNewMem         = tmpMemPtr;
    vm_protect_vm(hNewMem, hNewMem + vmSectionSize, inh->OptionalHeader.ImageBase, newRVA, newRVA + curPos);
    curPos         += vmovmSize;
    
    oldEntry        = inh->OptionalHeader.AddressOfEntryPoint;
    inh->OptionalHeader.AddressOfEntryPoint = newRVA + curPos;

    *(DWORD*)(m_loaderAlloc + 8)  = newRVA + curPos + 5;
    //*(DWORD*)(loaderAlloc + 27) = vAlloc;
    *(DWORD*)(m_loaderAlloc + 43) = newRVA + vmSectionSize + vmInit;
    *(DWORD*)(m_loaderAlloc + 52) = oldEntry;
    memmove(hNewMem + curPos, m_loaderAlloc, sizeof(m_loaderAlloc));
    curPos += sizeof(m_loaderAlloc);

    vmSectionSize += vmovmSize + sizeof(m_loaderAlloc);
  }

  // increased size by alignment
  DWORD vmSectionSizeFileAligned   = TRUNC( vmSectionSize, inh->OptionalHeader.FileAlignment);  
  DWORD vmSectionSizeSecAligned    = TRUNC( vmSectionSize, inh->OptionalHeader.SectionAlignment);


  /* add VM section */
  ish                             += inh->FileHeader.NumberOfSections;
  inh->FileHeader.NumberOfSections++;
  memset(ish, 0, sizeof(IMAGE_SECTION_HEADER));
  ish->Characteristics             = 0xE00000E0;  /* CODE, INITIALIZED_DATA, UNINITIALIZED_DATA, EXECUTE, READ, WRITE */
  ish->Misc.VirtualSize            = vmSectionSizeSecAligned;    // after section aligned 
  inh->OptionalHeader.SizeOfImage += ish->Misc.VirtualSize;
  memmove(ish->Name, ".VM", 4);
  ish->PointerToRawData            = m_fSize;
  ish->SizeOfRawData               = vmSectionSizeFileAligned;   // after file aligned 
  ish->VirtualAddress              = newRVA;

  /* write to a new file */
  HANDLE hFile = CreateFile(m_outFileName, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
  WriteFile(hFile, m_hInMem, m_fSize, &tmp, 0);
  WriteFile(hFile, hNewMem, vmSectionSize, &tmp, 0);
  memset(hNewMem, 0, vmSectionSizeFileAligned - vmSectionSize);   // bug here if rest size > vmSectionSize (hNewMem)? But NOT possible for this case
  WriteFile(hFile, hNewMem, vmSectionSizeFileAligned - vmSectionSize, &tmp, 0);
  CloseHandle(hFile);

  /* free temporary memory */
  GlobalFree(hNewMem);
  GlobalFree(RVA_ranges); 
  GlobalFree(Raw_offset_sizes); 
}
