pushq %rax
mov $0x5561dc69,%rdi
movq $0x6166373939623935,%rax
movq %rax,(%rdi)
movb $0x00,8(%rdi)
popq %rax
pushq $0x4018fa
ret
