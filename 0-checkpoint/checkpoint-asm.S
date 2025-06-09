#include "rpi-asm.h"

@ trampoline used to:
@   - save registers
@   - save stack
@   - save heap
@   - 
@  note: also have to save/restore lr
MK_FN(checkpoint)
    push {r4-r11}
    push {lr}
    mov r0, sp
    mrs r1, cpsr
    bl checkpoint_fn
    pop {lr}
    pop {r4-r11}
    bx lr
