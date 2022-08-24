global load_idt:function
load_idt:
lidt [rdi]
sti
ret

extern tss_gdt

global load_gdt:function
load_gdt:
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

global load_tss:function
load_tss:
mov rax,tss_gdt wrt ..sym
mov [rax + 4],rsp
mov ax,0x28
ltr ax
ret

global get_pmap:function
get_pmap:
mov rax,cr3
ret

global tlb_invalidate:function
tlb_invalidate:
invlpg [rdi]
ret