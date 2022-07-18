extern exception_err_handler
extern exception_no_err_handler
extern page_fault_handler
extern irq_handler
extern pit_handler
extern keyb_handler

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

global isr_stub_table:data
isr_stub_table:
    dq 8 dup isr_no_err_stub wrt ..sym
    dq isr_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq 4 dup isr_err_stub wrt ..sym
    dq isr_page_fault wrt ..sym 
    dq 2 dup isr_no_err_stub wrt ..sym
    dq isr_err_stub wrt ..sym
    dq 12 dup isr_no_err_stub wrt ..sym
    dq isr_err_stub wrt ..sym
    dq isr_no_err_stub wrt ..sym
    dq isr_pit_stub wrt ..sym
    dq isr_keyb_stub wrt ..sym
    dq 14 dup isr_irq_stub wrt ..sym