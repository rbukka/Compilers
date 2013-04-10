	.file	"test3_g.c"
	.comm	a,4,4
	.comm	b,4,4
	.comm	c,4,4
	.text
	.globl	init
	.type	init, @function
init:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$1, a(%rip)
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	init, .-init
	.section	.rodata
.LC0:
	.string	"Please input 1st value: "
.LC1:
	.string	"%d"
.LC2:
	.string	"%d, %d\n"
.LC3:
	.string	"%d %d %d\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB1:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$10, -4(%rbp)
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	leaq	-4(%rbp), %rax
	movq	%rax, %rsi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	scanf
	jmp	.L3
.L5:
	movl	$0, %eax
	call	init
	movl	a(%rip), %eax
	addl	$1, %eax
	movl	%eax, b(%rip)
	movl	b(%rip), %eax
	cmpl	$4, %eax
	jg	.L4
	movl	b(%rip), %eax
	addl	$5, %eax
	movl	%eax, c(%rip)
.L4:
	movl	-4(%rbp), %esi
	movl	c(%rip), %ecx
	movl	b(%rip), %edx
	movl	a(%rip), %eax
	movl	%esi, %r8d
	movl	%eax, %esi
	movl	$.LC2, %edi
	movl	$0, %eax
	call	printf
	movl	-4(%rbp), %eax
	subl	$1, %eax
	movl	%eax, -4(%rbp)
.L3:
	movl	-4(%rbp), %eax
	cmpl	$5, %eax
	jg	.L5
	movl	$0, %eax
	call	init
	movl	c(%rip), %ecx
	movl	b(%rip), %edx
	movl	a(%rip), %eax
	movl	%eax, %esi
	movl	$.LC3, %edi
	movl	$0, %eax
	call	printf
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.7.0"
	.section	.note.GNU-stack,"",@progbits
