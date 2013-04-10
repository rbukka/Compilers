	.file	"test2.c"
	.section	.rodata
.LC0:
	.string	"Please input 1st value: "
.LC1:
	.string	"%f"
.LC4:
	.string	"Values AFTER %f, %f\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	leaq	-8(%rbp), %rax
	movq	%rax, %rsi
	movl	$.LC1, %edi
	movl	$0, %eax
	call	scanf
.L4:
	movss	-8(%rbp), %xmm0
	xorps	%xmm1, %xmm1
	ucomiss	%xmm1, %xmm0
	jbe	.L7
.L6:
	movl	.LC3(%rip), %eax
	movl	%eax, -4(%rbp)
	movss	-8(%rbp), %xmm0
	subss	-4(%rbp), %xmm0
	movss	%xmm0, -8(%rbp)
	jmp	.L4
.L7:
	nop
	movss	-4(%rbp), %xmm1
	cvtps2pd	%xmm1, %xmm1
	movss	-8(%rbp), %xmm0
	unpcklps	%xmm0, %xmm0
	cvtps2pd	%xmm0, %xmm0
	movl	$.LC4, %edi
	movl	$2, %eax
	call	printf
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.section	.rodata
	.align 4
.LC3:
	.long	1082130432
	.ident	"GCC: (GNU) 4.7.0"
	.section	.note.GNU-stack,"",@progbits
