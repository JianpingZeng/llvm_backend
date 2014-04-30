	.file	"libc.c.bc"
	.text
	.globl	llexit
	.align	16, 0x90
	.type	llexit,@function
llexit:                                 # @llexit
	.cfi_startproc
# BB#0:                                 # %entry
	movabsq	$60, %rax
	#APP
	syscall
	#NO_APP
	ret
.Ltmp0:
	.size	llexit, .Ltmp0-llexit
	.cfi_endproc

	.globl	llstrlen
	.align	16, 0x90
	.type	llstrlen,@function
llstrlen:                               # @llstrlen
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, %rcx
	andq	$-8, %rcx
	andq	$7, %rdi
	movq	%rdi, %rax
	negq	%rax
	cmpq	$6, %rdi
	ja	.LBB1_22
# BB#1:                                 # %entry
	jmpq	*.LJTI1_0(,%rdi,8)
.LBB1_2:                                # %entry.sw.bb3_crit_edge
	movq	(%rcx), %rdx
	jmp	.LBB1_5
.LBB1_22:                               # %entry.sw.default_crit_edge
	movq	(%rcx), %rdx
	jmp	.LBB1_23
.LBB1_7:                                # %entry.sw.bb8_crit_edge
	movq	(%rcx), %rdx
	jmp	.LBB1_8
.LBB1_10:                               # %entry.sw.bb14_crit_edge
	movq	(%rcx), %rdx
	jmp	.LBB1_11
.LBB1_13:                               # %entry.sw.bb20_crit_edge
	movq	(%rcx), %rdx
	jmp	.LBB1_14
.LBB1_16:                               # %entry.sw.bb26_crit_edge
	movq	(%rcx), %rdx
	jmp	.LBB1_17
.LBB1_19:                               # %entry.sw.bb32_crit_edge
	movq	(%rcx), %rdx
	jmp	.LBB1_20
.LBB1_3:                                # %if.end42
	addq	$8, %rcx
	addq	$8, %rax
.LBB1_4:                                # %do.body
	movq	(%rcx), %rdx
	testq	$255, %rdx
	je	.LBB1_25
.LBB1_5:                                # %sw.bb3
	testq	$65280, %rdx            # imm = 0xFF00
	jne	.LBB1_8
# BB#6:                                 # %if.then6
	incq	%rax
	ret
.LBB1_8:                                # %sw.bb8
	testq	$16711680, %rdx         # imm = 0xFF0000
	jne	.LBB1_11
# BB#9:                                 # %if.then11
	addq	$2, %rax
	ret
.LBB1_11:                               # %sw.bb14
	movabsq	$4278190080, %rsi       # imm = 0xFF000000
	testq	%rsi, %rdx
	jne	.LBB1_14
# BB#12:                                # %if.then17
	addq	$3, %rax
	ret
.LBB1_14:                               # %sw.bb20
	movabsq	$1095216660480, %rsi    # imm = 0xFF00000000
	testq	%rsi, %rdx
	jne	.LBB1_17
# BB#15:                                # %if.then23
	addq	$4, %rax
	ret
.LBB1_17:                               # %sw.bb26
	movabsq	$280375465082880, %rsi  # imm = 0xFF0000000000
	testq	%rsi, %rdx
	jne	.LBB1_20
# BB#18:                                # %if.then29
	addq	$5, %rax
	ret
.LBB1_20:                               # %sw.bb32
	movabsq	$71776119061217280, %rsi # imm = 0xFF000000000000
	testq	%rsi, %rdx
	jne	.LBB1_23
# BB#21:                                # %if.then35
	addq	$6, %rax
	ret
.LBB1_23:                               # %sw.default
	shrq	$56, %rdx
	testq	%rdx, %rdx
	jne	.LBB1_3
# BB#24:                                # %if.then40
	addq	$7, %rax
.LBB1_25:                               # %sw.epilog
	ret
.Ltmp1:
	.size	llstrlen, .Ltmp1-llstrlen
	.cfi_endproc
	.section	.rodata,"a",@progbits
	.align	8
