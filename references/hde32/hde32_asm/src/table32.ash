
; Hacker Disassembler Engine 32
; Copyright (c) 2006-2009, Vyacheslav Patkov.
; All rights reserved.

%define delta_o opcode_flags-opcode_map
%define delta_o2 opcode_flags-opcode2_map
%define delta_g opcode_groups-opcode_map
%define delta_g2 opcode2_groups-opcode2_map
%define delta_p2 opcode2_prefix_flags-opcode2_prefix_map

hde32_table:

opcode_map:
  db delta_o+13 ; 00..03
  db delta_o+18 ; 04..07
  db delta_o+13 ; 08..0b
  db delta_o+18 ; 0c..0f
  db delta_o+13 ; 10..13
  db delta_o+18 ; 14..17
  db delta_o+13 ; 18..1b
  db delta_o+18 ; 1c..1f
  db delta_o+13 ; 20..23
  db delta_o+18 ; 24..27
  db delta_o+13 ; 28..2b
  db delta_o+18 ; 2c..2f
  db delta_o+13 ; 30..33
  db delta_o+18 ; 34..37
  db delta_o+13 ; 38..3b
  db delta_o+18 ; 3c..3f
  db delta_o+20 ; 40..43
  db delta_o+20 ; 44..47
  db delta_o+20 ; 48..4b
  db delta_o+20 ; 4c..4f
  db delta_o+20 ; 50..53
  db delta_o+20 ; 54..57
  db delta_o+20 ; 58..5b
  db delta_o+20 ; 5c..5f
  db delta_o+22 ; 60..63
  db delta_o+20 ; 64..67
  db delta_o+28 ; 68..6b
  db delta_o+20 ; 6c..6f
  db delta_o+9  ; 70..73
  db delta_o+9  ; 74..77
  db delta_o+9  ; 78..7b
  db delta_o+9  ; 7c..7f
  db delta_o+31 ; 80..83
  db delta_o+13 ; 84..87
  db delta_o+13 ; 88..8b
  db delta_o+14 ; 8c..8f
  db delta_o+20 ; 90..93
  db delta_o+20 ; 94..97
  db delta_o+36 ; 98..9b
  db delta_o+20 ; 9c..9f
  db delta_o    ; a0..a3
  db delta_o+20 ; a4..a7
  db delta_o+18 ; a8..ab
  db delta_o+20 ; ac..af
  db delta_o+45 ; b0..b3
  db delta_o+45 ; b4..b7
  db delta_o    ; b8..bb
  db delta_o    ; bc..bf
  db delta_o+33 ; c0..c3
  db delta_o+24 ; c4..c7
  db delta_o+64 ; c8..cb
  db delta_o+39 ; cc..cf
  db delta_o+13 ; d0..d3
  db delta_o+47 ; d4..d7
  db delta_o+13 ; d8..db
  db delta_o+13 ; dc..df
  db delta_o+9  ; e0..e3
  db delta_o+45 ; e4..e7
  db delta_o+6  ; e8..eb
  db delta_o+20 ; ec..ef
  db delta_o+20 ; f0..f3
  db delta_o+22 ; f4..f7
  db delta_o+20 ; f8..fb
  db delta_o+41 ; fc..ff

opcode_groups:
  db C_MODRM | C_IMM8, 01111111b    ; c6 (group 12)
  db C_MODRM | C_IMM_P66, 01111111b ; c7 (group 12)
  db C_MODRM, 01111111b             ; 8f (group 10)
  db C_MODRM, 00111111b             ; fe (group 4)
  db C_MODRM, 00000001b             ; ff (group 5)

