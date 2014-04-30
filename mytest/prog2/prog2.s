	.file	"prog2.c.bc"
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
	testq	%rdi, %rdi
	je	.LBB1_2
# BB#1:                                 # %while.cond.preheader
	jg	.LBB1_3
	jmp	.LBB1_4
	.align	16, 0x90
.LBB1_3:                                # %while.body
                                        # =>This Inner Loop Header: Depth=1
	movabsq	$7378697629483820647, %rcx # imm = 0x6666666666666667
	movq	%rdi, %rax
	imulq	%rcx
	movq	%rdx, %rax
	shrq	$63, %rax
	sarq	$2, %rdx
	addq	%rax, %rdx
	shlq	$1, %rdx
	leaq	(%rdx,%rdx,4), %rbx
	subq	%rbx, %rdi
	addq	$48, %rdi
	callq	putchar
	testq	%rbx, %rbx
	movq	%rbx, %rdi
	jg	.LBB1_3
.LBB1_4:                                # %if.end
	popq	%rbx
	ret
.LBB1_2:                                # %if.then
	movabsq	$48, %rdi
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
	addq	$8, %rsp
	popq	%rbx
	popq	%r14
	ret
.Ltmp14:
	.size	PrintShort, .Ltmp14-PrintShort
	.cfi_endproc

	.globl	Print
	.align	16, 0x90
	.type	Print,@function
Print:                                  # @Print
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%r15
.Ltmp20:
	.cfi_def_cfa_offset 16
	pushq	%r14
.Ltmp21:
	.cfi_def_cfa_offset 24
	pushq	%r12
.Ltmp22:
	.cfi_def_cfa_offset 32
	pushq	%rbx
.Ltmp23:
	.cfi_def_cfa_offset 40
	pushq	%rax
.Ltmp24:
	.cfi_def_cfa_offset 48
.Ltmp25:
	.cfi_offset %rbx, -40
.Ltmp26:
	.cfi_offset %r12, -32
.Ltmp27:
	.cfi_offset %r14, -24
.Ltmp28:
	.cfi_offset %r15, -16
	movq	%rdi, %r14
	movabsq	$51, %rdi
	callq	putchar
	movabsq	$46, %rdi
	callq	putchar
	movabsq	$1, %r12
	cmpq	$2, size(%rip)
	jl	.LBB3_2
	.align	16, 0x90
.LBB3_1:                                # %for.body
                                        # =>This Inner Loop Header: Depth=1
	movq	(%r14,%r12,8), %r15
	movabsq	$7378697629483820647, %rcx # imm = 0x6666666666666667
	movq	%r15, %rax
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
	leaq	48(%r15,%rax), %rdi
	callq	putchar
	incq	%r12
	cmpq	size(%rip), %r12
	jl	.LBB3_1
.LBB3_2:                                # %for.end
	movabsq	$10, %rdi
	addq	$8, %rsp
	popq	%rbx
	popq	%r12
	popq	%r14
	popq	%r15
	jmp	putchar                 # TAILCALL
.Ltmp29:
	.size	Print, .Ltmp29-Print
	.cfi_endproc

	.globl	arctan
	.align	16, 0x90
	.type	arctan,@function
arctan:                                 # @arctan
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%r14
.Ltmp33:
	.cfi_def_cfa_offset 16
	pushq	%rbx
.Ltmp34:
	.cfi_def_cfa_offset 24
.Ltmp35:
	.cfi_offset %rbx, -24
.Ltmp36:
	.cfi_offset %r14, -16
	movq	%rdx, %r8
	xorq	%rcx, %rcx
	movq	powers(%rip), %r9
	movq	size(%rip), %rax
	testq	%rax, %rax
	jle	.LBB4_2
	.align	16, 0x90
.LBB4_1:                                # %for.body
                                        # =>This Inner Loop Header: Depth=1
	movq	$0, (%r9,%rcx,8)
	incq	%rcx
	movq	size(%rip), %rax
	cmpq	%rax, %rcx
	jl	.LBB4_1
.LBB4_2:                                # %for.end
	imulq	%rsi, %rdi
	xorq	%rbx, %rbx
	imulq	%rsi, %rsi
	movabsq	$1, %r10
	movq	term(%rip), %r11
	movq	pi(%rip), %r14