.LJTI1_0:
	.quad	.LBB1_4
	.quad	.LBB1_2
	.quad	.LBB1_7
	.quad	.LBB1_10
	.quad	.LBB1_13
	.quad	.LBB1_16
	.quad	.LBB1_19

	.text
	.globl	puts
	.align	16, 0x90
	.type	puts,@function
puts:                                   # @puts
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rbx
.Ltmp4:
	.cfi_def_cfa_offset 16
.Ltmp5:
	.cfi_offset %rbx, -16
	movq	%rdi, %rbx
	callq	llstrlen
	movq	%rax, %rcx
	movabsq	$1, %rax
	movabsq	$1, %rdi
	movq	%rbx, %rsi
	movq	%rcx, %rdx
	#APP
	syscall
	#NO_APP
	popq	%rbx
	ret
.Ltmp6:
	.size	puts, .Ltmp6-puts
	.cfi_endproc

	.globl	putchar
	.align	16, 0x90
	.type	putchar,@function
putchar:                                # @putchar
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, -8(%rsp)
	leaq	-8(%rsp), %rsi
	movabsq	$1, %rax
	movabsq	$1, %rdi
	movabsq	$1, %rdx
	#APP
	syscall
	#NO_APP
	ret
.Ltmp7:
	.size	putchar, .Ltmp7-putchar
	.cfi_endproc

	.globl	malloc
	.align	16, 0x90
	.type	malloc,@function
malloc:                                 # @malloc
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, %rdx
	addq	$7, %rdx
	andq	$-8, %rdx
	movq	current_brk(%rip), %rdi
	addq	%rdx, %rdi
	movq	%rdi, current_brk(%rip)
	movabsq	$12, %rax
	#APP
	syscall
	#NO_APP
	movq	current_brk(%rip), %rax
	subq	%rdx, %rax
	ret
.Ltmp8:
	.size	malloc, .Ltmp8-malloc
	.cfi_endproc

	.globl	isspace
	.align	16, 0x90
	.type	isspace,@function
isspace:                                # @isspace
	.cfi_startproc
# BB#0:                                 # %entry
	movabsq	$1, %rax
	cmpq	$32, %rdi
	je	.LBB5_2
# BB#1:                                 # %lor.rhs
	addq	$-9, %rdi
	cmpq	$5, %rdi
	pushfq
	popq	%rax
.LBB5_2:                                # %lor.end
	andq	$1, %rax
	ret
.Ltmp9:
	.size	isspace, .Ltmp9-isspace
	.cfi_endproc

	.globl	isdigit
	.align	16, 0x90
	.type	isdigit,@function
isdigit:                                # @isdigit
	.cfi_startproc
# BB#0:                                 # %entry
	addq	$-48, %rdi
	cmpq	$10, %rdi
	pushfq
	popq	%rax
	ret
.Ltmp10:
	.size	isdigit, .Ltmp10-isdigit
	.cfi_endproc

	.globl	LLreadchar
	.align	16, 0x90
	.type	LLreadchar,@function
LLreadchar:                             # @LLreadchar
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, %rax
	andq	$-8, %rax
	movq	(%rax), %rax
	andq	$7, %rdi
	decq	%rdi
	cmpq	$6, %rdi
	ja	.LBB7_9
# BB#1:                                 # %entry
	jmpq	*.LJTI7_0(,%rdi,8)
.LBB7_2:                                # %sw.bb2
	shrq	$8, %rax
	andq	$255, %rax
	ret
.LBB7_3:                                # %sw.bb4
	shrq	$16, %rax
	andq	$255, %rax
	ret
.LBB7_4:                                # %sw.bb6
	shrq	$24, %rax
	andq	$255, %rax
	ret
.LBB7_5:                                # %sw.bb8
	shrq	$32, %rax
	andq	$255, %rax
	ret
.LBB7_6:                                # %sw.bb10
	shrq	$40, %rax
	andq	$255, %rax
	ret
.LBB7_7:                                # %sw.bb12
	shrq	$48, %rax
	andq	$255, %rax
	ret
.LBB7_8:                                # %sw.bb14
	shrq	$56, %rax
