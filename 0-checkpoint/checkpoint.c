
#include "checkpoint.h"
#include "my-fat32-driver.h"

unsigned verbose = 1;

void print_metadata(criu_t metadata) {

    printk("[criu.c] Printing Metadata\n");
    printk("----------------------------------------\n");
    printk("sp: %x\n", metadata.sp);
    printk("stack top: %x\n", metadata.stack_top);
    printk("code_start: %x\n", metadata.code_start);
    printk("prog_end: %x\n", metadata.prog_end);
    printk("cpsr: %x\n", metadata.cpsr);
    printk("----------------------------------------\n");
}

int checkpoint_fn(uint32_t *sp, uint32_t cpsr) {

    uint32_t *stack_top = (void*)STACK_ADDR;
    int stack_size = stack_top - sp;
    
    assert(sp<stack_top);
    assert(aligned(sp,4));
    assert(aligned(stack_top,4));

    // print the stack
    if (verbose) {
        printk("[criu.c] Stack is [%x, %x]\n", sp, stack_top);
        for (uint32_t *i = sp; i < stack_top; i++) {
            printk("[criu.c] %x : %x\n", i, *i);
        }
    }
    
    // sweep through stack and save it into the FAT32 file system (includes r11-r4)
    if (my_fat32_write("STACK.TXT", (char *) sp, stack_size * sizeof(uint32_t))) {
        printk("[criu.c] Written STACK state\n");
    } else {
        panic("[criu.c] STACK write FAILED\n");
    }

    // PROGRAM
    int program_size = __prog_end__ - __code_start__;

    // sweep through code and save it into the FAT32 file system
    if (my_fat32_write("PROGRAM.TXT", (char *) __code_start__, program_size * sizeof(uint32_t))) {
        printk("[criu.c] Written PROGRAM state\n");
    } else {
        panic("[criu.c] PROGRAM write FAILED\n");
    }

    criu_t checkpoint_metadata = {
        .sp = sp,
        .stack_top = stack_top,
        .code_start = __code_start__,
        .prog_end = __prog_end__,
        .cpsr = cpsr,
    };

    if (my_fat32_write("METADATA.TXT", (char *) &checkpoint_metadata, sizeof(criu_t))) {
        printk("[criu.c] Written METADATA\n");
        print_metadata(checkpoint_metadata);
    } else {
        panic("[criu.c] METADATA write FAILED\n");
    }

    return 1;
}