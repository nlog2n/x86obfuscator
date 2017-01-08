; masm/win32 example

.386
.model flat,stdcall
option casemap:none

include c:\masm32\include\kernel32.inc
include c:\masm32\include\user32.inc
includelib c:\masm32\lib\kernel32.lib
includelib c:\masm32\lib\user32.lib

include hde32.masm
includelib hde32.a

.data

hs hde32s <>
buf db "Invalid instruction !",100 dup (0)
caption db "hde32 assembler example",0
format db "cmp dword [edi+0x12],0x12345678",13,10,13,10
       db "    lenght:  0x%02x",13,10
       db "    imm32:   0x%08x",13,10
       db "    disp8:   0x%02x",0

.code

start:
        push    offset hs
        push    offset test_code
        call    hde32_disasm
        add     esp,8

        test    dword ptr [hs.flags],F_ERROR
        jnz     display_result

        movzx   ebx,byte ptr [hs.disp.disp8]
        push    ebx
        push    dword ptr [hs.imm.imm32]
        push    eax
        push    offset format
        push    offset buf
        call    wsprintf
        add     esp,20

      display_result:
        push    0
        push    offset caption
        push    offset buf
        push    0
        call    MessageBox

        push    0
        call    ExitProcess

      test_code:
        cmp     dword ptr [edi+12h],12345678h

end start
