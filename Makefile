# This is the name that our final kernel executable will have.
# Change as needed.
KERNEL := rotomos.elf

# It is highly recommended to use a custom built cross toolchain to build a kernel.
# We are only using "cc" as a placeholder here. It may work by using
# the host system's toolchain, but this is not guaranteed.
#CC := x86_64-elf-gcc
CC := clang
CPP := clang++
# Likewise, "ld" here is just a placeholder and your mileage may vary if using the
# host's "ld".
#LD := x86_64-elf-ld
LD := ld.lld

AS := nasm

# User controllable CFLAGS.
CFLAGS ?= -Wall -Wextra -Os -pipe -Iinclude
CPPFLAGS ?= -Wall -Wextra -Os -pipe -Iinclude

# User controllable linker flags. We set none by default.
LDFLAGS ?= 	-Tkernel/linker.ld \
	-nostdlib              \
	-static                \
	-pie                   \
	--no-dynamic-linker 

CFLAGS +=            \
	-I.                  \
	-std=c17           \
	-ffreestanding       \
	-fno-stack-protector \
	-fpie                \
	-fPIC \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-target x86_64-elf \
	-g

CPPFLAGS +=            \
	-I.                  \
	-std=c++20           \
	-ffreestanding       \
	-fno-stack-protector \
	-fpie                \
	-fPIC \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mno-red-zone        \
	-target x86_64-elf \
	-g


OBJ := x86-64/bin.o x86-64/cpu.o x86-64/fb.o x86-64/fs.o x86-64/isr.o x86-64/keyb.o x86-64/logo.o x86-64/main.o x86-64/mm.o x86-64/pic.o x86-64/printf.o x86-64/sys.o x86-64/syscall.o x86-64/tables.o x86-64/task.o kernel/libc.o kernel/list.o

# Default target.
.PHONY: all
all: $(KERNEL)

# Link rules for the final kernel executable.
$(KERNEL): $(OBJ)
	$(LD) $(OBJ) $(LDFLAGS) -o $@

%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(AS) -felf64 -g $< -o $@

# Remove object files and the final executable.
.PHONY: clean
clean:
	rm -rf $(KERNEL) $(OBJ)