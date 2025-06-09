#ifndef __CRIU_H__
#define __CRIU_H__

#include <stdint.h>
#include "libc/helper-macros.h"
#include "rpi-constants.h"
#include "rpi.h"
#include "memmap.h"

typedef struct {
    uint32_t *lr;
    uint32_t *sp;
    uint32_t *stack_top;
    uint32_t *code_start;
    uint32_t *prog_end;
    uint32_t cpsr;
} criu_t;

void restore(void);

#endif