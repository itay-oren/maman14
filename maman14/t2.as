; This file contains various macr-related errors


; Error: Macr name that's a reserved word
macr mov
sub r3, r4
endmacr

; verylonglabelname: mov r1, r2 ; This comment makes the line exceed the maximum allowed length for a single line in the assembly language


; Error: Macr redefinition
macr duplicatemacr
mov r1, r2
endmacr

macr duplicatemacr
add r3, #5
endmacr

; Error: Macr with directive name
macr .data
mov r1, #5
endmacr

; Valid macr definition (for contrast)
macr validmacr
mov r1, #5
inc r2
endmacr

; Error: Incorrect macr usage (extra parameters)
validmacr r1, r2