#include "rpi.h"
#include "my-fat32-driver.h"

static my_fat32_t my_fat32;
static int my_fat32_initialized = 0;

void my_fat32_init() {
    if (my_fat32_initialized) {
        return;
    }

    kmalloc_init(FAT32_HEAP_MB);
    pi_sd_init();
  
    // printk("Reading the MBR.\n");
    mbr_t *mbr = mbr_read();
  
    // printk("Loading the first partition.\n");
    mbr_partition_ent_t partition;
    memcpy(&partition, mbr->part_tab1, sizeof(mbr_partition_ent_t));
    assert(mbr_part_is_fat32(partition.part_type));
  
    // printk("Loading the FAT.\n");
    my_fat32.fs = fat32_mk(&partition);
  
    // printk("Loading the root directory.\n");
    my_fat32.root = fat32_get_root(&my_fat32.fs);

    my_fat32_initialized = 1;

    // printk("Listing files:\n");
    // pi_directory_t files = fat32_readdir(&my_fat32.fs, &my_fat32.root);
    // printk("Got %d files.\n", files.ndirents);
    // for (int i = 0; i < files.ndirents; i++) {
    //     pi_dirent_t *dirent = &files.dirents[i];
    //     if (dirent->is_dir_p) {
    //         printk("\tD: %s (cluster %d)\n", dirent->name, dirent->cluster_id);
    //     } else {
    //         printk("\tF: %s (cluster %d; %d bytes)\n", dirent->name, dirent->cluster_id, dirent->nbytes);
    //     }
    // }
}

int my_fat32_read(char *name, char *buffer) {
    if (!my_fat32_initialized) {
        my_fat32_init();
    }

    pi_file_t *file = fat32_read(&my_fat32.fs, &my_fat32.root, name);

    if (!file) {
        printk("%s not found.\n", name);
        return -1;
    }

    memcpy(buffer, file->data, file->n_data);
    return file->n_data;
}

int my_fat32_write(char *name, char *buffer, int size) {
    if (!my_fat32_initialized) {
        my_fat32_init();
    }

    fat32_delete(&my_fat32.fs, &my_fat32.root, name);
	fat32_create(&my_fat32.fs, &my_fat32.root, name, 0);

	pi_file_t file = (pi_file_t){
		.data = buffer,
		.n_data = size,
		.n_alloc = size,
	};

    return fat32_write(&my_fat32.fs, &my_fat32.root, name, &file);
}
