extern exception_no_err_handler
extern exception_err_handler
extern exception_pf_handler

global die:function
die:
cli
hlt
jmp die

global idle:function
idle:
hlt
ret

global isr_no_err_stub:function
isr_no_err_stub:
call exception_no_err_handler

global isr_err_stub:function
isr_err_stub:
pop rdi
call exception_err_handler

global isr_pf_stub:function
isr_pf_stub:
pop rdi
mov rsi, cr2
call exception_pf_handler

global isr_irqm_stub:function
isr_irqm_stub:
push rax
mov al, 0x20
out 0x20, al
pop rax
iretq

global isr_irqs_stub:function
isr_irqs_stub:
push rax
mov al, 0x20
out 0x20, al
out 0xA0, al
pop rax
iretq

global isr_stub_table:data
isr_stub_table:
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
dq isr_err_stub wrt ..sym
dq isr_err_stub wrt ..sym
dq isr_err_stub wrt ..sym
dq isr_err_stub wrt ..sym
dq isr_pf_stub wrt ..sym 
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
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqm_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym
dq isr_irqs_stub wrt ..sym