PROGS := notmain.c

COMMON_SRC += checkpoint-asm.S
COMMON_SRC += checkpoint.c
COMMON_SRC += my-fat32-driver.c

LIBS += ./libfat32.a

RUN=1
# if you get issues, try swapping this.
# OPT_LEVEL = -O0
OPT_LEVEL = -O2
CFLAGS += -fno-inline

# LIB_POST := $(CS240LX_2025_PATH)/lib/libgcc.a
# LIBS += $(LIB_POST)

EXCLUDE ?= grep -v simple_boot
include $(CS240LX_2025_PATH)/libpi/mk/Makefile.robust-v2