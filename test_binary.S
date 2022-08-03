bits 64
org 0x800000
start:

mov rdi,1
mov rsi,message
mov rdx,[message_len]
int 48

int 50

message: db "Hello, world!"
message_len: dq 13