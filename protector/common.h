
#ifndef _COMMON_H_
#define _COMMON_H_

#include <windows.h>
#include "hde.h"


DWORD WINAPI _lde(BYTE* off);
//extern "C" DWORD WINAPI lde(BYTE* off);
#define lde _lde

typedef void (__stdcall *polyFunc)(BYTE* buf, DWORD size, DWORD pos);
extern polyFunc polyEnc;
extern polyFunc polyDec;
extern BYTE _vm_poly_dec[121];

int  genCodeMap(BYTE* codeBase, int codeSize, DWORD* codeMap);
void genPolyEncDec();
void genPermutation(BYTE* buf, int size);
void invPerm256(BYTE* buf);
void invPerm16(BYTE* buf);
void permutateJcc(WORD* buf, int elemCount, BYTE* permutation);

#endif