extern write_handler
global isr_write
isr_write:
call write_handler
iretq

extern read_handler
global isr_read
isr_read:
call read_handler
iretq

extern exit_handler
global isr_exit
isr_exit:
call exit_handler
iretq

extern open_handler
global isr_open
isr_open:
call open_handler
iretq

extern close_handler
global isr_close
isr_close:
call close_handler
iretq

extern mmap_handler
global isr_mmap
isr_mmap:
call mmap_handler
iretq

extern munmap_handler
global isr_munmap
isr_munmap:
call munmap_handler
iretq

extern exec_handler
global isr_exec
isr_exec:
call exec_handler
iretq