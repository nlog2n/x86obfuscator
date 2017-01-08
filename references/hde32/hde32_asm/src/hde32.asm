
; Hacker Disassembler Engine 32
; Copyright (c) 2006-2009, Vyacheslav Patkov.
; All rights reserved.

cpu 386
bits 32

%ifidni __OUTPUT_FORMAT__,obj
  section _TEXT class=CODE use32 FLAT
%else
  section .text
%endif

%macro globals 1
  global %1
  global _%1
  %1:
  _%1:
%endmacro

%include "hde32.ash"

globals hde32_disasm
        pushad
        mov     edi,[esp+40]
        xor     eax,eax
        xor     ecx,ecx
        xor     ebx,ebx
        cdq
        mov     cl,hde32s_size
        rep     stosb
        sub     edi,hde32s_size
        mov     esi,[esp+36]
        mov     cl,16
      prefixes:
        lodsb
        dec     ecx
        jz      prefixes_done
        cmp     al,0xf0
        jnz     no_prefix_lock
        or      dl,PRE_LOCK
        mov     [edi+hde32s.p_lock],al
        jmp     prefixes
      no_prefix_lock:
        mov     ah,al
        and     al,-2
        cmp     al,0xf2
        jnz     no_prefix_rep
        mov     [edi+hde32s.p_rep],ah
        sub     ah,al
        inc     ah
        shl     ah,1
        or      dl,ah
        jmp     prefixes
      no_prefix_rep:
        cmp     al,0x64
        jz      prefix_seg
        mov     al,ah
        and     al,0xe7
        cmp     al,0x26
        jnz     no_prefix_seg
      prefix_seg:
        or      dl,PRE_SEG
        mov     [edi+hde32s.p_seg],ah
        jmp     prefixes
      no_prefix_seg:
        mov     al,ah
        cmp     al,0x66
        jnz     no_prefix_66
        or      dl,PRE_66
        mov     [edi+hde32s.p_66],al
        jmp     prefixes
      no_prefix_66:
        cmp     al,0x67
        jnz     prefixes_done
        or      dl,PRE_67
        mov     [edi+hde32s.p_67],al
        jmp     prefixes
      prefixes_done:
        test    dl,dl
        jnz     ok_prefix
        or      dl,PRE_NONE
      ok_prefix:
        mov     ah,dl
        shr     ah,1
        mov     [edi+hde32s.flags+3],ah

        xor     ah,ah
        mov     [edi+hde32s.opcode],al
        call    get_delta
      get_delta:
        pop     ecx
        add     ecx,hde32_table-get_delta
        push    ecx
        cmp     al,0x0f
        jnz     no_opcode2
        lodsb
        mov     [edi+hde32s.opcode2],al
        add     ecx,opcode2_map-opcode_map
        jmp     get_flags
      no_opcode2:
        cmp     al,0xa0
        jb      get_flags
        cmp     al,0xa3
        ja      get_flags
        test    dl,PRE_67
        jz      a0_a3_no_p67
        or      dl,PRE_66
        jmp     get_flags
      a0_a3_no_p67:
        and     dl,~PRE_66

      get_flags:
        mov     bh,al
        mov     dh,al
        and     dh,3
        shr     al,2
        mov     al,[ecx+eax]
        add     al,dh
        mov     al,[ecx+eax]
        cmp     al,C_ERROR
        jnz     no_error_opcode
        or      byte [edi+hde32s.flags+1],(F_ERROR | F_ERROR_OPCODE) >> 8
        xor     al,al
        mov     bl,bh
        and     bl,-3
        cmp     bl,0x24
        jnz     flags_done
        inc     eax
        jmp     flags_done
      no_error_opcode:
        test    al,C_GROUP
        jz      flags_done
        and     al,0x7f
        mov     ax,[ecx+eax]
      flags_done:
        mov     cl,al

        cmp     byte [edi+hde32s.opcode2],0
        jz      no_prefix_error
        push    eax
        push    ecx
        mov     ecx,[esp+8]
        add     ecx,opcode2_prefix_map-hde32_table
        mov     al,bh
        mov     bl,bh
        xor     ah,ah
        and     bl,3
        shr     al,2
        mov     al,[ecx+eax]
        add     al,bl
        mov     al,[ecx+eax]
        test    dl,al
        pop     ecx
        pop     eax
        jz      no_prefix_error
        or      byte [edi+hde32s.flags+1],(F_ERROR | F_ERROR_OPCODE) >> 8
      no_prefix_error:

        test    cl,C_MODRM
        jnz     modrm
        test    dl,PRE_LOCK
        jz      no_modrm_and_lock
        or      byte [edi+hde32s.flags+1],(F_ERROR | F_ERROR_LOCK) >> 8
      no_modrm_and_lock:
        jmp     immediates
      modrm:
        or      dword [edi+hde32s.flags],F_MODRM
        lodsb
        mov     [edi+hde32s.modrm],al
        mov     ch,al
        shr     ch,6
        mov     [edi+hde32s.modrm_mod],ch
        mov     dh,al
        and     dh,7
        mov     [edi+hde32s.modrm_rm],dh
        shl     al,2
        shr     al,5
        mov     [edi+hde32s.modrm_reg],al
        test    ah,ah
        jz      no_opcode_group
        xchg    al,cl
        shl     ah,cl
        xchg    al,cl
        test    ah,0x80
        jz      no_opcode_group
        or      byte [edi+hde32s.flags+1],(F_ERROR | F_ERROR_OPCODE) >> 8
      no_opcode_group:

        cmp     byte [edi+hde32s.opcode2],0
        jnz     no_fpu_d9_df
        cmp     bh,0xd9
        jb      no_fpu_d9_df
        cmp     bh,0xdf
        ja      no_fpu_d9_df
        push    ecx
        push    ebx
        shr     ebx,8
        sub     bl,0xd9
        cmp     ch,3
        mov     ecx,[esp+8]
        lea     ecx,[ecx+fpu_reg-hde32_table]
        jz      fpu_modrm_extension
        mov     bl,[ecx+ebx]
        mov     cl,al
        jmp     fpu_at_end
      fpu_modrm_extension:
        lea     ecx,[ecx+ebx*8+fpu_modrm-fpu_reg]
        mov     bl,[ecx+eax]
        mov     cl,dh
      fpu_at_end:
        shl     bl,cl
        test    bl,0x80
        jz      no_fpu_error
        or      byte [edi+hde32s.flags+1],(F_ERROR | F_ERROR_OPCODE) >> 8
      no_fpu_error:
        pop     ebx
        pop     ecx
      no_fpu_d9_df:

        test    dl,PRE_LOCK
        jz      no_lock_error
        push    ecx
        push    edx
        cmp     ch,3
        jz      lock_error
        mov     ecx,[esp+8]
        add     ecx,opcode_lock_ok-hde32_table
        mov     bl,bh
        cmp     byte [edi+hde32s.opcode2],0
        jz      lock_test_no_opcode2
        add     ecx,opcode2_lock_ok-opcode_lock_ok
        lea     edx,[ecx+opcode_only_memory-opcode2_lock_ok]
        jmp     lock_test_opcode
      lock_test_no_opcode2:
        and     bl,-2
        lea     edx,[ecx+opcode2_lock_ok-opcode_lock_ok]
      lock_test_opcode:
        cmp     [ecx],bl
        jz      lock_test_opcode_found
        inc     ecx
        inc     ecx
        cmp     ecx,edx
        jz      lock_error
        jmp     lock_test_opcode
      lock_test_opcode_found:
        mov     bl,[ecx+1]
        mov     cl,al
        shl     bl,cl
        test    bl,0x80
        jz      lock_test_at_end
      lock_error:
        or      byte [edi+hde32s.flags+1],(F_ERROR | F_ERROR_LOCK) >> 8
      lock_test_at_end:
        pop     edx
        pop     ecx
      no_lock_error:

        cmp     byte [edi+hde32s.opcode2],0
        jz      operand_test_registers_no_opcode2
        mov     bl,bh
        and     bl,-3
        cmp     bl,0x20
        jnz     operand_test_no_0f_20_22
        mov     ch,3
        cmp     al,1
        jz      operand_error_jmp
        cmp     al,4
        ja      operand_error_jmp
        jmp     no_operand_error_jmp
      operand_test_no_0f_20_22:
        cmp     bl,0x21
        jnz     operand_test_mod
        mov     ch,3
        cmp     al,4
        jz      operand_error_jmp
        cmp     al,5
        jz      operand_error_jmp
        jmp     no_operand_error_jmp
      operand_test_registers_no_opcode2:
        cmp     bh,0x8c
        jz      operand_test_8c
        cmp     bh,0x8e
        jnz     operand_test_mod
        cmp     al,1
        jz      operand_error_jmp
      operand_test_8c:
        cmp     al,5
        jna     no_operand_error_jmp

      operand_error_jmp:
        jmp     operand_error
      no_operand_error_jmp:
        jmp     no_operand_error

      operand_test_mod:
        cmp     ch,3
        jnz     operand_test_register_only
        push    ecx
        push    edx
        mov     ecx,[esp+8]
        add     ecx,opcode_only_memory-hde32_table
        mov     bl,dl
        cmp     byte [edi+hde32s.opcode2],0
        jz      operand_test_mem_only_no_opcode2
        add     ecx,opcode2_only_memory-opcode_only_memory
        lea     edx,[ecx+hde32_table_end-opcode2_only_memory]
        jmp     operand_test_mem_only
      operand_test_mem_only_no_opcode2:
        lea     edx,[ecx+opcode2_only_memory-opcode_only_memory]
      operand_test_mem_only:
        cmp     [ecx],bh
        jz      operand_test_mem_only_found
        add     ecx,3
        cmp     ecx,edx
        jz      operand_test_mem_only_at_end
        jmp     operand_test_mem_only
      operand_test_mem_only_found:
        test    bl,[ecx+1]
        jz      operand_test_mem_only_at_end
        mov     bl,[ecx+2]
        mov     cl,al
        shl     bl,cl
        test    bl,0x80
        jnz     operand_test_mem_only_at_end
      operand_error_mem_only:
        or      word [edi+hde32s.flags+1],(F_ERROR | F_ERROR_OPERAND) >> 8
      operand_test_mem_only_at_end:
        pop     edx
        pop     ecx
        jmp     no_operand_error

      operand_test_register_only:
        cmp     byte [edi+hde32s.opcode2],0
        jz      no_operand_error
        cmp     bh,0xc5
        jz      operand_error
        test    dl,PRE_NONE | PRE_66
        jz      operand_test_no_pre_none_or_66
        cmp     bh,0x50
        jz      operand_error
        cmp     bh,0xd7
        jz      operand_error
        cmp     bh,0xf7
        jz      operand_error
      operand_test_no_pre_none_or_66:
        test    dl,PRE_F2 | PRE_F3
        jz      no_operand_error
        cmp     bh,0xd6
        jnz     no_operand_error
      operand_error:
        or      word [edi+hde32s.flags+1],(F_ERROR | F_ERROR_OPERAND) >> 8
      no_operand_error:

        cmp     al,1
        lodsb
        ja      no_f6_f7_reg_0_or_1
        cmp     byte [edi+hde32s.opcode2],0
        jnz     no_f6_f7_reg_0_or_1
        cmp     bh,0xf6
        jnz     no_f6_reg_0_or_1
        or      cl,C_IMM8
      no_f6_reg_0_or_1:
        cmp     bh,0xf7
        jnz     no_f6_f7_reg_0_or_1
        or      cl,C_IMM_P66
      no_f6_f7_reg_0_or_1:
        xor     bh,bh
        test    ch,ch
        jnz     no_mod_is_0
        test    dl,PRE_67
        jz      mod_is_0_no_p67
        cmp     dh,6
        jnz     no_mod_is_0
        mov     bh,2
      mod_is_0_no_p67:
        cmp     dh,5
        jnz     no_mod_is_0
        mov     bh,4
      no_mod_is_0:
        cmp     ch,1
        jnz     no_mod_is_1
        mov     bh,1
      no_mod_is_1:
        cmp     ch,2
        jnz     no_mod_is_2
        mov     bh,2
        test    dl,PRE_67
        jnz     no_mod_is_2
        shl     bh,1
      no_mod_is_2:
        cmp     ch,3
        jz      displacements
        cmp     dh,4
        jnz     displacements
        test    dl,PRE_67
        jnz     displacements
        or      dword [edi+hde32s.flags],F_SIB
        inc     esi
        mov     [edi+hde32s.sib],al
        mov     ah,al
        shr     ah,6
        mov     [edi+hde32s.sib_scale],ah
        mov     ah,al
        shl     ah,2
        shr     ah,5
        mov     [edi+hde32s.sib_index],ah
        and     al,7
        mov     [edi+hde32s.sib_base],al
        cmp     al,5
        jnz     displacements
        test    ch,1
        jnz     displacements
        mov     bh,4

      displacements:
        dec     esi
        test    bh,bh
        jz      immediates
        xor     eax,eax
        cmp     bh,1
        jnz     no_disp8
        lodsb
      no_disp8:
        cmp     bh,2
        jnz     no_disp16
        lodsw
      no_disp16:
        cmp     bh,4
        jnz     dispxx_save
        lodsd
      dispxx_save:
        xor     bl,bl
        shr     ebx,3
        or      [edi+hde32s.flags],bl
        mov     [edi+hde32s.disp],eax
      immediates:
        test    cl,C_IMM_P66
        jz      no_imm_p66
        test    cl,C_REL32
        jz      no_relxx
        test    dl,PRE_66
        jz      ok_rel32
        or      dword [edi+hde32s.flags],F_IMM16 | F_RELATIVE
        lodsw
        mov     [edi+hde32s.imm],ax
        jmp     at_end
      no_relxx:
        xor     eax,eax
        test    dl,PRE_66
        jz      immxx_no_p66
        or      dword [edi+hde32s.flags],F_IMM16
        lodsw
        jmp     immxx_save
      immxx_no_p66:
        or      dword [edi+hde32s.flags],F_IMM32
        lodsd
      immxx_save:
        mov     [edi+hde32s.imm],eax
      no_imm_p66:
        test    cl,C_IMM16
        jz      no_imm16
        lodsw
        mov     bl,[edi+hde32s.flags]
        test    bl,F_IMM32
        jz      no_imm16_32
        or      dword [edi+hde32s.flags],F_IMM16
        jmp     ok_2immxx
      no_imm16_32:
        test    bl,F_IMM16
        jz      no_2imm16
        or      byte [edi+hde32s.flags+1],F_2IMM16 >> 8
      ok_2immxx:
        mov     [edi+hde32s.disp],ax
        jmp     no_imm16
      no_2imm16:
        or      dword [edi+hde32s.flags],F_IMM16
        mov     [edi+hde32s.imm],ax
      no_imm16:
        test    cl,C_IMM8
        jz      no_imm8
        or      dword [edi+hde32s.flags],F_IMM8
        lodsb
        mov     [edi+hde32s.imm],al
      no_imm8:
        test    cl,C_REL32
        jz      no_rel32
      ok_rel32:
        or      dword [edi+hde32s.flags],F_IMM32 | F_RELATIVE
        lodsd
        mov     [edi+hde32s.imm],eax
      no_rel32:
        test    cl,C_REL8
        jz      at_end
        or      dword [edi+hde32s.flags],F_IMM8 | F_RELATIVE
        lodsb
        mov     [edi+hde32s.imm],al

      at_end:
        pop     edx
        xchg    eax,esi
        sub     eax,[esp+36]
        cmp     al,15
        jbe     no_length_error
        or      byte [edi+hde32s.flags+1],(F_ERROR | F_ERROR_LENGTH) >> 8
        mov     al,15
      no_length_error:
        mov     [esp+28],eax
        stosb
        popad
        ret

%include "table32.ash"
