#ifndef __MY_FAT32_H__
#define __MY_FAT32_H__

#include "rpi.h"
#include "fat32/code/fat32.h"

typedef struct {
    fat32_fs_t fs;
    pi_dirent_t root;
} my_fat32_t;

void my_fat32_init();

// This is very unique to our Pi setup. `buffer` points to a physical address.
// Neither the caller or this function allocates anything. It just writes to
// the physical address pointed to by `buffer`, and return the number of bytes written.
int my_fat32_read(char *name, char *buffer);

int my_fat32_write(char *name, char *buffer, int size);

#endif
