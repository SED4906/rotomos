global tlb_invalidate:function
tlb_invalidate:
invlpg [rdi]
ret

global get_pmap:function
get_pmap:
mov rax,cr3
ret