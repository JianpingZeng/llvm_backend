	.file	"hello.c.bc"
	.text
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	xorq	%rax, %rax
	retq
.Ltmp0:
	.size	main, .Ltmp0-main
	.cfi_endproc


	.ident	"clang version 3.5 (trunk 202454)"
	.section	".note.GNU-stack","",@progbits
