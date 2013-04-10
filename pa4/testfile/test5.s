	.file	"test5.c"
	.section	.rodata
.LC0:
	.string	"%d %d\n"
	.text
	.globl	fun
	.type	fun, @function
fun:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$32, %rsp
	movl	%edi, -20(%rbp)
	movl	%esi, -24(%rbp)
	movl	$9, -4(%rbp)
	movl	$1, -8(%rbp)
	movl	-8(%rbp), %eax
	cmpl	$1, %eax
	je	.L3
	cmpl	$4, %eax
	je	.L4
	jmp	.L2
.L3:
	movl	-4(%rbp), %eax
	cmpl	$1, %eax
	je	.L6
	cmpl	$2, %eax
	je	.L7
	jmp	.L5
.L6:
	addl	$1, -4(%rbp)
	jmp	.L8
.L7:
	movl	-20(%rbp), %eax
	cmpl	$1, %eax
	je	.L9
	cmpl	$2, %eax
	je	.L10
	jmp	.L5
.L9:
	movl	-24(%rbp), %eax
	imull	-20(%rbp), %eax
	movl	%eax, -12(%rbp)
	jmp	.L5
.L10:
	movl	$2, %eax
	movl	%eax, %edx
	sarl	$31, %edx
	idivl	-20(%rbp)
	movl	%eax, %edx
	movl	-12(%rbp), %eax
	addl	%edx, %eax
	movl	%eax, -24(%rbp)
	nop
.L5:
	addl	$2, -4(%rbp)
.L8:
	movl	$1, -16(%rbp)
.L4:
	movl	-8(%rbp), %eax
	addl	$4, %eax
	movl	%eax, -16(%rbp)
.L2:
	addl	$1, -8(%rbp)
	movl	-4(%rbp), %eax
	movl	-8(%rbp), %edx
	addl	%edx, %eax
	addl	$1, %eax
	movl	%eax, -16(%rbp)
	movl	-16(%rbp), %eax
	addl	$20, %eax
	movl	%eax, -8(%rbp)
	movl	-16(%rbp), %eax
	addl	$22, %eax
	movl	%eax, -12(%rbp)
	movl	-16(%rbp), %edx
	movl	-12(%rbp), %eax
	movl	%eax, %esi
	movl	$.LC0, %edi
	movl	$0, %eax
	call	printf
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	fun, .-fun
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
	movl	$2, %esi
	movl	$1, %edi
	call	fun
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE1:
	.size	main, .-main
	.ident	"GCC: (GNU) 4.7.0"
	.section	.note.GNU-stack,"",@progbits
