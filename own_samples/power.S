extern printf
global main
global cpower

section .data
text_format db "Result is: %d", 10, 0
section .text
main:
  push ebp
  mov ebp, esp
  sub esp, 16

  pushad
  push dword 3 ;exponent
  push dword 10 ;base
  call cpower
  add esp, 8
  mov dword [ebp-4], eax
  popad

  pushad
  push dword [ebp-4]
  push dword text_format
  call printf
  add esp, 8
  popad

  leave
  ret

cpower:
  push ebp
  mov ebp, esp
  sub esp, 16
  mov [ebp-4], dword 1 ;result = 1

while_0_begin:
  cmp dword [ebp+12], 0 ;exponent == 0 ?
  je while_0_end ;yes, then get out
  nop
  mov eax, dword [ebp-4] ;result
  mov ebx, dword [ebp+8] ;base
  mul ebx ;result * base
  mov dword [ebp-4], eax ;result = result * base
  mov ebx, dword [ebp+12] ;exponent
  sub ebx, 1  ; exponent - 1
  mov dword [ebp+12], ebx ;exponent = exponent - 1
  jmp while_0_begin
  nop
while_0_end:

  mov eax, dword [ebp-4] ;return result;
  leave
  ret