opcode2_map:
  db delta_o2+68 ; 0f 00..03
  db delta_o2+54 ; 0f 04..07
  db delta_o2+49 ; 0f 08..0b
  db delta_o2+75 ; 0f 0c..0f
  db delta_o2+13 ; 0f 10..13
  db delta_o2+13 ; 0f 14..17
  db delta_o2+13 ; 0f 18..1b
  db delta_o2+13 ; 0f 1c..1f
  db delta_o2+13 ; 0f 20..23
  db delta_o2+51 ; 0f 24..27
  db delta_o2+13 ; 0f 28..2b
  db delta_o2+13 ; 0f 2c..2f
  db delta_o2+20 ; 0f 30..33
  db delta_o2+49 ; 0f 34..37
  db delta_o2+51 ; 0f 38..3b
  db delta_o2+51 ; 0f 3c..3f
  db delta_o2+13 ; 0f 40..43
  db delta_o2+13 ; 0f 44..47
  db delta_o2+13 ; 0f 48..4b
  db delta_o2+13 ; 0f 4c..4f
  db delta_o2+13 ; 0f 50..53
  db delta_o2+13 ; 0f 54..57
  db delta_o2+13 ; 0f 58..5b
  db delta_o2+13 ; 0f 5c..5f
  db delta_o2+13 ; 0f 60..63
  db delta_o2+13 ; 0f 64..67
  db delta_o2+13 ; 0f 68..6b
  db delta_o2+13 ; 0f 6c..6f
  db delta_o2+78 ; 0f 70..73
  db delta_o2+60 ; 0f 74..77
  db delta_o2+49 ; 0f 78..7b
  db delta_o2+13 ; 0f 7c..7f
  db delta_o2+4  ; 0f 80..83
  db delta_o2+4  ; 0f 84..87
  db delta_o2+4  ; 0f 88..8b
  db delta_o2+4  ; 0f 8c..8f
  db delta_o2+13 ; 0f 90..93
  db delta_o2+13 ; 0f 94..97
  db delta_o2+13 ; 0f 98..9b
  db delta_o2+13 ; 0f 9c..9f
  db delta_o2+21 ; 0f a0..a3
  db delta_o2+72 ; 0f a4..a7
  db delta_o2+21 ; 0f a8..ab
  db delta_o2+82 ; 0f ac..af
  db delta_o2+13 ; 0f b0..b3
  db delta_o2+13 ; 0f b4..b7
  db delta_o2+57 ; 0f b8..bb
  db delta_o2+13 ; 0f bc..bf
  db delta_o2+70 ; 0f c0..c3
  db delta_o2+87 ; 0f c4..c7
  db delta_o2+20 ; 0f c8..cb
  db delta_o2+20 ; 0f cc..cf
  db delta_o2+13 ; 0f d0..d3
  db delta_o2+13 ; 0f d4..d7
  db delta_o2+13 ; 0f d8..db
  db delta_o2+13 ; 0f dc..df
  db delta_o2+13 ; 0f e0..e3
  db delta_o2+13 ; 0f e4..e7
  db delta_o2+13 ; 0f e8..eb
  db delta_o2+13 ; 0f ec..ef
  db delta_o2+13 ; 0f f0..f3
  db delta_o2+13 ; 0f f4..f7
  db delta_o2+13 ; 0f f8..fb
  db delta_o2+83 ; 0f fc..ff

opcode2_groups:
  db C_MODRM, 00000011b          ; 0f 00 (group 6)
  db C_MODRM, 00000100b          ; 0f 01 (group 7)
  db C_MODRM | C_IMM8, 11010101b ; 0f 71,72 (group 13)
  db C_MODRM | C_IMM8, 11001100b ; 0f 73 (group 15)
  db C_MODRM, 10111100b          ; 0f c7 (group 9)
  db C_MODRM | C_IMM8, 11110000b ; 0f ba (group 8)

