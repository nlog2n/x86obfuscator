(* Delphi example by Mike *)

program example;
{$APPTYPE CONSOLE}

uses
  hde32;
const
  code: packed array[0..6] of byte = ($81,$7f,$12,$78,$56,$34,$12);
var
  len: longword;
  hs: hde32s;

function long2hex(a: longint): string;
const
  digit: packed array[0..$f] of char = '0123456789abcdef';
var
  i: byte;
begin
  for i := 1 to 8 do
  begin
    insert(digit[a and $f],result,1);
    a := a shr 4;
  end;
end;

begin
  len := hde32_disasm(@code[0],hs);
  if (hs.flags and F_ERROR) <> 0 then
    writeln('Invalid instruction !')
  else
    writeln('cmp dword [edi+0x12],0x12345678',
             #13#10'  length of command: ',long2hex(len),
             #13#10'  immediate32:       ',long2hex(hs.imm.imm32),
             #13#10'  displacement8:     ',long2hex(hs.disp.disp8));
  readln;
end.
