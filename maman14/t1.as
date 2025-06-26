; This is a comprehensive example covering various legal cases

; Macro definition
macr macrexample
inc r2
mov STR, r1
endmacr

.extern EX
MAIN:   mov r3, LENGTH
LOOP:   clr K
macrexample
cmp r3, #-5
bne NOTEQUAL

mov r1, r2
add #5, STR
sub r1, r4
lea ARRAY, r6

clr K
not r2
inc r1
dec r0
inc EX
jmp ENDLOOP
bne LOOP
red r5
prn #48

jsr SUBROUTINE
rts

NOTEQUAL: mov *r3, r2
cmp *r3, #-12
add r4, *r2

ENDLOOP: stop

SUBROUTINE: mov #5, r1
prn #-12
jsr NESTED
rts

NESTED: inc r1
bne SUBROUTINE
rts

STR:    .string "Hello, World!"
LENGTH: .data 13, 27, -5, 1048
ARRAY:  .data 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
K:      .data 0

    .entry MAIN
    .entry LOOP
    .extern EXTERNALLABEL