.LBB4_3:                                # %while.cond.outer
                                        # =>This Loop Header: Depth=1
                                        #     Child Loop BB4_4 Depth 2
                                        #     Child Loop BB4_6 Depth 2
                                        #     Child Loop BB4_11 Depth 2
                                        #     Child Loop BB4_10 Depth 2
                                        #     Child Loop BB4_13 Depth 2
                                        #     Child Loop BB4_16 Depth 2
	cmpq	%rax, %rbx
	movq	%rbx, %rcx
	jge	.LBB4_5
	.align	16, 0x90
.LBB4_4:                                # %for.body4
                                        #   Parent Loop BB4_3 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	addq	(%r9,%rcx,8), %rdi
	movq	%rdi, %rax
	cqto
	idivq	%rsi
	movq	%rax, (%r9,%rcx,8)
	imulq	%rsi, %rax
	subq	%rax, %rdi
	imulq	$100, %rdi, %rdi
	incq	%rcx
	movq	size(%rip), %rax
	cmpq	%rax, %rcx
	jl	.LBB4_4
.LBB4_5:                                # %for.cond13.loopexit
                                        #   in Loop: Header=BB4_3 Depth=1
	xorq	%rdi, %rdi
	cmpq	%rax, %rbx
	movq	%rbx, %rcx
	jge	.LBB4_7
	.align	16, 0x90
.LBB4_6:                                # %for.body15
                                        #   Parent Loop BB4_3 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	addq	(%r9,%rcx,8), %rdi
	movq	%rdi, %rax
	cqto
	idivq	%r10
	movq	%rax, (%r11,%rcx,8)
	imulq	%r10, %rax
	subq	%rax, %rdi
	imulq	$100, %rdi, %rdi
	incq	%rcx
	movq	size(%rip), %rax
	cmpq	%rax, %rcx
	jl	.LBB4_6
.LBB4_7:                                # %for.end26
                                        #   in Loop: Header=BB4_3 Depth=1
	testq	%r8, %r8
	jle	.LBB4_9
# BB#8:                                 # %for.cond29.preheader
                                        #   in Loop: Header=BB4_3 Depth=1
	xorq	%rcx, %rcx
	testq	%rax, %rax
	jg	.LBB4_10
	jmp	.LBB4_12
	.align	16, 0x90
.LBB4_10:                               # %for.body31
                                        #   Parent Loop BB4_3 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	addq	-8(%r14,%rax,8), %rcx
	movq	-8(%r11,%rax,8), %rdx
	leaq	-100(%rdx,%rcx), %rdi
	addq	%rdx, %rcx
	cmpq	$99, %rcx
	#pushfq
	#popq	%rdx
	cmovgq	%rdi, %rcx
	movq	%rcx, -8(%r14,%rax,8)
	decq	%rax
	testq	%rax, %rax
	movq	%rdx, %rcx
	jg	.LBB4_10
	jmp	.LBB4_12
	.align	16, 0x90

.LBB4_9:                                # %for.cond44.preheader
                                        #   in Loop: Header=BB4_3 Depth=1
	xorq	%rdx, %rdx
	testq	%rax, %rax
	jg	.LBB4_11
	jmp	.LBB4_12
	.align	16, 0x90
.LBB4_11:                               # %for.body46
                                        #   Parent Loop BB4_3 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	movq	-8(%r14,%rax,8), %rcx
	subq	-8(%r11,%rax,8), %rcx
	subq	%rdx, %rcx
	leaq	100(%rcx), %rdx
	testq	%rcx, %rcx
	cmovnsq	%rcx, %rdx
	movq	%rdx, -8(%r14,%rax,8)
	shrq	$63, %rcx
	decq	%rax
	testq	%rax, %rax
	movq	%rcx, %rdx
	jg	.LBB4_11
.LBB4_12:                               # %if.end59
                                        #   in Loop: Header=BB4_3 Depth=1
	addq	$2, %r10
	negq	%r8
	movq	size(%rip), %rax
	movq	%rbx, %rcx
	.align	16, 0x90
