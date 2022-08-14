extern exception_err_handler
extern exception_no_err_handler
extern double_fault_handler
extern page_fault_handler
extern is_working_handler
extern irq_handler
extern pit_handler
extern keyb_handler
extern write_handler
extern read_handler
extern exit_handler
extern open_handler
extern close_handler
extern mmap_handler
extern munmap_handler
extern exec_handler
extern ud2_handler
extern mouse_handler

global isr_err_stub:function
isr_err_stub:
    xchg rdi,[rsp]
    call exception_err_handler
    xchg rdi,[rsp]
    iretq

global isr_no_err_stub:function
isr_no_err_stub:
    call exception_no_err_handler
    iretq

global isr_ud2_stub:function
isr_ud2_stub:
    mov rdi,[rsp]
    call ud2_handler
    iretq

global isr_page_fault:function
isr_page_fault:
    pop rdi
    call page_fault_handler
    iretq

global isr_pit_stub:function
isr_pit_stub:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
    push rbp
    call pit_handler
    pop rbp
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    iretq

global isr_irq_stub:function
isr_irq_stub:
    call irq_handler
    iretq

global isr_keyb_stub:function
isr_keyb_stub:
    call keyb_handler
    iretq

global isr_double_fault_stub:function
isr_double_fault_stub:
    pop rdi
    call double_fault_handler
    iretq

global isr_syscall_read_stub:function
isr_syscall_read_stub:
    call read_handler
    iretq

global isr_syscall_write_stub:function
isr_syscall_write_stub:
    call write_handler
    iretq

global isr_syscall_exit_stub:function
isr_syscall_exit_stub:
    call exit_handler
    iretq

global isr_syscall_open_stub:function
isr_syscall_open_stub:
    call open_handler
    iretq

global isr_syscall_close_stub:function
isr_syscall_close_stub:
    call close_handler
    iretq

global isr_syscall_mmap_stub:function
isr_syscall_mmap_stub:
    call mmap_handler
    iretq

global isr_syscall_munmap_stub:function
isr_syscall_munmap_stub:
    call munmap_handler
    iretq

global isr_syscall_exec_stub:function
isr_syscall_exec_stub:
    call exec_handler
    iretq

global isr_mouse_stub:function
isr_mouse_stub:
    call mouse_handler
    iretq

global isr_stub_table:data
isr_stub_table:
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_ud2_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_double_fault_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_err_stub wrt ..sym
    dq isr_err_stub wrt ..sym
    dq isr_err_stub wrt ..sym
    dq isr_err_stub wrt ..sym
    dq isr_page_fault wrt ..sym 
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_pit_stub wrt ..sym
    dq isr_keyb_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_mouse_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_syscall_write_stub wrt ..sym
    dq isr_syscall_read_stub wrt ..sym
    dq isr_syscall_exit_stub wrt ..sym
    dq isr_syscall_open_stub wrt ..sym
    dq isr_syscall_close_stub wrt ..sym
    dq isr_syscall_mmap_stub wrt ..sym
    dq isr_syscall_munmap_stub wrt ..sym
    dq isr_syscall_exec_stub wrt ..sym

global load_idt:function
load_idt:
lidt [rdi]
sti
ret

global hang_forever:function
hang_forever:
cli
hlt
jmp hang_forever

global hang_idle:function
hang_idle:
hlt
ret