.LBB7_9:                                # %sw.epilog
	andq	$255, %rax
	ret
.Ltmp11:
	.size	LLreadchar, .Ltmp11-LLreadchar
	.cfi_endproc
	.section	.rodata,"a",@progbits
	.align	8
.LJTI7_0:
	.quad	.LBB7_2
	.quad	.LBB7_3
	.quad	.LBB7_4
	.quad	.LBB7_5
	.quad	.LBB7_6
	.quad	.LBB7_7
	.quad	.LBB7_8

	.text
	.globl	atol
	.align	16, 0x90
	.type	atol,@function
atol:                                   # @atol
	.cfi_startproc
# BB#0:                                 # %entry
	.align	16, 0x90
.LBB8_1:                                # %while.cond
                                        # =>This Inner Loop Header: Depth=1
	movq	%rdi, %rax
	andq	$-8, %rax
	movq	(%rax), %rdx
	movq	%rdi, %rax
	andq	$7, %rax
	decq	%rax
	cmpq	$6, %rax
	movq	%rdx, %rcx
	ja	.LBB8_10
# BB#2:                                 # %while.cond
                                        #   in Loop: Header=BB8_1 Depth=1
	jmpq	*.LJTI8_0(,%rax,8)
.LBB8_3:                                # %sw.bb2.i
                                        #   in Loop: Header=BB8_1 Depth=1
	movq	%rdx, %rcx
	shrq	$8, %rcx
	jmp	.LBB8_10
.LBB8_4:                                # %sw.bb4.i
                                        #   in Loop: Header=BB8_1 Depth=1
	movq	%rdx, %rcx
	shrq	$16, %rcx
	jmp	.LBB8_10
.LBB8_5:                                # %sw.bb6.i
                                        #   in Loop: Header=BB8_1 Depth=1
	movq	%rdx, %rcx
	shrq	$24, %rcx
	jmp	.LBB8_10
.LBB8_6:                                # %sw.bb8.i
                                        #   in Loop: Header=BB8_1 Depth=1
	movq	%rdx, %rcx
	shrq	$32, %rcx
	jmp	.LBB8_10
.LBB8_7:                                # %sw.bb10.i
                                        #   in Loop: Header=BB8_1 Depth=1
	movq	%rdx, %rcx
	shrq	$40, %rcx
	jmp	.LBB8_10
.LBB8_8:                                # %sw.bb12.i
                                        #   in Loop: Header=BB8_1 Depth=1
	movq	%rdx, %rcx
	shrq	$48, %rcx
	jmp	.LBB8_10
.LBB8_9:                                # %sw.bb14.i
                                        #   in Loop: Header=BB8_1 Depth=1
	movq	%rdx, %rcx
	shrq	$56, %rcx
	.align	16, 0x90
.LBB8_10:                               # %LLreadchar.exit
                                        #   in Loop: Header=BB8_1 Depth=1
	incq	%rdi
	andq	$255, %rcx
	cmpq	$32, %rcx
	leaq	-9(%rcx), %rcx
	je	.LBB8_1
# BB#11:                                # %LLreadchar.exit
                                        #   in Loop: Header=BB8_1 Depth=1
	cmpq	$5, %rcx
	jb	.LBB8_1
# BB#12:                                # %while.end
	cmpq	$6, %rax
	ja	.LBB8_21
# BB#13:                                # %while.end
	jmpq	*.LJTI8_1(,%rax,8)
.LBB8_14:                               # %sw.bb2.i63
	shrq	$8, %rdx
	jmp	.LBB8_21
.LBB8_15:                               # %sw.bb4.i65
	shrq	$16, %rdx
	jmp	.LBB8_21
.LBB8_16:                               # %sw.bb6.i67
	shrq	$24, %rdx
	jmp	.LBB8_21
.LBB8_17:                               # %sw.bb8.i69
	shrq	$32, %rdx
	jmp	.LBB8_21
.LBB8_18:                               # %sw.bb10.i71
	shrq	$40, %rdx
	jmp	.LBB8_21