.LBB4_13:                               # %for.cond62
                                        #   Parent Loop BB4_3 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	cmpq	%rax, %rcx
	jge	.LBB4_17
# BB#14:                                # %for.body64
                                        #   in Loop: Header=BB4_13 Depth=2
	cmpq	$0, (%r9,%rcx,8)
	leaq	1(%rcx), %rcx
	je	.LBB4_13
# BB#15:                                # %while.cond74.loopexit
                                        #   in Loop: Header=BB4_3 Depth=1
	decq	%rbx
	.align	16, 0x90
.LBB4_16:                               # %while.cond74
                                        #   Parent Loop BB4_3 Depth=1
                                        # =>  This Inner Loop Header: Depth=2
	xorq	%rdi, %rdi
	cmpq	$0, 8(%r9,%rbx,8)
	leaq	1(%rbx), %rbx
	je	.LBB4_16
	jmp	.LBB4_3
.LBB4_17:                               # %while.cond.loopexit
	popq	%rbx
	popq	%r14
	ret
.Ltmp37:
	.size	arctan, .Ltmp37-arctan
	.cfi_endproc

	.globl	_main
	.align	16, 0x90
	.type	_main,@function
_main:                                  # @_main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbp
.Ltmp45:
	.cfi_def_cfa_offset 16
	pushq	%r15
.Ltmp46:
	.cfi_def_cfa_offset 24
	pushq	%r14
.Ltmp47:
	.cfi_def_cfa_offset 32
	pushq	%r13
.Ltmp48:
	.cfi_def_cfa_offset 40
	pushq	%r12
.Ltmp49:
	.cfi_def_cfa_offset 48
	pushq	%rbx
.Ltmp50:
	.cfi_def_cfa_offset 56
	pushq	%rax
.Ltmp51:
	.cfi_def_cfa_offset 64
.Ltmp52:
	.cfi_offset %rbx, -56
.Ltmp53:
	.cfi_offset %r12, -48
.Ltmp54:
	.cfi_offset %r13, -40
.Ltmp55:
	.cfi_offset %r14, -32
.Ltmp56:
	.cfi_offset %r15, -24
.Ltmp57:
	.cfi_offset %rbp, -16
	movq	%rsi, %rbx
	cmpq	$2, %rdi
	je	.LBB5_2
# BB#1:                                 # %if.then
	movabsq	$.L.str, %rdi
	callq	puts
	movabsq	$1, %rdi
	callq	llexit
.LBB5_2:                                # %if.end
	movq	8(%rbx), %rdi
	callq	atol
	movq	%rax, size(%rip)
	testq	%rax, %rax
	jg	.LBB5_4
# BB#3:                                 # %if.then3
	movabsq	$.L.str1, %rdi
	callq	puts
	movabsq	$1, %rdi
	callq	llexit
	movq	size(%rip), %rax
.LBB5_4:                                # %if.end5
	leaq	1(%rax), %rcx
	shrq	$63, %rcx
	leaq	1(%rax,%rcx), %rax
	sarq	%rax
	leaq	1(%rax), %rbp
	movq	%rbp, size(%rip)
	leaq	8(,%rax,8), %rbx
	movq	%rbx, %rdi
	callq	malloc
	movq	%rax, %r14
	movq	%r14, pi(%rip)
	movq	%rbx, %rdi
	callq	malloc
	movq	%rax, %r15
	movq	%r15, powers(%rip)
	movq	%rbx, %rdi
	callq	malloc
	movq	%rax, term(%rip)
	testq	%r14, %r14
	je	.LBB5_7
# BB#5:                                 # %if.end5
	testq	%r15, %r15
	je	.LBB5_7
# BB#6:                                 # %if.end5
	testq	%rax, %rax
	jne	.LBB5_8
.LBB5_7:                                # %if.then16
	movabsq	$.L.str2, %rdi
	callq	puts
	movabsq	$1, %rdi
	callq	llexit
	movq	size(%rip), %rbp
.LBB5_8:                                # %for.cond.preheader
	testq	%rbp, %rbp
	jle	.LBB5_11
# BB#9:                                 # %for.body.lr.ph
	xorq	%rax, %rax
	movq	pi(%rip), %rcx
	.align	16, 0x90
