extern LoadNextTask

global ContextSwitch:function
ContextSwitch:
mov rdi, rsp
mov rsi, cr3
call LoadNextTask
mov rsp, rax
mov cr3, rdx
ret

global ContextSwitchNoSave:function
ContextSwitchNoSave:
mov rsp, rdi
mov cr3, rsi
ret

global KeIdle:function
KeIdle:
hlt
ret

global KeLockUpForever:function
KeLockUpForever:
cli
hlt
ret

global IdtLoad:function
IdtLoad:
lidt [rdi]
sti
ret

extern HandleExceptWithCode
extern HandleExceptNoCode
extern HandleUD2
extern HandlePageFault
extern HandlePit
extern HandleIrq
extern HandleKeyb
extern HandleDoubleFault

global isr_err_stub:function
isr_err_stub:
    xchg rdi,[rsp]
    call HandleExceptWithCode
    xchg rdi,[rsp]
    iretq

global isr_no_err_stub:function
isr_no_err_stub:
    call HandleExceptNoCode
    iretq

global isr_ud2_stub:function
isr_ud2_stub:
    mov rdi,[rsp]
    call HandleUD2
    iretq

global isr_page_fault:function
isr_page_fault:
    pop rdi
    call HandlePageFault
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
    call HandlePit
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
    call HandleIrq
    iretq

global isr_keyb_stub:function
isr_keyb_stub:
    call HandleKeyb
    iretq

global isr_double_fault_stub:function
isr_double_fault_stub:
    pop rdi
    call HandleDoubleFault
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
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym
    dq isr_irq_stub wrt ..sym

global GdtLoad:function
GdtLoad:
lgdt [rdi]
reloadSegments:
; Reload CS register:
PUSH 0x08                 ; Push code segment to stack, 0x08 is a stand-in for your code segment
LEA RAX, [rel .reload_CS] ; Load address of .reload_CS into RAX
PUSH RAX                  ; Push this value to the stack
RETFQ                     ; Perform a far return, RETFQ or LRETQ depending on syntax
.reload_CS:
; Reload data segment registers
MOV   AX, 0x10 ; 0x10 is a stand-in for your data segment
MOV   DS, AX
MOV   ES, AX
MOV   FS, AX
MOV   GS, AX
MOV   SS, AX
RET

global TssLoad:function
TssLoad:
mov [rdi+4],rsp
mov ax,0x28
ltr ax
ret

global MemCurrentPagemap:function
MemCurrentPagemap:
mov rax,cr3
ret