.LBB8_19:                               # %sw.bb12.i73
	shrq	$48, %rdx
	jmp	.LBB8_21
.LBB8_20:                               # %sw.bb14.i75
	shrq	$56, %rdx
.LBB8_21:                               # %LLreadchar.exit78
	andq	$255, %rdx
	xorq	%r8, %r8
	cmpq	$43, %rdx
	je	.LBB8_24
# BB#22:                                # %LLreadchar.exit78
	leaq	-1(%rdi), %rax
	cmpq	$45, %rdx
	jne	.LBB8_25
# BB#23:                                # %sw.bb
	movabsq	$1, %r8
.LBB8_24:                               # %sw.bb3
	movq	%rdi, %rax
.LBB8_25:                               # %while.cond5.preheader
	xorq	%rdx, %rdx
	jmp	.LBB8_26
	.align	16, 0x90
.LBB8_45:                               # %LLreadchar.exit40
                                        #   in Loop: Header=BB8_26 Depth=1
	incq	%rax
	shlq	$1, %rdx
	leaq	(%rdx,%rdx,4), %rdx
	andq	$255, %rcx
	addq	$48, %rdx
	subq	%rcx, %rdx
.LBB8_26:                               # %while.cond5
                                        # =>This Inner Loop Header: Depth=1
	movq	%rax, %rcx
	andq	$-8, %rcx
	movq	(%rcx), %rcx
	movq	%rax, %rdi
	andq	$7, %rdi
	decq	%rdi
	cmpq	$6, %rdi
	movq	%rcx, %rsi
	ja	.LBB8_35
# BB#27:                                # %while.cond5
                                        #   in Loop: Header=BB8_26 Depth=1
	jmpq	*.LJTI8_2(,%rdi,8)
.LBB8_28:                               # %sw.bb2.i44
                                        #   in Loop: Header=BB8_26 Depth=1
	movq	%rcx, %rsi
	shrq	$8, %rsi
	jmp	.LBB8_35
.LBB8_29:                               # %sw.bb4.i46
                                        #   in Loop: Header=BB8_26 Depth=1
	movq	%rcx, %rsi
	shrq	$16, %rsi
	jmp	.LBB8_35
.LBB8_30:                               # %sw.bb6.i48
                                        #   in Loop: Header=BB8_26 Depth=1
	movq	%rcx, %rsi
	shrq	$24, %rsi
	jmp	.LBB8_35
.LBB8_31:                               # %sw.bb8.i50
                                        #   in Loop: Header=BB8_26 Depth=1
	movq	%rcx, %rsi
	shrq	$32, %rsi
	jmp	.LBB8_35
.LBB8_32:                               # %sw.bb10.i52
                                        #   in Loop: Header=BB8_26 Depth=1
	movq	%rcx, %rsi
	shrq	$40, %rsi
	jmp	.LBB8_35
.LBB8_33:                               # %sw.bb12.i54
                                        #   in Loop: Header=BB8_26 Depth=1
	movq	%rcx, %rsi
	shrq	$48, %rsi
	jmp	.LBB8_35
.LBB8_34:                               # %sw.bb14.i56
                                        #   in Loop: Header=BB8_26 Depth=1
	movq	%rcx, %rsi
	shrq	$56, %rsi
	.align	16, 0x90
.LBB8_35:                               # %LLreadchar.exit59
                                        #   in Loop: Header=BB8_26 Depth=1
	andq	$255, %rsi
	addq	$-48, %rsi
	cmpq	$9, %rsi
	ja	.LBB8_46
# BB#36:                                # %while.body9
                                        #   in Loop: Header=BB8_26 Depth=1
	cmpq	$6, %rdi
	ja	.LBB8_45
# BB#37:                                # %while.body9
                                        #   in Loop: Header=BB8_26 Depth=1
	jmpq	*.LJTI8_3(,%rdi,8)
.LBB8_38:                               # %sw.bb2.i25
                                        #   in Loop: Header=BB8_26 Depth=1
	shrq	$8, %rcx
	jmp	.LBB8_45
