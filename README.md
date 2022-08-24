# Rotom OS
## Brief introduction
Rotom OS is a 64-bit operating system kernel.

It attempts to be as simple as possible.

Ring 3 has not yet been implemented.
## Application Binary Interface
Programs load to virtual address 0x800000 and that is where their entry point is.
The only executable format currently supported is a flat binary.

There are currently 8 system calls across 8 interrupts starting from vector 0x30.
In order, they are: write, read, exit, open, close, mmap, munmap, exec.

Do not be fooled, these don't work like their UNIX counterparts at all. I mean it, none of them work the same.

Look at the function prototypes in `testing/rotomos_abi.h` to confirm this (also listed below using more sensible types than in the actual file):

```c
size_t write(void* handle, void* str, size_t len); // int 48
size_t read(void* handle, void* str, size_t len); // int 49
void exit(); // int 50
void* open(char* path); // int 51
void close(void* handle); // int 52
void* mmap(void* vaddr); // int 53
void munmap(void* vaddr); // int 54
void exec(char* path); // int 55
```

Arguments are passed like usual C functions.
## Writing to the screen or reading from the keyboard
You'll want to open path "tty" or "keyboard" first.

The void* returned is a handle, a pointer to some data in memory sufficient to do file operations.

You can then just write data to the terminal using this handle, and read data from the keyboard.

There's also nothing stopping you from reading from the terminal or writing to the keyboard...

Don't forget to close the handle too when you're going to exit!
## Notable Quirks with FIFOs
The filesystem type used for the tty and keyboard is a FIFO.

Reading begins from wherever the end was when the handle was created.
Writing always happens at the end.

The position of reading does not change with writes.
