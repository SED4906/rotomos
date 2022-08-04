bits 64
org 0x800000
start:

mov rdi,1
mov rsi,message
mov rdx,[message_len]
int 48

mov rdi,1<<39
int 53

mov rdi,1<<39
mov rsi,message2
mov rcx,[message2_len]
rep movsb

mov rdi,1
mov rsi,1<<39
mov rdx,[message2_len]
int 48

int 54

int 50

message: db "Hello, world!",13,10
message_len: dq 15

message2: db "Hello mmap!",13,10
message2_len: dq 13