.LBB8_39:                               # %sw.bb4.i27
                                        #   in Loop: Header=BB8_26 Depth=1
	shrq	$16, %rcx
	jmp	.LBB8_45
.LBB8_40:                               # %sw.bb6.i29
                                        #   in Loop: Header=BB8_26 Depth=1
	shrq	$24, %rcx
	jmp	.LBB8_45
.LBB8_41:                               # %sw.bb8.i31
                                        #   in Loop: Header=BB8_26 Depth=1
	shrq	$32, %rcx
	jmp	.LBB8_45
.LBB8_42:                               # %sw.bb10.i33
                                        #   in Loop: Header=BB8_26 Depth=1
	shrq	$40, %rcx
	jmp	.LBB8_45
.LBB8_43:                               # %sw.bb12.i35
                                        #   in Loop: Header=BB8_26 Depth=1
	shrq	$48, %rcx
	jmp	.LBB8_45
.LBB8_44:                               # %sw.bb14.i37
                                        #   in Loop: Header=BB8_26 Depth=1
	shrq	$56, %rcx
	jmp	.LBB8_45
.LBB8_46:                               # %while.end13
	movq	%rdx, %rax
	negq	%rax
	testq	%r8, %r8
	cmovneq	%rdx, %rax
	ret
.Ltmp12:
	.size	atol, .Ltmp12-atol
	.cfi_endproc
	.section	.rodata,"a",@progbits
	.align	8
.LJTI8_0:
	.quad	.LBB8_3
	.quad	.LBB8_4
	.quad	.LBB8_5
	.quad	.LBB8_6
	.quad	.LBB8_7
	.quad	.LBB8_8
	.quad	.LBB8_9
.LJTI8_1:
	.quad	.LBB8_14
	.quad	.LBB8_15
	.quad	.LBB8_16
	.quad	.LBB8_17
	.quad	.LBB8_18
	.quad	.LBB8_19
	.quad	.LBB8_20
.LJTI8_2:
	.quad	.LBB8_28
	.quad	.LBB8_29
	.quad	.LBB8_30
	.quad	.LBB8_31
	.quad	.LBB8_32
	.quad	.LBB8_33
	.quad	.LBB8_34
.LJTI8_3:
	.quad	.LBB8_38
	.quad	.LBB8_39
	.quad	.LBB8_40
	.quad	.LBB8_41
	.quad	.LBB8_42
	.quad	.LBB8_43
	.quad	.LBB8_44

	.text
	.globl	time
	.align	16, 0x90
	.type	time,@function
time:                                   # @time
	.cfi_startproc
# BB#0:                                 # %entry
	movq	%rdi, %rdx
	leaq	-16(%rsp), %rsi
	movabsq	$228, %rax
	xorq	%rdi, %rdi
	#APP
	syscall
	#NO_APP
	movq	-16(%rsp), %rax
	testq	%rdx, %rdx
	je	.LBB9_2
# BB#1:                                 # %if.then
	movq	%rax, (%rdx)
.LBB9_2:                                # %if.end
	ret
.Ltmp13:
	.size	time, .Ltmp13-time
	.cfi_endproc

	.globl	__libc_start_main
	.align	16, 0x90
	.type	__libc_start_main,@function
__libc_start_main:                      # @__libc_start_main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp15:
	.cfi_def_cfa_offset 16
	movq	%rdi, %r8
	movabsq	$12, %rax
	xorq	%rdi, %rdi
	#APP
	syscall
	#NO_APP
	movq	%rax, current_brk(%rip)
	leaq	8(%rdx,%rsi,8), %rax
	movq	%rsi, %rdi
	movq	%rdx, %rsi
	movq	%rax, %rdx
	callq	*%r8
	movq	%rax, %rcx
	movabsq	$60, %rax
	movq	%rcx, %rdi
	#APP
	syscall
	#NO_APP
	xorq	%rax, %rax
	popq	%rdx
	ret
.Ltmp16:
	.size	__libc_start_main, .Ltmp16-__libc_start_main
	.cfi_endproc

	.type	current_brk,@object     # @current_brk
	.comm	current_brk,8,8

	.section	".note.GNU-stack","",@progbits
