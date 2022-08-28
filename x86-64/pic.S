global init_pic:function
init_pic:
mov dx, 0x21
in al, dx
xchg bl,al
mov dx, 0xA1
in al, dx
xchg cl,al
mov al, 0x11
mov dx, 0x20
out dx, al
mov dx, 0xA0
out dx, al
mov dx, 0x21
xchg ax,di
out dx, al
mov dx, 0xA1
xchg ax,si
out dx, al
mov al, 4
mov dx, 0x21
out dx, al
mov al, 2
mov dx, 0xA1
out dx, al
mov al, 1
mov dx, 0x21
out dx, al
mov dx, 0xA1
out dx, al
mov dx, 0x21
xchg al,bl
out dx, al
mov dx, 0xA1
xchg al,cl
out dx, al
ret

global pic_clear_mask:function
pic_clear_mask:
cmp di, 8
jb .master
mov dx, 0xA1
sub di, 8
jmp .do
.master:
mov dx, 0x21
.do:
in al, dx
mov bl, 1
xchg cx, di
shl bl, cl
not bl
and al, bl
out dx, al
ret

global init_pit:function
init_pit:
mov al, 0x36
out 0x43, al
mov al, 0x8C
out 0x40, al
mov al, 0x43
out 0x40, al
ret

extern switch_task

global context_switch:function
context_switch:
mov rdi, rsp
mov rsi, cr3
call switch_task
mov rsp, rax
mov cr3, rdx
ret

global context_switch_nosave:function
context_switch_nosave:
mov rsp,rdi
mov cr3,rsi
ret

extern save_register
extern load_register

global pit_context_switch:function
pit_context_switch:
push rax
push rbx
push rcx
push rdx
push rbp
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

%assign i 0
%rep 5
pop rdi
mov rsi,i
call save_register
%assign i i+1
%endrep

%assign i i+1
%rep 10
pop rdi
mov rsi,i
call save_register
%assign i i+1
%endrep

mov al, 0x20
out 0x20, al
call context_switch

%assign i 0
%rep 5
mov rdi,i
call load_register
push rax
%assign i i+1
%endrep

%assign i i+1
%rep 10
mov rdi,i
call load_register
push rax
%assign i i+1
%endrep

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
pop rbp
pop rdx
pop rcx
pop rbx
pop rax
iretq

extern keyb_handler
global keyb_interrupt:function
keyb_interrupt:
push rax
push rdx
push rdi
mov dx, 0x60
xor rax,rax
in al, dx
xor rdi,rdi
add rdi, rax
call keyb_handler
mov al, 0x20
out 0x20, al
pop rdi
pop rdx
pop rax
iretq