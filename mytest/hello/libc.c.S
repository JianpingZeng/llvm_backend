	.file	"libc.c.bc"
	.text
	.globl	__libc_start_main
	.align	16, 0x90
	.type	__libc_start_main,@function
__libc_start_main:                      # @__libc_start_main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp1:
	.cfi_def_cfa_offset 16
	movq	%rdi, %rax
	leaq	8(%rdx,%rsi,8), %rcx
	movq	%rsi, %rdi
	movq	%rdx, %rsi
	movq	%rcx, %rdx
	callq	*%rax
	movq	%rax, %rcx
	movq	$60, %rax
	movq	%rcx, %rdi
	#APP
	syscall
	#NO_APP
	xorq	%rax, %rax
	popq	%rdx
	retq
.Ltmp2:
	.size	__libc_start_main, .Ltmp2-__libc_start_main
	.cfi_endproc


	.ident	"clang version 3.5 (trunk 202454)"
	.section	".note.GNU-stack","",@progbits
