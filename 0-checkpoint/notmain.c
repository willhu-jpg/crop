#include "checkpoint.h"

void notmain() {

    printk("Hello World\n");

    for (int i = 0; i < 10; i++) {
        printk("i = %d\n", i);

        if (i == 5)
            checkpoint();
    }
    printk("Goodbye World\n");
}