
# Hacker Disassembler Engine 32
# Copyright (c) 2006-2009, Vyacheslav Patkov.
# All rights reserved.

# Makefile for GNU make

AS = nasm     # or yasm
AFLAGS = -Ox  # or "-O999" for old nasm/yasm
format = elf  # see "nasm -hf" or "yasm -f help"

all:
	$(AS) $(AFLAGS) -f $(format) hde32.asm

.PHONY: all
