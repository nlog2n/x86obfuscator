/* created by: fanghui  28 August, 2010  */
/* x86.Virtualizer
 * rev 20101005: 
 *      use reference instead of pointer for input parameters in vm_init()
 */


#ifndef _PROTECT_H_
#define _PROTECT_H_

#include <cstdio>
#include <ctime>
#include <Windows.h>

#include "macros.h"
#include "common.h"




class MyObfuscator{
private:
    DWORD m_vmKey;
    
    char  m_inFileName[MAX_PATH];
    char  m_outFileName[MAX_PATH];
    BYTE* m_hInMem;   /* for input program to protect */
    DWORD m_fSize;
    
    BYTE* m_vmImage;   /* image for VM loader */
    DWORD m_vmCoreSize;

  //char  m_loaderAlloc[];

    BYTE  opcodeTab[VM_INSTR_COUNT]; //vm opcode table
    BYTE  condTab[16];               //jump table

public:
    int   m_errno;

private:
    int   get_block_ranges(HWND listBox, IMAGE_NT_HEADERS* inh, IMAGE_SECTION_HEADER*  ish,DWORD* &RVA_ranges, DWORD* &Raw_offset_sizes);

public:
    MyObfuscator(char* fileName);
    ~MyObfuscator();

    int   vm_init(BYTE* &retMem, DWORD &_vmInit, DWORD &_vmStart);   // use reference instead for retMem
    int   vm_protect(BYTE* codeBase, int codeSize, BYTE* outCodeBuf, DWORD inExeFuncRVA, BYTE* relocBuf, DWORD imgBase);
    int   vm_protect_vm(BYTE* vm_in_exe, BYTE* outBuf, DWORD imgBase, DWORD vmRVA, DWORD newRVA);
    void  do_protect(HWND listBox, bool vmovervm);

};


#endif