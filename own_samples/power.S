extern printf
global main
global cpower
global printbool

section .data
text_format db "Result is: %d", 10, 0
dec_format db "Dec: %d", 10, 0
true_format db "true", 0
false_format db "false", 0
section .text
main:
  push ebp
  mov ebp, esp
  sub esp, 16

  mov eax, dword -1
  mov ebx, dword 10
  xor eax, ebx
  mov ebx, eax
  push dword ebx
  push dword dec_format
  call printf
  add esp, 8

  mov ebx, dword -8
  sar ebx, 3
  push dword ebx
  push dword dec_format
  call printf
  add esp, 8

  push dword 3 ;exponent
  push dword 10 ;base
  call cpower
  add esp, 8
  mov dword [ebp-4], eax

  push dword [ebp-4]
  push dword text_format
  call printf
  add esp, 8

  push dword 1
  call printbool
  add esp, 4

  push dword 0
  call printbool
  add esp, 4

  leave
  ret

printbool:
  push ebp
  mov ebp, esp

printbool_if_begin:
  cmp dword [ebp+8], 1
  jne printbool_if_else
  nop

  push dword true_format
  call printf
  add esp, 4

  jmp printbool_if_end
  nop
printbool_if_else:
  push dword false_format
  call printf
  add esp, 4

printbool_if_end:
  leave
  ret

cpower:
  push ebp
  mov ebp, esp
  sub esp, 16
  mov [ebp-4], dword 1 ;result = 1

cpower_while_begin:
  cmp dword [ebp+12], 0 ;exponent == 0 ?
  je cpower_while_end ;yes, then get out
  nop
  mov eax, dword [ebp-4] ;result
  mov ebx, dword [ebp+8] ;base
  mul ebx ;result * base
  mov dword [ebp-4], eax ;result = result * base
  mov ebx, dword [ebp+12] ;exponent
  sub ebx, 1  ; exponent - 1
  mov dword [ebp+12], ebx ;exponent = exponent - 1
  jmp cpower_while_begin
  nop
cpower_while_end:

  mov eax, dword [ebp-4] ;return result;
  leave
  ret
