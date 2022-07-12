extern switch_task
global context_switch:function
context_switch:
mov rdi,rsp
mov rsi,cr3
call switch_task
mov rsp,rax
mov cr3,rdx
ret

global context_switch_nosave:function
context_switch_nosave:
mov rsp,rdi
mov cr3,rsi
ret