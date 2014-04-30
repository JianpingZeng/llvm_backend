	.file	"prog4.bc"
	.text
	.globl	OutDig
	.align	16, 0x90
	.type	OutDig,@function
OutDig:                                 # @OutDig
	.cfi_startproc
# BB#0:                                 # %entry
	addq	$48, %rdi
	jmp	putchar                 # TAILCALL
.Ltmp0:
	.size	OutDig, .Ltmp0-OutDig
	.cfi_endproc

	.globl	PrintVal
	.align	16, 0x90
	.type	PrintVal,@function
PrintVal:                               # @PrintVal
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbx
.Ltmp3:
	.cfi_def_cfa_offset 16
.Ltmp4:
	.cfi_offset %rbx, -16
	movq	%rdi, %rbx
	cmpq	$10, %rbx
	jl	.LBB1_2
# BB#1:                                 # %if.then
	movabsq	$7378697629483820647, %rcx # imm = 0x6666666666666667
	movq	%rbx, %rax
	imulq	%rcx
	movq	%rdx, %rax
	shrq	$63, %rax
	sarq	$2, %rdx
	leaq	(%rdx,%rax), %rdi
	callq	PrintVal
.LBB1_2:                                # %if.end
	movabsq	$7378697629483820647, %rcx # imm = 0x6666666666666667
	movq	%rbx, %rax
	imulq	%rcx
	movq	%rdx, %rax
	shrq	$63, %rax
	sarq	$2, %rdx
	addq	%rax, %rdx
	shlq	$1, %rdx
	leaq	(%rdx,%rdx,4), %rax
	negq	%rax
	leaq	48(%rbx,%rax), %rdi
	popq	%rbx
	jmp	putchar                 # TAILCALL
.Ltmp5:
	.size	PrintVal, .Ltmp5-PrintVal
	.cfi_endproc

	.globl	PrintShort
	.align	16, 0x90
	.type	PrintShort,@function
PrintShort:                             # @PrintShort
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%r14
.Ltmp9:
	.cfi_def_cfa_offset 16
	pushq	%rbx
.Ltmp10:
	.cfi_def_cfa_offset 24
	pushq	%rax
.Ltmp11:
	.cfi_def_cfa_offset 32
.Ltmp12:
	.cfi_offset %rbx, -24
.Ltmp13:
	.cfi_offset %r14, -16
	movq	%rdi, %r14
	movabsq	$7378697629483820647, %rcx # imm = 0x6666666666666667
	movq	%r14, %rax
	imulq	%rcx
	movq	%rdx, %rbx
	movq	%rbx, %rax
	shrq	$63, %rax
	sarq	$2, %rbx
	addq	%rax, %rbx
	movq	%rbx, %rax
	imulq	%rcx
	movq	%rdx, %rax
	shrq	$63, %rax
	sarq	$2, %rdx
	addq	%rax, %rdx
	shlq	$1, %rdx
	leaq	(%rdx,%rdx,4), %rax
	movq	%rbx, %rcx
	subq	%rax, %rcx
	leaq	48(%rcx), %rdi
	callq	putchar
	shlq	$1, %rbx
	leaq	(%rbx,%rbx,4), %rax
	negq	%rax
	leaq	48(%r14,%rax), %rdi
	callq	putchar
	movabsq	$10, %rdi
	callq	putchar
	addq	$8, %rsp
	popq	%rbx
	popq	%r14
	ret
.Ltmp14:
	.size	PrintShort, .Ltmp14-PrintShort
	.cfi_endproc

	.globl	_main
	.align	16, 0x90
	.type	_main,@function
_main:                                  # @_main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbx
.Ltmp17:
	.cfi_def_cfa_offset 16
.Ltmp18:
	.cfi_offset %rbx, -16
	cmpq	$2, pi+8(%rip)
	pushfq
	popq	%rbx
	movq	%rbx, pi+8(%rip)
	movabsq	$48, %rdi
	callq	putchar
	orq	$48, %rbx
	movq	%rbx, %rdi
	callq	putchar
	movabsq	$10, %rdi
	callq	putchar
	xorq	%rax, %rax
	popq	%rbx
	ret
.Ltmp19:
	.size	_main, .Ltmp19-_main
	.cfi_endproc

	.type	pi,@object              # @pi
	.comm	pi,800,16
	.type	powers,@object          # @powers
	.comm	powers,800,16
	.type	term,@object            # @term
	.comm	term,800,16

	.section	".note.GNU-stack","",@progbits