opcode_flags:
  db C_IMM_P66             ; 0
  db C_IMM_P66             ; 1
  db C_IMM_P66             ; 2
  db C_IMM_P66             ; 3
  db C_IMM_P66 | C_REL32   ; 4
  db C_IMM_P66 | C_REL32   ; 5
  db C_IMM_P66 | C_REL32   ; 6
  db C_IMM_P66 | C_REL32   ; 7
  db C_IMM16 | C_IMM_P66   ; 8
  db C_REL8                ; 9
  db C_REL8                ; 10
  db C_REL8                ; 11
  db C_REL8                ; 12
  db C_MODRM               ; 13
  db C_MODRM               ; 14
  db C_MODRM               ; 15
  db C_MODRM               ; 16
  db C_GROUP | delta_g+4   ; 17
  db C_IMM8                ; 18
  db C_IMM_P66             ; 19
  db C_NONE                ; 20
  db C_NONE                ; 21
  db C_NONE                ; 22
  db C_NONE                ; 23
  db C_MODRM               ; 24
  db C_MODRM               ; 25
  db C_GROUP | delta_g     ; 26
  db C_GROUP | delta_g+2   ; 27
  db C_IMM_P66             ; 28
  db C_MODRM | C_IMM_P66   ; 29
  db C_IMM8                ; 30
  db C_MODRM | C_IMM8      ; 31
  db C_MODRM | C_IMM_P66   ; 32
  db C_MODRM | C_IMM8      ; 33
  db C_MODRM | C_IMM8      ; 34
  db C_IMM16               ; 35
  db C_NONE                ; 36
  db C_NONE                ; 37
  db C_IMM16 | C_IMM_P66   ; 38
  db C_NONE                ; 39
  db C_IMM8                ; 40
  db C_NONE                ; 41
  db C_NONE                ; 42
  db C_GROUP | delta_g+6   ; 43
  db C_GROUP | delta_g+8   ; 44
  db C_IMM8                ; 45
  db C_IMM8                ; 46
  db C_IMM8                ; 47
  db C_IMM8                ; 48
  db C_NONE                ; 49
  db C_NONE                ; 50
  db C_ERROR               ; 51
  db C_ERROR               ; 52
  db C_ERROR               ; 53
  db C_ERROR               ; 54
  db C_NONE                ; 55
  db C_NONE                ; 56
  db C_NONE                ; 57
  db C_ERROR               ; 58
  db C_GROUP | delta_g2+10 ; 59
  db C_MODRM               ; 60
  db C_MODRM               ; 61
  db C_MODRM               ; 62
  db C_NONE                ; 63
  db C_IMM8 | C_IMM16      ; 64
  db C_NONE                ; 65
  db C_IMM16               ; 66
  db C_NONE                ; 67
  db C_GROUP | delta_g2    ; 68
  db C_GROUP | delta_g2+2  ; 69
  db C_MODRM               ; 70
  db C_MODRM               ; 71
  db C_MODRM | C_IMM8      ; 72
  db C_MODRM               ; 73
  db C_ERROR               ; 74
  db C_ERROR               ; 75
  db C_MODRM               ; 76
  db C_NONE                ; 77
  db C_MODRM | C_IMM8      ; 78
  db C_GROUP | delta_g2+4  ; 79
  db C_GROUP | delta_g2+4  ; 80
  db C_GROUP | delta_g2+6  ; 81
  db C_MODRM | C_IMM8      ; 82
  db C_MODRM               ; 83
  db C_MODRM               ; 84
  db C_MODRM               ; 85
  db C_ERROR               ; 86
  db C_MODRM | C_IMM8      ; 87
  db C_MODRM | C_IMM8      ; 88
  db C_MODRM | C_IMM8      ; 89
  db C_GROUP | delta_g2+8  ; 90

fpu_reg:
  db 01000000b ; d9
  db 0         ; da
  db 00001010b ; db
  db 0         ; dc
  db 00000100b ; dd
  db 0         ; de
  db 0         ; df

fpu_modrm:
  db 0,0,01111111b,0,00110011b,00000001b,0,0 ; d9
  db 0,0,0,0,0xff,10111111b,0xff,0xff        ; da
  db 0,0,0,0,00000111b,0,0,0xff              ; db
  times 8 db 0                               ; dc
  db 0,0,0,0,0,0,0xff,0xff                   ; dd
  db 0,0,0,10111111b,0,0,0,0                 ; de
  db 0,0,0,0,01111111b,0,0,0xff              ; df

