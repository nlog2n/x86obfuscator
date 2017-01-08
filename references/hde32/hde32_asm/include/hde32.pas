(*
 * Hacker Disassembler Engine 32
 * Copyright (c) 2006-2009, Vyacheslav Patkov.
 * All rights reserved.
 *
 * hde32.pas: Pascal header file (tnx: Mike)
 *
 *)

unit hde32;

interface

const
  F_MODRM         = $00000001;
  F_SIB           = $00000002;
  F_IMM8          = $00000004;
  F_IMM16         = $00000008;
  F_IMM32         = $00000010;
  F_DISP8         = $00000020;
  F_DISP16        = $00000040;
  F_DISP32        = $00000080;
  F_RELATIVE      = $00000100;
  F_2IMM16        = $00000800;
  F_ERROR         = $00001000;
  F_ERROR_OPCODE  = $00002000;
  F_ERROR_LENGTH  = $00004000;
  F_ERROR_LOCK    = $00008000;
  F_ERROR_OPERAND = $00010000;
  F_PREFIX_REPNZ  = $01000000;
  F_PREFIX_REPX   = $02000000;
  F_PREFIX_REP    = $03000000;
  F_PREFIX_66     = $04000000;
  F_PREFIX_67     = $08000000;
  F_PREFIX_LOCK   = $10000000;
  F_PREFIX_SEG    = $20000000;
  F_PREFIX_ANY    = $3f000000;

  PREFIX_SEGMENT_CS   = $2e;
  PREFIX_SEGMENT_SS   = $36;
  PREFIX_SEGMENT_DS   = $3e;
  PREFIX_SEGMENT_ES   = $26;
  PREFIX_SEGMENT_FS   = $64;
  PREFIX_SEGMENT_GS   = $65;
  PREFIX_LOCK         = $f0;
  PREFIX_REPNZ        = $f2;
  PREFIX_REPX         = $f3;
  PREFIX_OPERAND_SIZE = $66;
  PREFIX_ADDRESS_SIZE = $67;

type
  hde32s = packed record
    len: byte;
    p_rep: byte;
    p_lock: byte;
    p_seg: byte;
    p_66: byte;
    p_67: byte;
    opcode: byte;
    opcode2: byte;
    modrm: byte;
    modrm_mod: byte;
    modrm_reg: byte;
    modrm_rm: byte;
    sib: byte;
    sib_scale: byte;
    sib_index: byte;
    sib_base: byte;
    imm: packed record case byte of
      0: (imm8: byte);
      1: (imm16: word);
      2: (imm32: longword);
    end;
    disp: packed record case byte of
      0: (disp8: byte);
      1: (disp16: word);
      2: (disp32: longword);
    end;
    flags: longword;
  end;

function hde32_disasm(code: pointer; var hs: hde32s): longword; cdecl;

implementation

function hde32_disasm(code: pointer; var hs: hde32s): longword; cdecl; external;
{$L hde32.obj}

end.