.LBB5_10:                               # %for.body
                                        # =>This Inner Loop Header: Depth=1
	movq	$0, (%rcx,%rax,8)
	incq	%rax
	cmpq	size(%rip), %rax
	jl	.LBB5_10
.LBB5_11:                               # %for.end
	xorq	%rdi, %rdi
	callq	time
	movq	%rax, %r14
	movabsq	$1, %r13
	movabsq	$16, %rdi
	movabsq	$5, %rsi
	movabsq	$1, %rdx
	callq	arctan
	movabsq	$4, %rdi
	movabsq	$239, %rsi
	movq	$-1, %rdx
	callq	arctan
	xorq	%rdi, %rdi
	callq	time
	movq	%rax, %r12
	movq	pi(%rip), %rbp
	movabsq	$51, %rdi
	callq	putchar
	movabsq	$46, %rdi
	callq	putchar
	cmpq	$2, size(%rip)
	jl	.LBB5_13
	.align	16, 0x90
.LBB5_12:                               # %for.body.i
                                        # =>This Inner Loop Header: Depth=1
	movq	(%rbp,%r13,8), %r15
	movabsq	$7378697629483820647, %rcx # imm = 0x6666666666666667
	movq	%r15, %rax
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
	leaq	48(%r15,%rax), %rdi
	callq	putchar
	incq	%r13
	cmpq	size(%rip), %r13
	jl	.LBB5_12
.LBB5_13:                               # %Print.exit
	movabsq	$10, %rdi
	callq	putchar
	movabsq	$.L.str3, %rdi
	callq	puts
	cmpq	%r14, %r12
	je	.LBB5_15
# BB#14:                                # %while.cond.preheader.i
	subq	%r14, %r12
	testq	%r12, %r12
	jg	.LBB5_16
	jmp	.LBB5_17
.LBB5_15:                               # %if.then.i
	movabsq	$48, %rdi
	callq	putchar
	jmp	.LBB5_17
	.align	16, 0x90
.LBB5_16:                               # %while.body.i
                                        # =>This Inner Loop Header: Depth=1
	movabsq	$7378697629483820647, %rcx # imm = 0x6666666666666667
	movq	%r12, %rax
	imulq	%rcx
	movq	%rdx, %rax
	shrq	$63, %rax
	sarq	$2, %rdx
	addq	%rax, %rdx
	shlq	$1, %rdx
	leaq	(%rdx,%rdx,4), %rbx
	subq	%rbx, %r12
	addq	$48, %r12
	movq	%r12, %rdi
	callq	putchar
	testq	%rbx, %rbx
	movq	%rbx, %r12
	jg	.LBB5_16
.LBB5_17:                               # %PrintVal.exit
	movabsq	$.L.str4, %rdi
	callq	puts
	xorq	%rax, %rax
	addq	$8, %rsp
	popq	%rbx
	popq	%r12
	popq	%r13
	popq	%r14
	popq	%r15
	popq	%rbp
	ret
.Ltmp58:
	.size	_main, .Ltmp58-_main
	.cfi_endproc

	.type	size,@object            # @size
	.comm	size,8,8
	.type	powers,@object          # @powers
	.comm	powers,8,8
	.type	term,@object            # @term
	.comm	term,8,8
	.type	pi,@object              # @pi
	.comm	pi,8,8
	.type	.L.str,@object          # @.str
	.section	.rodata.str1.1,"aMS",@progbits,1
.L.str:
	.asciz	 "I need to know how many digits to compute.\n"
	.size	.L.str, 44

	.type	.L.str1,@object         # @.str1
.L.str1:
	.asciz	 "Invalid argument.\n"
	.size	.L.str1, 19

	.type	.L.str2,@object         # @.str2
.L.str2:
	.asciz	 "Unable to allocate enough memory.\n"
	.size	.L.str2, 35

	.type	.L.str3,@object         # @.str3
.L.str3:
	.asciz	 "Calculation time "
	.size	.L.str3, 18

	.type	.L.str4,@object         # @.str4
.L.str4:
	.asciz	 " cycles\n"
	.size	.L.str4, 9


	.section	".note.GNU-stack","",@progbits