opcode2_prefix_map:
  db delta_p2+10 ; 0f 00..03
  db delta_p2+10 ; 0f 04..07
  db delta_p2+10 ; 0f 08..0b
  db delta_p2+10 ; 0f 0c..0f
  db delta_p2+11 ; 0f 10..13
  db delta_p2+18 ; 0f 14..17
  db delta_p2+10 ; 0f 18..1b
  db delta_p2+10 ; 0f 1c..1f
  db delta_p2+10 ; 0f 20..23
  db delta_p2+10 ; 0f 24..27
  db delta_p2+15 ; 0f 28..2b
  db delta_p2+12 ; 0f 2c..2f
  db delta_p2+10 ; 0f 30..33
  db delta_p2+10 ; 0f 34..37
  db delta_p2+10 ; 0f 38..3b
  db delta_p2+10 ; 0f 3c..3f
  db delta_p2+10 ; 0f 40..43
  db delta_p2+10 ; 0f 44..47
  db delta_p2+10 ; 0f 48..4b
  db delta_p2+10 ; 0f 4c..4f
  db delta_p2+21 ; 0f 50..53
  db delta_p2+5  ; 0f 54..57
  db delta_p2    ; 0f 58..5b
  db delta_p2+10 ; 0f 5c..5f
  db delta_p2+10 ; 0f 60..63
  db delta_p2+10 ; 0f 64..67
  db delta_p2+5  ; 0f 68..6b
  db delta_p2+25 ; 0f 6c..6f
  db delta_p2+13 ; 0f 70..73
  db delta_p2+6  ; 0f 74..77
  db delta_p2+10 ; 0f 78..7b
  db delta_p2+29 ; 0f 7c..7f
  db delta_p2+10 ; 0f 80..83
  db delta_p2+10 ; 0f 84..87
  db delta_p2+10 ; 0f 88..8b
  db delta_p2+10 ; 0f 8c..8f
  db delta_p2+10 ; 0f 90..93
  db delta_p2+10 ; 0f 94..97
  db delta_p2+10 ; 0f 98..9b
  db delta_p2+10 ; 0f 9c..9f
  db delta_p2+10 ; 0f a0..a3
  db delta_p2+10 ; 0f a4..a7
  db delta_p2+10 ; 0f a8..ab
  db delta_p2+10 ; 0f ac..af
  db delta_p2+10 ; 0f b0..b3
  db delta_p2+10 ; 0f b4..b7
  db delta_p2+33 ; 0f b8..bb
  db delta_p2+35 ; 0f bc..bf
  db delta_p2+39 ; 0f c0..c3
  db delta_p2+14 ; 0f c4..c7
  db delta_p2+10 ; 0f c8..cb
  db delta_p2+10 ; 0f cc..cf
  db delta_p2+43 ; 0f d0..d3
  db delta_p2+45 ; 0f d4..d7
  db delta_p2+10 ; 0f d8..db
  db delta_p2+10 ; 0f dc..df
  db delta_p2+5  ; 0f e0..e3
  db delta_p2+45 ; 0f e4..e7
  db delta_p2+10 ; 0f e8..eb
  db delta_p2+10 ; 0f ec..ef
  db delta_p2+4  ; 0f f0..f3
  db delta_p2+5  ; 0f f4..f7
  db delta_p2+10 ; 0f f8..fb
  db delta_p2+10 ; 0f fc..ff

