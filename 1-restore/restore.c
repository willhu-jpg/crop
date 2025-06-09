
#include "restore.h"
#include "my-fat32-driver.h"

unsigned verbose = 0;

void print_metadata(criu_t metadata) {

    printk("[criu.c] Printing Metadata\n");
    printk("----------------------------------------\n");
    printk("sp: %x\n", metadata.sp);
    printk("stack top: %x\n", metadata.stack_top);
    printk("code_start: %x\n", metadata.code_start);
    printk("prog_end: %x\n", metadata.prog_end);
    printk("----------------------------------------\n");
}

void restore() {

    criu_t checkpoint_metadata;
    int num_bytes_read = my_fat32_read("METADATA.TXT", (char *) &checkpoint_metadata);
    if(num_bytes_read) {
        printk("[criu.c] Read %d bytes from METADATA\n", num_bytes_read);
        print_metadata(checkpoint_metadata);
    } else {
        printk("[criu.c] Could not read METADATA.TXT\n");
    }
    
    uint32_t *sp = checkpoint_metadata.sp;
    uint32_t *stack_top = checkpoint_metadata.stack_top;
    uint32_t *code_start = checkpoint_metadata.code_start;
    uint32_t *prog_end = checkpoint_metadata.prog_end;
    uint32_t cpsr = checkpoint_metadata.cpsr;

    // Restore PROGRAM
    int prog_size = prog_end - code_start;

    // Read out the PROGRAM
    uint32_t PROG_OUT[prog_size];
    num_bytes_read = my_fat32_read("PROGRAM.TXT", (char *) PROG_OUT);
    if(num_bytes_read) {
        printk("[criu.c] Read %d bytes for [%x, %x]\n", num_bytes_read, code_start, prog_end);
    } else {
        printk("[criu.c] Could not read PROGRAM.TXT\n");
    }

    // Restore the program
    memcpy(code_start, PROG_OUT, 4 * prog_size);

    // Restore STACK and registers
    int stack_size = stack_top - sp;

    // Read out the STACK and registers
    uint32_t STACK_OUT[stack_size];
    num_bytes_read = my_fat32_read("STACK.TXT", (char *) STACK_OUT);
    if(num_bytes_read) {
        printk("[criu.c] Read %d bytes\n", num_bytes_read);

        if (verbose) {
            for (int i = 0; i < stack_size; i++) {
                printk("[criu.c] STACK_OUT[%d]: %x\n", i, STACK_OUT[i]);
            }
        }
    } else {
        printk("[criu.c] Could not read METADATA.TXT\n");
    }

    // Restore the stack
    memcpy(sp, STACK_OUT, 4 * stack_size);
    
    // Print the stack
    if (verbose) {
        printk("[criu.c] Stack is [%x, %x]\n", sp, stack_top);
        for (uint32_t *i = sp; i < stack_top; i++) {
            printk("[criu.c] %x : %x\n", i, *i);
        }
    }

    asm volatile(
        "msr cpsr, %0\n"
        "mov sp, %1\n"
        "pop {lr}\n"
        "pop {r4-r11}\n" 
        "bx lr\n"
        :
        : "r"(cpsr), "r"(sp)
        : "memory"
    );
}