opcode2_prefix_flags:
  db PRE_ANY                    ; 0
  db PRE_ANY                    ; 1
  db PRE_ANY                    ; 2
  db PRE_F2                     ; 3
  db PRE_NONE | PRE_F3 | PRE_66 ; 4
  db PRE_F2 | PRE_F3            ; 5
  db PRE_F2 | PRE_F3            ; 6
  db PRE_F2 | PRE_F3            ; 7
  db PRE_F2 | PRE_F3            ; 8
  db PRE_F2 | PRE_F3 | PRE_66   ; 9
  db PRE_ANY                    ; 10
  db PRE_ANY                    ; 11
  db PRE_ANY                    ; 12
  db PRE_ANY                    ; 13
  db PRE_F2 | PRE_F3            ; 14
  db PRE_F2 | PRE_F3            ; 15
  db PRE_F2 | PRE_F3            ; 16
  db PRE_ANY                    ; 17
  db PRE_F2 | PRE_F3            ; 18
  db PRE_F2 | PRE_F3            ; 19
  db PRE_F2                     ; 20
  db PRE_F2 | PRE_F3            ; 21
  db PRE_ANY                    ; 22
  db PRE_F2 | PRE_66            ; 23
  db PRE_F2 | PRE_66            ; 24
  db PRE_NONE | PRE_F2 | PRE_F3 ; 25
  db PRE_NONE | PRE_F2 | PRE_F3 ; 26
  db PRE_F2 | PRE_F3            ; 27
  db PRE_F2                     ; 28
  db PRE_NONE | PRE_F3          ; 29
  db PRE_NONE | PRE_F3          ; 30
  db PRE_F2                     ; 31
  db PRE_F2                     ; 32
  db PRE_ANY                    ; 33
  db PRE_ANY                    ; 34
  db PRE_F3                     ; 35
  db PRE_F3                     ; 36
  db PRE_F3                     ; 37
  db PRE_F3                     ; 38
  db PRE_ANY                    ; 39
  db PRE_ANY                    ; 40
  db PRE_ANY                    ; 41
  db PRE_F2 | PRE_F3 | PRE_66   ; 42
  db PRE_NONE | PRE_F3          ; 43
  db PRE_F2 | PRE_F3            ; 44
  db PRE_F2 | PRE_F3            ; 45
  db PRE_F2 | PRE_F3            ; 46
  db PRE_NONE                   ; 47
  db PRE_F2 | PRE_F3            ; 48

opcode_lock_ok:
  db 0x00,0         ; 00,01
  db 0x08,0         ; 08,09
  db 0x10,0         ; 10,11
  db 0x18,0         ; 18,19
  db 0x20,0         ; 20,21
  db 0x28,0         ; 28,29
  db 0x30,0         ; 30,31
  db 0x80,00000001b ; 80,81 /0../6
  db 0x82,00000001b ; 82,83 /0../6
  db 0x86,0         ; 86,87
  db 0xf6,11001111b ; f6,f7 /2/3
  db 0xfe,00111111b ; fe,ff /0/1

opcode2_lock_ok:
  db 0xab,0         ; 0f ab
  db 0xb0,0         ; 0f b0
  db 0xb1,0         ; 0f b1
  db 0xb3,0         ; 0f b3
  db 0xba,11111000b ; 0f ba /5/6/7
  db 0xbb,0         ; 0f bb
  db 0xc0,0         ; 0f c0
  db 0xc1,0         ; 0f c1
  db 0xc7,10111111b ; 0f c7 /1

opcode_only_memory:
  db 0x62,PRE_ALL,0         ; 62
  db 0x8d,PRE_ALL,0         ; 8d
  db 0xc4,PRE_ALL,0         ; c4
  db 0xc5,PRE_ALL,0         ; c5
  db 0xff,PRE_ALL,11101011b ; ff /3/5

opcode2_only_memory:
  db 0x01,PRE_ALL,00001110b   ; 0f 01 /0/1/2/3/7
  db 0x12,PRE_66,0            ; 66 0f 12
  db 0x13,PRE_NONE | PRE_66,0 ; none|66 0f 12
  db 0x16,PRE_66,0            ; 66 0f 16
  db 0x17,PRE_NONE | PRE_66,0 ; none|66 0f 17
  db 0x2b,PRE_NONE | PRE_66,0 ; none|66 0f 2b
  db 0xae,PRE_ALL,00000111b   ; 0f ae /0/1/2/3/4
  db 0xb2,PRE_ALL,0           ; 0f b2
  db 0xb4,PRE_ALL,0           ; 0f b4
  db 0xb5,PRE_ALL,0           ; 0f b5
  db 0xc3,PRE_NONE,0          ; none 0f c3
  db 0xc7,PRE_ALL,10111111b   ; 0f c7 /1
  db 0xe7,PRE_66,0            ; 66 0f e7
  db 0xf0,PRE_F2,0            ; f2 0f f0

hde32_table_end:
