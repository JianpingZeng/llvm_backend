	.file	"libc.c.bc"
	.file	1 "libc.c"
	.file	2 "./syscall.h"
	.text
	.globl	llexit
	.align	16, 0x90
	.type	llexit,@function
llexit:                                 # @llexit
	.cfi_startproc
.Lfunc_begin0:
	.loc	1 6 0                   # libc.c:6:0
# BB#0:                                 # %entry
	#DEBUG_VALUE: llexit:s <- RDI+0
	#DEBUG_VALUE: __syscall1:n <- 60+0
	#DEBUG_VALUE: __syscall1:a1 <- RDI+0
	movabsq	$60, %rax
	.loc	2 324 0 prologue_end    # ./syscall.h:324:0
.Ltmp0:
	#APP
	syscall
	#NO_APP
.Ltmp1:
	.loc	1 8 0                   # libc.c:8:0
	ret
.Ltmp2:
.Ltmp3:
	.size	llexit, .Ltmp3-llexit
.Lfunc_end0:
	.cfi_endproc

	.globl	llstrlen
	.align	16, 0x90
	.type	llstrlen,@function
llstrlen:                               # @llstrlen
	.cfi_startproc
.Lfunc_begin1:
	.loc	1 10 0                  # libc.c:10:0
# BB#0:                                 # %entry
	#DEBUG_VALUE: llstrlen:s <- RDI+0
	#DEBUG_VALUE: v <- RDI+0
	.loc	1 13 0 prologue_end     # libc.c:13:0
	movq	%rdi, %rcx
	andq	$-8, %rcx
	.loc	1 14 0                  # libc.c:14:0
	andq	$7, %rdi
.Ltmp4:
	#DEBUG_VALUE: p <- RCX+0
	movq	%rdi, %rax
	negq	%rax
.Ltmp5:
	#DEBUG_VALUE: l <- RAX+0
	.loc	1 15 0                  # libc.c:15:0
	cmpq	$6, %rdi
	ja	.LBB1_2
.Ltmp6:
# BB#1:                                 # %entry
	jmpq	*.LJTI1_0(,%rdi,8)
.LBB1_3:                                # %entry.sw.bb3_crit_edge
	.loc	1 19 0                  # libc.c:19:0
.Ltmp7:
	movq	(%rcx), %rdx
.Ltmp8:
	.loc	1 15 0                  # libc.c:15:0
	jmpq	.LBB1_10
.LBB1_2:                                # %entry.sw.default_crit_edge
.Ltmp9:
	#DEBUG_VALUE: p <- RCX+0
	#DEBUG_VALUE: l <- RAX+0
	.loc	1 31 0                  # libc.c:31:0
	movq	(%rcx), %rdx
.Ltmp10:
	.loc	1 15 0                  # libc.c:15:0
	jmpq	.LBB1_22
.Ltmp11:
.LBB1_4:                                # %entry.sw.bb8_crit_edge
	.loc	1 21 0                  # libc.c:21:0
	movq	(%rcx), %rdx
.Ltmp12:
	.loc	1 15 0                  # libc.c:15:0
	jmpq	.LBB1_12
.LBB1_5:                                # %entry.sw.bb14_crit_edge
	.loc	1 23 0                  # libc.c:23:0
.Ltmp13:
	movq	(%rcx), %rdx
.Ltmp14:
	.loc	1 15 0                  # libc.c:15:0
	jmpq	.LBB1_14
.LBB1_6:                                # %entry.sw.bb20_crit_edge
	.loc	1 25 0                  # libc.c:25:0
.Ltmp15:
	movq	(%rcx), %rdx
.Ltmp16:
	.loc	1 15 0                  # libc.c:15:0
	jmpq	.LBB1_16
.LBB1_7:                                # %entry.sw.bb26_crit_edge
	.loc	1 27 0                  # libc.c:27:0
.Ltmp17:
	movq	(%rcx), %rdx
.Ltmp18:
	.loc	1 15 0                  # libc.c:15:0
	jmpq	.LBB1_18
.LBB1_8:                                # %entry.sw.bb32_crit_edge
	.loc	1 29 0                  # libc.c:29:0
.Ltmp19:
	movq	(%rcx), %rdx
.Ltmp20:
	.loc	1 15 0                  # libc.c:15:0
	jmpq	.LBB1_20
.LBB1_9:                                # %do.body
	.loc	1 17 0                  # libc.c:17:0
.Ltmp21:
	movq	(%rcx), %rdx
	testq	$255, %rdx
	je	.LBB1_25
	jmpq	.LBB1_10
.LBB1_10:                               # %sw.bb3
	.loc	1 19 0                  # libc.c:19:0
	testq	$65280, %rdx            # imm = 0xFF00
	jne	.LBB1_12
	jmpq	.LBB1_11
.LBB1_12:                               # %sw.bb8
	.loc	1 21 0                  # libc.c:21:0
	testq	$16711680, %rdx         # imm = 0xFF0000
	jne	.LBB1_14
	jmpq	.LBB1_13
.LBB1_14:                               # %sw.bb14
	movabsq	$4278190080, %rsi       # imm = 0xFF000000
	.loc	1 23 0                  # libc.c:23:0
	testq	%rsi, %rdx
	jne	.LBB1_16
	jmpq	.LBB1_15
.LBB1_16:                               # %sw.bb20
	movabsq	$1095216660480, %rsi    # imm = 0xFF00000000
	.loc	1 25 0                  # libc.c:25:0
	testq	%rsi, %rdx
	jne	.LBB1_18
	jmpq	.LBB1_17
.LBB1_18:                               # %sw.bb26
	movabsq	$280375465082880, %rsi  # imm = 0xFF0000000000
	.loc	1 27 0                  # libc.c:27:0
	testq	%rsi, %rdx
	jne	.LBB1_20
	jmpq	.LBB1_19
.LBB1_20:                               # %sw.bb32
	movabsq	$71776119061217280, %rsi # imm = 0xFF000000000000
	.loc	1 29 0                  # libc.c:29:0
	testq	%rsi, %rdx
	jne	.LBB1_22
	jmpq	.LBB1_21
.LBB1_22:                               # %sw.default
	.loc	1 31 0                  # libc.c:31:0
	shrq	$56, %rdx
	testq	%rdx, %rdx
	jne	.LBB1_24
	jmpq	.LBB1_23
.LBB1_23:                               # %if.then40
	addq	$7, %rax
	jmpq	.LBB1_25
.LBB1_24:                               # %if.end42
	.loc	1 33 0                  # libc.c:33:0
	addq	$8, %rcx
.Ltmp22:
	#DEBUG_VALUE: p <- RCX+0
	.loc	1 32 0                  # libc.c:32:0
	addq	$8, %rax
.Ltmp23:
	#DEBUG_VALUE: l <- RAX+0
	.loc	1 33 0                  # libc.c:33:0
	jmpq	.LBB1_9
.Ltmp24:
.LBB1_11:                               # %if.then6
	.loc	1 19 0                  # libc.c:19:0
	incq	%rax
	jmpq	.LBB1_25
.LBB1_13:                               # %if.then11
	.loc	1 21 0                  # libc.c:21:0
	addq	$2, %rax
	jmpq	.LBB1_25
.LBB1_15:                               # %if.then17
	.loc	1 23 0                  # libc.c:23:0
	addq	$3, %rax
	jmpq	.LBB1_25
.LBB1_17:                               # %if.then23
	.loc	1 25 0                  # libc.c:25:0
	addq	$4, %rax
	jmpq	.LBB1_25
.LBB1_19:                               # %if.then29
	.loc	1 27 0                  # libc.c:27:0
	addq	$5, %rax
	jmpq	.LBB1_25
.LBB1_21:                               # %if.then35
	.loc	1 29 0                  # libc.c:29:0
	addq	$6, %rax
	jmpq	.LBB1_25
.Ltmp25:
.LBB1_25:                               # %sw.epilog
	.loc	1 36 0                  # libc.c:36:0
	ret
.Ltmp26:
.Ltmp27:
	.size	llstrlen, .Ltmp27-llstrlen
.Lfunc_end1:
	.cfi_endproc
	.section	.rodata,"a",@progbits
	.align	8
.LJTI1_0:
	.quad	.LBB1_9
	.quad	.LBB1_3
	.quad	.LBB1_4
	.quad	.LBB1_5
	.quad	.LBB1_6
	.quad	.LBB1_7
	.quad	.LBB1_8

	.text
	.globl	puts
	.align	16, 0x90
	.type	puts,@function
puts:                                   # @puts
	.cfi_startproc
.Lfunc_begin2:
	.loc	1 38 0                  # libc.c:38:0
# BB#0:                                 # %entry
	pushq	%rbx
.Ltmp30:
	.cfi_def_cfa_offset 16
.Ltmp31:
	.cfi_offset %rbx, -16
	#DEBUG_VALUE: puts:s <- RDI+0
	movq	%rdi, %rbx
.Ltmp32:
	#DEBUG_VALUE: puts:s <- RBX+0
	.loc	1 39 0 prologue_end     # libc.c:39:0
	callq	llstrlen
	movq	%rax, %rcx
.Ltmp33:
	#DEBUG_VALUE: __syscall3:n <- 1+0
	#DEBUG_VALUE: __syscall3:a1 <- 1+0
	movabsq	$1, %rax
	movabsq	$1, %rdi
	.loc	2 339 0                 # ./syscall.h:339:0
.Ltmp34:
	movq	%rbx, %rsi
	movq	%rcx, %rdx
	#APP
	syscall
	#NO_APP
.Ltmp35:
	#DEBUG_VALUE: ret <- RAX+0
	.loc	1 39 0                  # libc.c:39:0
	popq	%rbx
.Ltmp36:
	ret
.Ltmp37:
.Ltmp38:
	.size	puts, .Ltmp38-puts
.Lfunc_end2:
	.cfi_endproc

	.globl	putchar
	.align	16, 0x90
	.type	putchar,@function
putchar:                                # @putchar
	.cfi_startproc
.Lfunc_begin3:
	.loc	1 42 0                  # libc.c:42:0
# BB#0:                                 # %entry
	#DEBUG_VALUE: putchar:c <- RDI+0
	.loc	1 44 0 prologue_end     # libc.c:44:0
	movq	%rdi, -8(%rsp)
	leaq	-8(%rsp), %rsi
.Ltmp39:
	#DEBUG_VALUE: __syscall3:n <- 1+0
	#DEBUG_VALUE: __syscall3:a1 <- 1+0
	movabsq	$1, %rax
	movabsq	$1, %rdi
.Ltmp40:
	movabsq	$1, %rdx
	.loc	2 339 0                 # ./syscall.h:339:0
.Ltmp41:
	#APP
	syscall
	#NO_APP
.Ltmp42:
	#DEBUG_VALUE: ret <- RAX+0
	.loc	1 45 0                  # libc.c:45:0
	ret
.Ltmp43:
.Ltmp44:
	.size	putchar, .Ltmp44-putchar
.Lfunc_end3:
	.cfi_endproc

	.globl	malloc
	.align	16, 0x90
	.type	malloc,@function
malloc:                                 # @malloc
	.cfi_startproc
.Lfunc_begin4:
	.loc	1 49 0                  # libc.c:49:0
# BB#0:                                 # %entry
	#DEBUG_VALUE: malloc:size <- RDI+0
	movq	%rdi, %rdx
.Ltmp45:
	#DEBUG_VALUE: malloc:size <- RDX+0
	.loc	1 50 0 prologue_end     # libc.c:50:0
	addq	$7, %rdx
.Ltmp46:
	andq	$-8, %rdx
.Ltmp47:
	#DEBUG_VALUE: malloc:size <- RDX+0
	movq	current_brk(%rip), %rdi
	.loc	1 51 0                  # libc.c:51:0
	addq	%rdx, %rdi
	movq	%rdi, current_brk(%rip)
.Ltmp48:
	#DEBUG_VALUE: __syscall1:n <- 12+0
	#DEBUG_VALUE: __syscall1:a1 <- RDI+0
	movabsq	$12, %rax
	.loc	2 324 0                 # ./syscall.h:324:0
.Ltmp49:
	#APP
	syscall
	#NO_APP
.Ltmp50:
	.loc	1 53 0                  # libc.c:53:0
	movq	current_brk(%rip), %rax
	subq	%rdx, %rax
	ret
.Ltmp51:
.Ltmp52:
	.size	malloc, .Ltmp52-malloc
.Lfunc_end4:
	.cfi_endproc

	.globl	isspace
	.align	16, 0x90
	.type	isspace,@function
isspace:                                # @isspace
	.cfi_startproc
.Lfunc_begin5:
	.loc	1 58 0                  # libc.c:58:0
# BB#0:                                 # %entry
	#DEBUG_VALUE: isspace:c <- RDI+0
	movabsq	$1, %rax
	.loc	1 59 0 prologue_end     # libc.c:59:0
.Ltmp53:
	cmpq	$32, %rdi
	je	.LBB5_2
	jmpq	.LBB5_1
.LBB5_1:                                # %lor.rhs
.Ltmp54:
	#DEBUG_VALUE: isspace:c <- RDI+0
	addq	$-9, %rdi
.Ltmp55:
	cmpq	$5, %rdi
	pushfq
	popq	%rax
.LBB5_2:                                # %lor.end
	andq	$1, %rax
	ret
.Ltmp56:
.Ltmp57:
	.size	isspace, .Ltmp57-isspace
.Lfunc_end5:
	.cfi_endproc

	.globl	isdigit
	.align	16, 0x90
	.type	isdigit,@function
isdigit:                                # @isdigit
	.cfi_startproc
.Lfunc_begin6:
	.loc	1 63 0                  # libc.c:63:0
# BB#0:                                 # %entry
	#DEBUG_VALUE: isdigit:c <- RDI+0
	.loc	1 64 0 prologue_end     # libc.c:64:0
	addq	$-48, %rdi
.Ltmp58:
	cmpq	$10, %rdi
	pushfq
	popq	%rax
	ret
.Ltmp59:
.Ltmp60:
	.size	isdigit, .Ltmp60-isdigit
.Lfunc_end6:
	.cfi_endproc

	.globl	LLreadchar
	.align	16, 0x90
	.type	LLreadchar,@function
LLreadchar:                             # @LLreadchar
	.cfi_startproc
.Lfunc_begin7:
	.loc	1 67 0                  # libc.c:67:0
# BB#0:                                 # %entry
	#DEBUG_VALUE: LLreadchar:c <- RDI+0
	#DEBUG_VALUE: v <- RDI+0
	.loc	1 70 0 prologue_end     # libc.c:70:0
	movq	%rdi, %rax
	andq	$-8, %rax
	.loc	1 71 0                  # libc.c:71:0
	movq	(%rax), %rax
.Ltmp61:
	#DEBUG_VALUE: r <- RAX+0
	#DEBUG_VALUE: p <- RAX+0
	.loc	1 72 0                  # libc.c:72:0
	andq	$7, %rdi
.Ltmp62:
	decq	%rdi
	cmpq	$6, %rdi
	ja	.LBB7_9
.Ltmp63:
# BB#1:                                 # %entry
	jmpq	*.LJTI7_0(,%rdi,8)
.LBB7_2:                                # %sw.bb2
	.loc	1 74 0                  # libc.c:74:0
.Ltmp64:
	shrq	$8, %rax
.Ltmp65:
	#DEBUG_VALUE: r <- RAX+0
	jmpq	.LBB7_9
.Ltmp66:
.LBB7_3:                                # %sw.bb4
	.loc	1 75 0                  # libc.c:75:0
	shrq	$16, %rax
.Ltmp67:
	#DEBUG_VALUE: r <- RAX+0
	jmpq	.LBB7_9
.Ltmp68:
.LBB7_4:                                # %sw.bb6
	.loc	1 76 0                  # libc.c:76:0
	shrq	$24, %rax
.Ltmp69:
	#DEBUG_VALUE: r <- RAX+0
	jmpq	.LBB7_9
.Ltmp70:
.LBB7_5:                                # %sw.bb8
	.loc	1 77 0                  # libc.c:77:0
	shrq	$32, %rax
.Ltmp71:
	#DEBUG_VALUE: r <- RAX+0
	jmpq	.LBB7_9
.Ltmp72:
.LBB7_6:                                # %sw.bb10
	.loc	1 78 0                  # libc.c:78:0
	shrq	$40, %rax
.Ltmp73:
	#DEBUG_VALUE: r <- RAX+0
	jmpq	.LBB7_9
.Ltmp74:
.LBB7_7:                                # %sw.bb12
	.loc	1 79 0                  # libc.c:79:0
	shrq	$48, %rax
.Ltmp75:
	#DEBUG_VALUE: r <- RAX+0
	jmpq	.LBB7_9
.Ltmp76:
.LBB7_8:                                # %sw.bb14
	.loc	1 80 0                  # libc.c:80:0
	shrq	$56, %rax
.Ltmp77:
	#DEBUG_VALUE: r <- RAX+0
.LBB7_9:                                # %sw.epilog
	#DEBUG_VALUE: p <- RAX+0
	.loc	1 82 0                  # libc.c:82:0
	andq	$255, %rax
.Ltmp78:
	ret
.Ltmp79:
.Ltmp80:
	.size	LLreadchar, .Ltmp80-LLreadchar
.Lfunc_end7:
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
.Lfunc_begin8:
	.loc	1 86 0                  # libc.c:86:0
# BB#0:                                 # %entry
	#DEBUG_VALUE: atol:s <- RDI+0
	#DEBUG_VALUE: n <- 0+0
	#DEBUG_VALUE: neg <- 0+0
	.align	16, 0x90
.LBB8_1:                                # %while.cond
                                        # =>This Inner Loop Header: Depth=1
	#DEBUG_VALUE: n <- 0+0
	#DEBUG_VALUE: neg <- 0+0
	#DEBUG_VALUE: LLreadchar:c <- RDI+0
	#DEBUG_VALUE: v <- RDI+0
	#DEBUG_VALUE: v <- RDI+0
	.loc	1 70 0 prologue_end     # libc.c:70:0
	movq	%rdi, %rax
	andq	$-8, %rax
	.loc	1 71 0                  # libc.c:71:0
	movq	(%rax), %rdx
.Ltmp81:
	#DEBUG_VALUE: r <- RDX+0
	#DEBUG_VALUE: r <- RDX+0
	#DEBUG_VALUE: p <- RAX+0
	#DEBUG_VALUE: p <- RAX+0
	.loc	1 72 0                  # libc.c:72:0
	movq	%rdi, %rax
.Ltmp82:
	andq	$7, %rax
	decq	%rax
	cmpq	$6, %rax
	movq	%rdx, %rcx
	ja	.LBB8_10
.Ltmp83:
# BB#2:                                 # %while.cond
                                        #   in Loop: Header=BB8_1 Depth=1
	jmpq	*.LJTI8_0(,%rax,8)
.LBB8_3:                                # %sw.bb2.i
                                        #   in Loop: Header=BB8_1 Depth=1
	.loc	1 74 0                  # libc.c:74:0
	movq	%rdx, %rcx
	shrq	$8, %rcx
.Ltmp84:
	#DEBUG_VALUE: r <- RCX+0
	jmpq	.LBB8_10
.Ltmp85:
.LBB8_4:                                # %sw.bb4.i
                                        #   in Loop: Header=BB8_1 Depth=1
	.loc	1 75 0                  # libc.c:75:0
	movq	%rdx, %rcx
	shrq	$16, %rcx
.Ltmp86:
	#DEBUG_VALUE: r <- RCX+0
	jmpq	.LBB8_10
.Ltmp87:
.LBB8_5:                                # %sw.bb6.i
                                        #   in Loop: Header=BB8_1 Depth=1
	.loc	1 76 0                  # libc.c:76:0
	movq	%rdx, %rcx
	shrq	$24, %rcx
.Ltmp88:
	#DEBUG_VALUE: r <- RCX+0
	jmpq	.LBB8_10
.Ltmp89:
.LBB8_6:                                # %sw.bb8.i
                                        #   in Loop: Header=BB8_1 Depth=1
	.loc	1 77 0                  # libc.c:77:0
	movq	%rdx, %rcx
	shrq	$32, %rcx
.Ltmp90:
	#DEBUG_VALUE: r <- RCX+0
	jmpq	.LBB8_10
.Ltmp91:
.LBB8_7:                                # %sw.bb10.i
                                        #   in Loop: Header=BB8_1 Depth=1
	.loc	1 78 0                  # libc.c:78:0
	movq	%rdx, %rcx
	shrq	$40, %rcx
.Ltmp92:
	#DEBUG_VALUE: r <- RCX+0
	jmpq	.LBB8_10
.Ltmp93:
.LBB8_8:                                # %sw.bb12.i
                                        #   in Loop: Header=BB8_1 Depth=1
	.loc	1 79 0                  # libc.c:79:0
	movq	%rdx, %rcx
	shrq	$48, %rcx
.Ltmp94:
	#DEBUG_VALUE: r <- RCX+0
	jmpq	.LBB8_10
.Ltmp95:
.LBB8_9:                                # %sw.bb14.i
                                        #   in Loop: Header=BB8_1 Depth=1
	.loc	1 80 0                  # libc.c:80:0
	movq	%rdx, %rcx
	shrq	$56, %rcx
.Ltmp96:
	#DEBUG_VALUE: r <- RCX+0
	.align	16, 0x90
.LBB8_10:                               # %LLreadchar.exit
                                        #   in Loop: Header=BB8_1 Depth=1
	#DEBUG_VALUE: n <- 0+0
	#DEBUG_VALUE: neg <- 0+0
	#DEBUG_VALUE: LLreadchar:c <- RDI+0
	#DEBUG_VALUE: v <- RDI+0
	#DEBUG_VALUE: p <- RAX+0
	#DEBUG_VALUE: r <- RDX+0
	.loc	1 89 0                  # libc.c:89:0
	incq	%rdi
.Ltmp97:
	#DEBUG_VALUE: atol:s <- RDI+0
	.loc	1 82 0                  # libc.c:82:0
	andq	$255, %rcx
.Ltmp98:
	#DEBUG_VALUE: isspace:c <- RCX+0
	.loc	1 59 0                  # libc.c:59:0
	cmpq	$32, %rcx
	leaq	-9(%rcx), %rcx
.Ltmp99:
	je	.LBB8_1
	jmpq	.LBB8_11
.Ltmp100:
.LBB8_11:                               # %LLreadchar.exit
                                        #   in Loop: Header=BB8_1 Depth=1
	#DEBUG_VALUE: atol:s <- RDI+0
	cmpq	$5, %rcx
	jl	.LBB8_1
	jmpq	.LBB8_12
.LBB8_12:                               # %while.end
	#DEBUG_VALUE: atol:s <- RDI+0
	.loc	1 72 0                  # libc.c:72:0
.Ltmp101:
	cmpq	$6, %rax
	ja	.LBB8_21
# BB#13:                                # %while.end
	#DEBUG_VALUE: atol:s <- RDI+0
	jmpq	*.LJTI8_1(,%rax,8)
.LBB8_14:                               # %sw.bb2.i62
	#DEBUG_VALUE: atol:s <- RDI+0
	.loc	1 74 0                  # libc.c:74:0
	shrq	$8, %rdx
.Ltmp102:
	#DEBUG_VALUE: r <- RDX+0
	jmpq	.LBB8_21
.Ltmp103:
.LBB8_15:                               # %sw.bb4.i64
	#DEBUG_VALUE: atol:s <- RDI+0
	.loc	1 75 0                  # libc.c:75:0
	shrq	$16, %rdx
.Ltmp104:
	#DEBUG_VALUE: r <- RDX+0
	jmpq	.LBB8_21
.Ltmp105:
.LBB8_16:                               # %sw.bb6.i66
	#DEBUG_VALUE: atol:s <- RDI+0
	.loc	1 76 0                  # libc.c:76:0
	shrq	$24, %rdx
.Ltmp106:
	#DEBUG_VALUE: r <- RDX+0
	jmpq	.LBB8_21
.Ltmp107:
.LBB8_17:                               # %sw.bb8.i68
	#DEBUG_VALUE: atol:s <- RDI+0
	.loc	1 77 0                  # libc.c:77:0
	shrq	$32, %rdx
.Ltmp108:
	#DEBUG_VALUE: r <- RDX+0
	jmpq	.LBB8_21
.Ltmp109:
.LBB8_18:                               # %sw.bb10.i70
	#DEBUG_VALUE: atol:s <- RDI+0
	.loc	1 78 0                  # libc.c:78:0
	shrq	$40, %rdx
.Ltmp110:
	#DEBUG_VALUE: r <- RDX+0
	jmpq	.LBB8_21
.Ltmp111:
.LBB8_19:                               # %sw.bb12.i72
	#DEBUG_VALUE: atol:s <- RDI+0
	.loc	1 79 0                  # libc.c:79:0
	shrq	$48, %rdx
.Ltmp112:
	#DEBUG_VALUE: r <- RDX+0
	jmpq	.LBB8_21
.Ltmp113:
.LBB8_20:                               # %sw.bb14.i74
	#DEBUG_VALUE: atol:s <- RDI+0
	.loc	1 80 0                  # libc.c:80:0
	shrq	$56, %rdx
.Ltmp114:
	#DEBUG_VALUE: r <- RDX+0
.LBB8_21:                               # %LLreadchar.exit77
	#DEBUG_VALUE: atol:s <- RDI+0
	.loc	1 82 0                  # libc.c:82:0
	andq	$255, %rdx
	xorq	%r8, %r8
.Ltmp115:
	.loc	1 90 0                  # libc.c:90:0
	cmpq	$43, %rdx
	je	.LBB8_24
	jmpq	.LBB8_22
.LBB8_22:                               # %LLreadchar.exit77
	#DEBUG_VALUE: atol:s <- RDI+0
	leaq	-1(%rdi), %rax
	cmpq	$45, %rdx
	jne	.LBB8_25
	jmpq	.LBB8_23
.LBB8_23:                               # %sw.bb
	#DEBUG_VALUE: atol:s <- RDI+0
	movabsq	$1, %r8
.Ltmp116:
	#DEBUG_VALUE: neg <- 1+0
.LBB8_24:                               # %sw.bb3
	#DEBUG_VALUE: atol:s <- RDI+0
	movq	%rdi, %rax
.Ltmp117:
	#DEBUG_VALUE: atol:s <- RAX+0
.LBB8_25:                               # %while.cond5.preheader
	xorq	%rdx, %rdx
	jmp	.LBB8_26
	.align	16, 0x90
.LBB8_45:                               # %LLreadchar.exit39
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 96 0                  # libc.c:96:0
	incq	%rax
.Ltmp118:
	#DEBUG_VALUE: atol:s <- RAX+0
	shlq	$1, %rdx
	leaq	(%rdx,%rdx,4), %rdx
	.loc	1 82 0                  # libc.c:82:0
.Ltmp119:
	andq	$255, %rcx
.Ltmp120:
	.loc	1 96 0                  # libc.c:96:0
	addq	$48, %rdx
	subq	%rcx, %rdx
.Ltmp121:
	#DEBUG_VALUE: n <- RDX+0
	jmpq	.LBB8_26
.Ltmp122:
.LBB8_26:                               # %while.cond5
                                        # =>This Inner Loop Header: Depth=1
	#DEBUG_VALUE: LLreadchar:c <- RAX+0
	#DEBUG_VALUE: v <- RAX+0
	#DEBUG_VALUE: v <- RAX+0
	.loc	1 70 0                  # libc.c:70:0
	movq	%rax, %rcx
	andq	$-8, %rcx
	.loc	1 71 0                  # libc.c:71:0
	movq	(%rcx), %rcx
.Ltmp123:
	#DEBUG_VALUE: r <- RCX+0
	#DEBUG_VALUE: r <- RCX+0
	#DEBUG_VALUE: p <- RCX+0
	#DEBUG_VALUE: p <- RCX+0
	.loc	1 72 0                  # libc.c:72:0
	movq	%rax, %rdi
	andq	$7, %rdi
	decq	%rdi
	cmpq	$6, %rdi
	movq	%rcx, %rsi
	ja	.LBB8_35
.Ltmp124:
# BB#27:                                # %while.cond5
                                        #   in Loop: Header=BB8_26 Depth=1
	jmpq	*.LJTI8_2(,%rdi,8)
.LBB8_28:                               # %sw.bb2.i43
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 74 0                  # libc.c:74:0
	movq	%rcx, %rsi
	shrq	$8, %rsi
.Ltmp125:
	#DEBUG_VALUE: r <- RSI+0
	jmpq	.LBB8_35
.Ltmp126:
.LBB8_29:                               # %sw.bb4.i45
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 75 0                  # libc.c:75:0
	movq	%rcx, %rsi
	shrq	$16, %rsi
.Ltmp127:
	#DEBUG_VALUE: r <- RSI+0
	jmpq	.LBB8_35
.Ltmp128:
.LBB8_30:                               # %sw.bb6.i47
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 76 0                  # libc.c:76:0
	movq	%rcx, %rsi
	shrq	$24, %rsi
.Ltmp129:
	#DEBUG_VALUE: r <- RSI+0
	jmpq	.LBB8_35
.Ltmp130:
.LBB8_31:                               # %sw.bb8.i49
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 77 0                  # libc.c:77:0
	movq	%rcx, %rsi
	shrq	$32, %rsi
.Ltmp131:
	#DEBUG_VALUE: r <- RSI+0
	jmpq	.LBB8_35
.Ltmp132:
.LBB8_32:                               # %sw.bb10.i51
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 78 0                  # libc.c:78:0
	movq	%rcx, %rsi
	shrq	$40, %rsi
.Ltmp133:
	#DEBUG_VALUE: r <- RSI+0
	jmpq	.LBB8_35
.Ltmp134:
.LBB8_33:                               # %sw.bb12.i53
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 79 0                  # libc.c:79:0
	movq	%rcx, %rsi
	shrq	$48, %rsi
.Ltmp135:
	#DEBUG_VALUE: r <- RSI+0
	jmpq	.LBB8_35
.Ltmp136:
.LBB8_34:                               # %sw.bb14.i55
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 80 0                  # libc.c:80:0
	movq	%rcx, %rsi
	shrq	$56, %rsi
.Ltmp137:
	#DEBUG_VALUE: r <- RSI+0
	.align	16, 0x90
.LBB8_35:                               # %LLreadchar.exit58
                                        #   in Loop: Header=BB8_26 Depth=1
	#DEBUG_VALUE: LLreadchar:c <- RAX+0
	#DEBUG_VALUE: v <- RAX+0
	#DEBUG_VALUE: p <- RCX+0
	#DEBUG_VALUE: r <- RCX+0
	.loc	1 82 0                  # libc.c:82:0
	andq	$255, %rsi
.Ltmp138:
	#DEBUG_VALUE: isdigit:c <- RSI+0
	.loc	1 64 0                  # libc.c:64:0
	addq	$-48, %rsi
.Ltmp139:
	.loc	1 95 0                  # libc.c:95:0
	cmpq	$9, %rsi
	ja	.LBB8_46
	jmpq	.LBB8_36
.Ltmp140:
.LBB8_36:                               # %while.body9
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 72 0                  # libc.c:72:0
	cmpq	$6, %rdi
	ja	.LBB8_45
# BB#37:                                # %while.body9
                                        #   in Loop: Header=BB8_26 Depth=1
	jmpq	*.LJTI8_3(,%rdi,8)
.LBB8_38:                               # %sw.bb2.i24
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 74 0                  # libc.c:74:0
	shrq	$8, %rcx
.Ltmp141:
	#DEBUG_VALUE: r <- RCX+0
	jmpq	.LBB8_45
.Ltmp142:
.LBB8_39:                               # %sw.bb4.i26
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 75 0                  # libc.c:75:0
	shrq	$16, %rcx
.Ltmp143:
	#DEBUG_VALUE: r <- RCX+0
	jmpq	.LBB8_45
.Ltmp144:
.LBB8_40:                               # %sw.bb6.i28
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 76 0                  # libc.c:76:0
	shrq	$24, %rcx
.Ltmp145:
	#DEBUG_VALUE: r <- RCX+0
	jmpq	.LBB8_45
.Ltmp146:
.LBB8_41:                               # %sw.bb8.i30
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 77 0                  # libc.c:77:0
	shrq	$32, %rcx
.Ltmp147:
	#DEBUG_VALUE: r <- RCX+0
	jmpq	.LBB8_45
.Ltmp148:
.LBB8_42:                               # %sw.bb10.i32
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 78 0                  # libc.c:78:0
	shrq	$40, %rcx
.Ltmp149:
	#DEBUG_VALUE: r <- RCX+0
	jmpq	.LBB8_45
.Ltmp150:
.LBB8_43:                               # %sw.bb12.i34
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 79 0                  # libc.c:79:0
	shrq	$48, %rcx
.Ltmp151:
	#DEBUG_VALUE: r <- RCX+0
	jmpq	.LBB8_45
.Ltmp152:
.LBB8_44:                               # %sw.bb14.i36
                                        #   in Loop: Header=BB8_26 Depth=1
	.loc	1 80 0                  # libc.c:80:0
	shrq	$56, %rcx
.Ltmp153:
	#DEBUG_VALUE: r <- RCX+0
	jmp	.LBB8_45
.Ltmp154:
.LBB8_46:                               # %while.end13
	.loc	1 97 0                  # libc.c:97:0
	movq	%rdx, %rax
	negq	%rax
	testq	%r8, %r8
	pushfq
	popq	%rcx
	testq	%rcx, %rcx
	cmovneq	%rdx, %rax
	ret
.Ltmp155:
.Ltmp156:
	.size	atol, .Ltmp156-atol
.Lfunc_end8:
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
.Lfunc_begin9:
	.loc	1 101 0                 # libc.c:101:0
# BB#0:                                 # %entry
	#DEBUG_VALUE: time:t <- RDI+0
	movq	%rdi, %rdx
.Ltmp157:
	#DEBUG_VALUE: __syscall2:n <- 228+0
	#DEBUG_VALUE: __syscall2:a1 <- 0+0
	#DEBUG_VALUE: time:t <- RDX+0
	leaq	-16(%rsp), %rsi
	movabsq	$228, %rax
	xorq	%rdi, %rdi
	.loc	2 331 0 prologue_end    # ./syscall.h:331:0
.Ltmp158:
	#APP
	syscall
	#NO_APP
.Ltmp159:
	.loc	1 105 0                 # libc.c:105:0
	movq	-16(%rsp), %rax
	.loc	1 104 0                 # libc.c:104:0
	testq	%rdx, %rdx
	je	.LBB9_2
	jmpq	.LBB9_1
.LBB9_1:                                # %if.then
.Ltmp160:
	#DEBUG_VALUE: time:t <- RDX+0
	movq	%rax, (%rdx)
.Ltmp161:
.LBB9_2:                                # %if.end
	.loc	1 105 0                 # libc.c:105:0
	ret
.Ltmp162:
.Ltmp163:
	.size	time, .Ltmp163-time
.Lfunc_end9:
	.file	3 "./libc.h"
	.cfi_endproc

	.globl	__libc_start_main
	.align	16, 0x90
	.type	__libc_start_main,@function
__libc_start_main:                      # @__libc_start_main
	.cfi_startproc
.Lfunc_begin10:
	.loc	1 112 0                 # libc.c:112:0
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp165:
	.cfi_def_cfa_offset 16
	#DEBUG_VALUE: __libc_start_main:main <- RDI+0
	#DEBUG_VALUE: __libc_start_main:argc <- RSI+0
	#DEBUG_VALUE: __libc_start_main:argv <- RDX+0
	#DEBUG_VALUE: __libc_start_main:init <- RCX+0
	#DEBUG_VALUE: __libc_start_main:fini <- R8+0
	#DEBUG_VALUE: __libc_start_main:ldso_fini <- R9+0
	movq	%rdi, %r8
.Ltmp166:
	#DEBUG_VALUE: __syscall1:n <- 12+0
	#DEBUG_VALUE: __syscall1:a1 <- 0+0
	#DEBUG_VALUE: __libc_start_main:main <- R8+0
	movabsq	$12, %rax
	xorq	%rdi, %rdi
	.loc	2 324 0 prologue_end    # ./syscall.h:324:0
.Ltmp167:
	#APP
	syscall
	#NO_APP
.Ltmp168:
	#DEBUG_VALUE: ret <- RAX+0
	.loc	1 123 0                 # libc.c:123:0
	movq	%rax, current_brk(%rip)
	.loc	1 113 0                 # libc.c:113:0
	leaq	8(%rdx,%rsi,8), %rax
.Ltmp169:
	#DEBUG_VALUE: envp <- RAX+0
	.loc	1 126 0                 # libc.c:126:0
	movq	%rsi, %rdi
	movq	%rdx, %rsi
.Ltmp170:
	movq	%rax, %rdx
.Ltmp171:
	callq	*%r8
.Ltmp172:
	movq	%rax, %rcx
.Ltmp173:
	#DEBUG_VALUE: __syscall1:n <- 60+0
	#DEBUG_VALUE: __syscall1:a1 <- RCX+0
	#DEBUG_VALUE: llexit:s <- RCX+0
	movabsq	$60, %rax
	.loc	2 324 0                 # ./syscall.h:324:0
.Ltmp174:
	movq	%rcx, %rdi
	#APP
	syscall
	#NO_APP
.Ltmp175:
	xorq	%rax, %rax
.Ltmp176:
	.loc	1 130 0                 # libc.c:130:0
	popq	%rdx
	ret
.Ltmp177:
.Ltmp178:
	.size	__libc_start_main, .Ltmp178-__libc_start_main
.Lfunc_end10:
	.cfi_endproc

	.type	current_brk,@object     # @current_brk
	.comm	current_brk,8,8
.Ltext_end:
	.data
.Ldata_end:
	.text
.Lsection_end1:
	.section	.debug_info,"",@progbits
.Lsection_info:
	.section	.debug_abbrev,"",@progbits
.Lsection_abbrev:
	.section	.debug_aranges,"",@progbits
	.section	.debug_macinfo,"",@progbits
	.section	.debug_line,"",@progbits
.Lsection_line:
	.section	.debug_loc,"",@progbits
	.section	.debug_pubtypes,"",@progbits
	.section	.debug_str,"MS",@progbits,1
.Linfo_string:
	.section	.debug_ranges,"",@progbits
.Ldebug_range:
	.section	.debug_loc,"",@progbits
.Lsection_debug_loc:
	.text
.Ltext_begin:
	.data
	.section	.debug_info,"",@progbits
.L.debug_info_begin0:
	.long	2138                    # Length of Compilation Unit Info
	.short	2                       # DWARF version number
	.long	.L.debug_abbrev_begin   # Offset Into Abbrev. Section
	.byte	8                       # Address Size (in bytes)
	.byte	1                       # Abbrev [1] 0xb:0x853 DW_TAG_compile_unit
	.long	.Linfo_string0          # DW_AT_producer
	.short	12                      # DW_AT_language
	.long	.Linfo_string1          # DW_AT_name
	.quad	0                       # DW_AT_low_pc
	.long	.Lsection_line          # DW_AT_stmt_list
	.long	.Linfo_string2          # DW_AT_comp_dir
                                        # DW_AT_APPLE_optimized
	.byte	2                       # Abbrev [2] 0x26:0x7 DW_TAG_base_type
	.long	.Linfo_string4          # DW_AT_name
	.byte	6                       # DW_AT_encoding
	.byte	1                       # DW_AT_byte_size
	.byte	3                       # Abbrev [3] 0x2d:0x5 DW_TAG_pointer_type
	.long	38                      # DW_AT_type
	.byte	4                       # Abbrev [4] 0x32:0x15 DW_TAG_variable
	.long	.Linfo_string3          # DW_AT_name
	.long	45                      # DW_AT_type
                                        # DW_AT_external
	.byte	1                       # DW_AT_decl_file
	.byte	48                      # DW_AT_decl_line
	.byte	9                       # DW_AT_location
	.byte	3
	.quad	current_brk
	.byte	5                       # Abbrev [5] 0x47:0x6b DW_TAG_subprogram
	.long	.Linfo_string5          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	6                       # DW_AT_decl_line
                                        # DW_AT_prototyped
                                        # DW_AT_external
	.quad	.Lfunc_begin0           # DW_AT_low_pc
	.quad	.Lfunc_end0             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	1                       # DW_AT_inline
	.byte	6                       # Abbrev [6] 0x61:0xf DW_TAG_formal_parameter
	.long	.Linfo_string28         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	6                       # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.long	.Ldebug_loc0            # DW_AT_location
	.byte	7                       # Abbrev [7] 0x70:0x41 DW_TAG_lexical_block
	.quad	.Ltmp0                  # DW_AT_low_pc
	.quad	.Ltmp2                  # DW_AT_high_pc
	.byte	8                       # Abbrev [8] 0x81:0x2f DW_TAG_inlined_subroutine
	.long	1956                    # DW_AT_abstract_origin
	.quad	.Ltmp0                  # DW_AT_low_pc
	.quad	.Ltmp1                  # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	7                       # DW_AT_call_line
	.byte	9                       # Abbrev [9] 0x98:0xe DW_TAG_formal_parameter
	.long	1969                    # DW_AT_abstract_origin
	.byte	8                       # DW_AT_const_value
	.quad	60
	.byte	10                      # Abbrev [10] 0xa6:0x9 DW_TAG_formal_parameter
	.long	1981                    # DW_AT_abstract_origin
	.long	.Ldebug_loc2            # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	11                      # Abbrev [11] 0xb2:0x6c DW_TAG_subprogram
	.long	.Linfo_string6          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	10                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	293                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin1           # DW_AT_low_pc
	.quad	.Lfunc_end1             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	6                       # Abbrev [6] 0xcf:0xf DW_TAG_formal_parameter
	.long	.Linfo_string28         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	10                      # DW_AT_decl_line
	.long	2013                    # DW_AT_type
	.long	.Ldebug_loc4            # DW_AT_location
	.byte	7                       # Abbrev [7] 0xde:0x3f DW_TAG_lexical_block
	.quad	.Lfunc_begin1           # DW_AT_low_pc
	.quad	.Ltmp26                 # DW_AT_high_pc
	.byte	12                      # Abbrev [12] 0xef:0xf DW_TAG_variable
	.long	.Linfo_string29         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	12                      # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.long	.Ldebug_loc6            # DW_AT_location
	.byte	12                      # Abbrev [12] 0xfe:0xf DW_TAG_variable
	.long	.Linfo_string30         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	13                      # DW_AT_decl_line
	.long	2018                    # DW_AT_type
	.long	.Ldebug_loc8            # DW_AT_location
	.byte	12                      # Abbrev [12] 0x10d:0xf DW_TAG_variable
	.long	.Linfo_string31         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	11                      # DW_AT_decl_line
	.long	918                     # DW_AT_type
	.long	.Ldebug_loc12           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	2                       # Abbrev [2] 0x11e:0x7 DW_TAG_base_type
	.long	.Linfo_string7          # DW_AT_name
	.byte	7                       # DW_AT_encoding
	.byte	8                       # DW_AT_byte_size
	.byte	13                      # Abbrev [13] 0x125:0xb DW_TAG_typedef
	.long	286                     # DW_AT_type
	.long	.Linfo_string8          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	15                      # DW_AT_decl_line
	.byte	11                      # Abbrev [11] 0x130:0x7c DW_TAG_subprogram
	.long	.Linfo_string9          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	38                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	428                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin2           # DW_AT_low_pc
	.quad	.Lfunc_end2             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	6                       # Abbrev [6] 0x14d:0xf DW_TAG_formal_parameter
	.long	.Linfo_string28         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	38                      # DW_AT_decl_line
	.long	2013                    # DW_AT_type
	.long	.Ldebug_loc16           # DW_AT_location
	.byte	7                       # Abbrev [7] 0x15c:0x4f DW_TAG_lexical_block
	.quad	.Ltmp32                 # DW_AT_low_pc
	.quad	.Ltmp37                 # DW_AT_high_pc
	.byte	8                       # Abbrev [8] 0x16d:0x3d DW_TAG_inlined_subroutine
	.long	1880                    # DW_AT_abstract_origin
	.quad	.Ltmp34                 # DW_AT_low_pc
	.quad	.Ltmp35                 # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	39                      # DW_AT_call_line
	.byte	9                       # Abbrev [9] 0x184:0xe DW_TAG_formal_parameter
	.long	1893                    # DW_AT_abstract_origin
	.byte	8                       # DW_AT_const_value
	.quad	1
	.byte	9                       # Abbrev [9] 0x192:0xe DW_TAG_formal_parameter
	.long	1905                    # DW_AT_abstract_origin
	.byte	8                       # DW_AT_const_value
	.quad	1
	.byte	14                      # Abbrev [14] 0x1a0:0x9 DW_TAG_variable
	.long	1942                    # DW_AT_abstract_origin
	.long	.Ldebug_loc19           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	2                       # Abbrev [2] 0x1ac:0x7 DW_TAG_base_type
	.long	.Linfo_string10         # DW_AT_name
	.byte	5                       # DW_AT_encoding
	.byte	8                       # DW_AT_byte_size
	.byte	11                      # Abbrev [11] 0x1b3:0x9f DW_TAG_subprogram
	.long	.Linfo_string11         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	42                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	428                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin3           # DW_AT_low_pc
	.quad	.Lfunc_end3             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	6                       # Abbrev [6] 0x1d0:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	42                      # DW_AT_decl_line
	.long	918                     # DW_AT_type
	.long	.Ldebug_loc21           # DW_AT_location
	.byte	7                       # Abbrev [7] 0x1df:0x72 DW_TAG_lexical_block
	.quad	.Lfunc_begin3           # DW_AT_low_pc
	.quad	.Ltmp43                 # DW_AT_high_pc
	.byte	15                      # Abbrev [15] 0x1f0:0xe DW_TAG_variable
	.long	.Linfo_string28         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	43                      # DW_AT_decl_line
	.long	2030                    # DW_AT_type
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	120
	.byte	8                       # Abbrev [8] 0x1fe:0x52 DW_TAG_inlined_subroutine
	.long	1880                    # DW_AT_abstract_origin
	.quad	.Ltmp41                 # DW_AT_low_pc
	.quad	.Ltmp42                 # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	45                      # DW_AT_call_line
	.byte	16                      # Abbrev [16] 0x215:0x15 DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	8                       # DW_AT_const_value
	.quad	1
	.byte	16                      # Abbrev [16] 0x22a:0x15 DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	8                       # DW_AT_const_value
	.quad	1
	.byte	17                      # Abbrev [17] 0x23f:0x10 DW_TAG_variable
	.long	.Linfo_string27         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	338                     # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.long	.Ldebug_loc23           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	11                      # Abbrev [11] 0x252:0x7c DW_TAG_subprogram
	.long	.Linfo_string12         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	49                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	718                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin4           # DW_AT_low_pc
	.quad	.Lfunc_end4             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	6                       # Abbrev [6] 0x26f:0xf DW_TAG_formal_parameter
	.long	.Linfo_string36         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	49                      # DW_AT_decl_line
	.long	2042                    # DW_AT_type
	.long	.Ldebug_loc25           # DW_AT_location
	.byte	7                       # Abbrev [7] 0x27e:0x4f DW_TAG_lexical_block
	.quad	.Ltmp45                 # DW_AT_low_pc
	.quad	.Ltmp51                 # DW_AT_high_pc
	.byte	8                       # Abbrev [8] 0x28f:0x3d DW_TAG_inlined_subroutine
	.long	1956                    # DW_AT_abstract_origin
	.quad	.Ltmp49                 # DW_AT_low_pc
	.quad	.Ltmp50                 # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	52                      # DW_AT_call_line
	.byte	16                      # Abbrev [16] 0x2a6:0x15 DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	8                       # DW_AT_const_value
	.quad	12
	.byte	18                      # Abbrev [18] 0x2bb:0x10 DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.long	.Ldebug_loc29           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	19                      # Abbrev [19] 0x2ce:0x1 DW_TAG_pointer_type
	.byte	11                      # Abbrev [11] 0x2cf:0x2d DW_TAG_subprogram
	.long	.Linfo_string13         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	57                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	428                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin5           # DW_AT_low_pc
	.quad	.Lfunc_end5             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	6                       # Abbrev [6] 0x2ec:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	57                      # DW_AT_decl_line
	.long	918                     # DW_AT_type
	.long	.Ldebug_loc31           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	11                      # Abbrev [11] 0x2fc:0x2d DW_TAG_subprogram
	.long	.Linfo_string14         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	62                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	428                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin6           # DW_AT_low_pc
	.quad	.Lfunc_end6             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	6                       # Abbrev [6] 0x319:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	62                      # DW_AT_decl_line
	.long	918                     # DW_AT_type
	.long	.Ldebug_loc34           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	20                      # Abbrev [20] 0x329:0x6d DW_TAG_subprogram
	.long	.Linfo_string15         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	67                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	918                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin7           # DW_AT_low_pc
	.quad	.Lfunc_end7             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	1                       # DW_AT_inline
	.byte	6                       # Abbrev [6] 0x347:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	67                      # DW_AT_decl_line
	.long	2013                    # DW_AT_type
	.long	.Ldebug_loc36           # DW_AT_location
	.byte	7                       # Abbrev [7] 0x356:0x3f DW_TAG_lexical_block
	.quad	.Lfunc_begin7           # DW_AT_low_pc
	.quad	.Ltmp79                 # DW_AT_high_pc
	.byte	12                      # Abbrev [12] 0x367:0xf DW_TAG_variable
	.long	.Linfo_string29         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	69                      # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.long	.Ldebug_loc38           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x376:0xf DW_TAG_variable
	.long	.Linfo_string38         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	68                      # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.long	.Ldebug_loc40           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x385:0xf DW_TAG_variable
	.long	.Linfo_string30         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	70                      # DW_AT_decl_line
	.long	2018                    # DW_AT_type
	.long	.Ldebug_loc49           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	13                      # Abbrev [13] 0x396:0xb DW_TAG_typedef
	.long	428                     # DW_AT_type
	.long	.Linfo_string16         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	4                       # DW_AT_decl_line
	.byte	11                      # Abbrev [11] 0x3a1:0x195 DW_TAG_subprogram
	.long	.Linfo_string17         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	85                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	1334                    # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin8           # DW_AT_low_pc
	.quad	.Lfunc_end8             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	6                       # Abbrev [6] 0x3be:0xf DW_TAG_formal_parameter
	.long	.Linfo_string28         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	85                      # DW_AT_decl_line
	.long	2013                    # DW_AT_type
	.long	.Ldebug_loc52           # DW_AT_location
	.byte	7                       # Abbrev [7] 0x3cd:0x168 DW_TAG_lexical_block
	.quad	.Lfunc_begin8           # DW_AT_low_pc
	.quad	.Ltmp155                # DW_AT_high_pc
	.byte	12                      # Abbrev [12] 0x3de:0xf DW_TAG_variable
	.long	.Linfo_string25         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	87                      # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.long	.Ldebug_loc56           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x3ed:0xf DW_TAG_variable
	.long	.Linfo_string39         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	88                      # DW_AT_decl_line
	.long	918                     # DW_AT_type
	.long	.Ldebug_loc59           # DW_AT_location
	.byte	21                      # Abbrev [21] 0x3fc:0x57 DW_TAG_inlined_subroutine
	.long	809                     # DW_AT_abstract_origin
	.long	.Ldebug_range           # DW_AT_ranges
	.byte	1                       # DW_AT_call_file
	.byte	89                      # DW_AT_call_line
	.byte	6                       # Abbrev [6] 0x407:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	67                      # DW_AT_decl_line
	.long	2013                    # DW_AT_type
	.long	.Ldebug_loc62           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x416:0xf DW_TAG_variable
	.long	.Linfo_string29         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	69                      # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.long	.Ldebug_loc67           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x425:0xf DW_TAG_variable
	.long	.Linfo_string38         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	68                      # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.long	.Ldebug_loc70           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x434:0xf DW_TAG_variable
	.long	.Linfo_string30         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	70                      # DW_AT_decl_line
	.long	2018                    # DW_AT_type
	.long	.Ldebug_loc91           # DW_AT_location
	.byte	6                       # Abbrev [6] 0x443:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	57                      # DW_AT_decl_line
	.long	918                     # DW_AT_type
	.long	.Ldebug_loc94           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	8                       # Abbrev [8] 0x453:0x45 DW_TAG_inlined_subroutine
	.long	809                     # DW_AT_abstract_origin
	.quad	.Ltmp101                # DW_AT_low_pc
	.quad	.Ltmp115                # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	90                      # DW_AT_call_line
	.byte	12                      # Abbrev [12] 0x46a:0xf DW_TAG_variable
	.long	.Linfo_string29         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	69                      # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.long	.Ldebug_loc65           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x479:0xf DW_TAG_variable
	.long	.Linfo_string38         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	68                      # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.long	.Ldebug_loc80           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x488:0xf DW_TAG_variable
	.long	.Linfo_string30         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	70                      # DW_AT_decl_line
	.long	2018                    # DW_AT_type
	.long	.Ldebug_loc89           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	21                      # Abbrev [21] 0x498:0x39 DW_TAG_inlined_subroutine
	.long	809                     # DW_AT_abstract_origin
	.long	.Ldebug_range+48        # DW_AT_ranges
	.byte	1                       # DW_AT_call_file
	.byte	96                      # DW_AT_call_line
	.byte	12                      # Abbrev [12] 0x4a3:0xf DW_TAG_variable
	.long	.Linfo_string29         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	69                      # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.long	.Ldebug_loc99           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x4b2:0xf DW_TAG_variable
	.long	.Linfo_string38         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	68                      # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.long	.Ldebug_loc114          # DW_AT_location
	.byte	12                      # Abbrev [12] 0x4c1:0xf DW_TAG_variable
	.long	.Linfo_string30         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	70                      # DW_AT_decl_line
	.long	2018                    # DW_AT_type
	.long	.Ldebug_loc123          # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	8                       # Abbrev [8] 0x4d1:0x63 DW_TAG_inlined_subroutine
	.long	809                     # DW_AT_abstract_origin
	.quad	.Ltmp122                # DW_AT_low_pc
	.quad	.Ltmp139                # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	95                      # DW_AT_call_line
	.byte	6                       # Abbrev [6] 0x4e8:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	67                      # DW_AT_decl_line
	.long	2013                    # DW_AT_type
	.long	.Ldebug_loc96           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x4f7:0xf DW_TAG_variable
	.long	.Linfo_string29         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	69                      # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.long	.Ldebug_loc101          # DW_AT_location
	.byte	12                      # Abbrev [12] 0x506:0xf DW_TAG_variable
	.long	.Linfo_string38         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	68                      # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.long	.Ldebug_loc104          # DW_AT_location
	.byte	12                      # Abbrev [12] 0x515:0xf DW_TAG_variable
	.long	.Linfo_string30         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	70                      # DW_AT_decl_line
	.long	2018                    # DW_AT_type
	.long	.Ldebug_loc125          # DW_AT_location
	.byte	6                       # Abbrev [6] 0x524:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	62                      # DW_AT_decl_line
	.long	918                     # DW_AT_type
	.long	.Ldebug_loc128          # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	2                       # Abbrev [2] 0x536:0x7 DW_TAG_base_type
	.long	.Linfo_string18         # DW_AT_name
	.byte	5                       # DW_AT_encoding
	.byte	8                       # DW_AT_byte_size
	.byte	11                      # Abbrev [11] 0x53d:0x81 DW_TAG_subprogram
	.long	.Linfo_string19         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	100                     # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	1470                    # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin9           # DW_AT_low_pc
	.quad	.Lfunc_end9             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	6                       # Abbrev [6] 0x55a:0xf DW_TAG_formal_parameter
	.long	.Linfo_string40         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	100                     # DW_AT_decl_line
	.long	2049                    # DW_AT_type
	.long	.Ldebug_loc130          # DW_AT_location
	.byte	7                       # Abbrev [7] 0x569:0x54 DW_TAG_lexical_block
	.quad	.Ltmp158                # DW_AT_low_pc
	.quad	.Ltmp162                # DW_AT_high_pc
	.byte	15                      # Abbrev [15] 0x57a:0xe DW_TAG_variable
	.long	.Linfo_string41         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	102                     # DW_AT_decl_line
	.long	2065                    # DW_AT_type
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	112
	.byte	8                       # Abbrev [8] 0x588:0x34 DW_TAG_inlined_subroutine
	.long	1816                    # DW_AT_abstract_origin
	.quad	.Ltmp158                # DW_AT_low_pc
	.quad	.Ltmp159                # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	103                     # DW_AT_call_line
	.byte	9                       # Abbrev [9] 0x59f:0xe DW_TAG_formal_parameter
	.long	1829                    # DW_AT_abstract_origin
	.byte	8                       # DW_AT_const_value
	.quad	228
	.byte	9                       # Abbrev [9] 0x5ad:0xe DW_TAG_formal_parameter
	.long	1841                    # DW_AT_abstract_origin
	.byte	8                       # DW_AT_const_value
	.quad	0
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	13                      # Abbrev [13] 0x5be:0xb DW_TAG_typedef
	.long	428                     # DW_AT_type
	.long	.Linfo_string20         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	14                      # DW_AT_decl_line
	.byte	11                      # Abbrev [11] 0x5c9:0x14f DW_TAG_subprogram
	.long	.Linfo_string21         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	108                     # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	918                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin10          # DW_AT_low_pc
	.quad	.Lfunc_end10            # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	6                       # Abbrev [6] 0x5e6:0xf DW_TAG_formal_parameter
	.long	.Linfo_string46         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	109                     # DW_AT_decl_line
	.long	2130                    # DW_AT_type
	.long	.Ldebug_loc134          # DW_AT_location
	.byte	6                       # Abbrev [6] 0x5f5:0xf DW_TAG_formal_parameter
	.long	.Linfo_string47         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	109                     # DW_AT_decl_line
	.long	918                     # DW_AT_type
	.long	.Ldebug_loc137          # DW_AT_location
	.byte	6                       # Abbrev [6] 0x604:0xf DW_TAG_formal_parameter
	.long	.Linfo_string48         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	109                     # DW_AT_decl_line
	.long	2104                    # DW_AT_type
	.long	.Ldebug_loc139          # DW_AT_location
	.byte	6                       # Abbrev [6] 0x613:0xf DW_TAG_formal_parameter
	.long	.Linfo_string49         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	110                     # DW_AT_decl_line
	.long	2130                    # DW_AT_type
	.long	.Ldebug_loc141          # DW_AT_location
	.byte	6                       # Abbrev [6] 0x622:0xf DW_TAG_formal_parameter
	.long	.Linfo_string50         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	110                     # DW_AT_decl_line
	.long	2136                    # DW_AT_type
	.long	.Ldebug_loc143          # DW_AT_location
	.byte	6                       # Abbrev [6] 0x631:0xf DW_TAG_formal_parameter
	.long	.Linfo_string51         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	111                     # DW_AT_decl_line
	.long	2136                    # DW_AT_type
	.long	.Ldebug_loc145          # DW_AT_location
	.byte	7                       # Abbrev [7] 0x640:0xd7 DW_TAG_lexical_block
	.quad	.Ltmp167                # DW_AT_low_pc
	.quad	.Ltmp177                # DW_AT_high_pc
	.byte	12                      # Abbrev [12] 0x651:0xf DW_TAG_variable
	.long	.Linfo_string52         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	113                     # DW_AT_decl_line
	.long	2104                    # DW_AT_type
	.long	.Ldebug_loc149          # DW_AT_location
	.byte	8                       # Abbrev [8] 0x660:0x52 DW_TAG_inlined_subroutine
	.long	1956                    # DW_AT_abstract_origin
	.quad	.Ltmp167                # DW_AT_low_pc
	.quad	.Ltmp168                # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	123                     # DW_AT_call_line
	.byte	16                      # Abbrev [16] 0x677:0x15 DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	8                       # DW_AT_const_value
	.quad	12
	.byte	16                      # Abbrev [16] 0x68c:0x15 DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	8                       # DW_AT_const_value
	.quad	0
	.byte	17                      # Abbrev [17] 0x6a1:0x10 DW_TAG_variable
	.long	.Linfo_string27         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	323                     # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.long	.Ldebug_loc147          # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	8                       # Abbrev [8] 0x6b2:0x64 DW_TAG_inlined_subroutine
	.long	71                      # DW_AT_abstract_origin
	.quad	.Ltmp174                # DW_AT_low_pc
	.quad	.Ltmp176                # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	126                     # DW_AT_call_line
	.byte	6                       # Abbrev [6] 0x6c9:0xf DW_TAG_formal_parameter
	.long	.Linfo_string28         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	6                       # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.long	.Ldebug_loc153          # DW_AT_location
	.byte	8                       # Abbrev [8] 0x6d8:0x3d DW_TAG_inlined_subroutine
	.long	1956                    # DW_AT_abstract_origin
	.quad	.Ltmp174                # DW_AT_low_pc
	.quad	.Ltmp176                # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	7                       # DW_AT_call_line
	.byte	16                      # Abbrev [16] 0x6ef:0x15 DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	8                       # DW_AT_const_value
	.quad	60
	.byte	18                      # Abbrev [18] 0x704:0x10 DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.long	.Ldebug_loc151          # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	22                      # Abbrev [22] 0x718:0x40 DW_TAG_subprogram
	.long	.Linfo_string22         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	328                     # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	428                     # DW_AT_type
	.byte	1                       # DW_AT_inline
	.byte	23                      # Abbrev [23] 0x725:0xc DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	328                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	23                      # Abbrev [23] 0x731:0xc DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	328                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	23                      # Abbrev [23] 0x73d:0xc DW_TAG_formal_parameter
	.long	.Linfo_string32         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	328                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	24                      # Abbrev [24] 0x749:0xe DW_TAG_lexical_block
	.byte	25                      # Abbrev [25] 0x74a:0xc DW_TAG_variable
	.long	.Linfo_string27         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	330                     # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	22                      # Abbrev [22] 0x758:0x4c DW_TAG_subprogram
	.long	.Linfo_string23         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	428                     # DW_AT_type
	.byte	1                       # DW_AT_inline
	.byte	23                      # Abbrev [23] 0x765:0xc DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	23                      # Abbrev [23] 0x771:0xc DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	23                      # Abbrev [23] 0x77d:0xc DW_TAG_formal_parameter
	.long	.Linfo_string32         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	23                      # Abbrev [23] 0x789:0xc DW_TAG_formal_parameter
	.long	.Linfo_string33         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	24                      # Abbrev [24] 0x795:0xe DW_TAG_lexical_block
	.byte	25                      # Abbrev [25] 0x796:0xc DW_TAG_variable
	.long	.Linfo_string27         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	338                     # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	22                      # Abbrev [22] 0x7a4:0x34 DW_TAG_subprogram
	.long	.Linfo_string24         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	428                     # DW_AT_type
	.byte	1                       # DW_AT_inline
	.byte	23                      # Abbrev [23] 0x7b1:0xc DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	23                      # Abbrev [23] 0x7bd:0xc DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	428                     # DW_AT_type
	.byte	24                      # Abbrev [24] 0x7c9:0xe DW_TAG_lexical_block
	.byte	25                      # Abbrev [25] 0x7ca:0xc DW_TAG_variable
	.long	.Linfo_string27         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	323                     # DW_AT_decl_line
	.long	286                     # DW_AT_type
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	26                      # Abbrev [26] 0x7d8:0x5 DW_TAG_const_type
	.long	38                      # DW_AT_type
	.byte	3                       # Abbrev [3] 0x7dd:0x5 DW_TAG_pointer_type
	.long	2008                    # DW_AT_type
	.byte	3                       # Abbrev [3] 0x7e2:0x5 DW_TAG_pointer_type
	.long	286                     # DW_AT_type
	.byte	27                      # Abbrev [27] 0x7e7:0x7 DW_TAG_base_type
	.long	.Linfo_string35         # DW_AT_name
	.byte	4                       # DW_AT_byte_size
	.byte	5                       # DW_AT_encoding
	.byte	28                      # Abbrev [28] 0x7ee:0xc DW_TAG_array_type
	.long	918                     # DW_AT_type
	.byte	29                      # Abbrev [29] 0x7f3:0x6 DW_TAG_subrange_type
	.long	2023                    # DW_AT_type
	.byte	0                       # DW_AT_upper_bound
	.byte	0                       # End Of Children Mark
	.byte	2                       # Abbrev [2] 0x7fa:0x7 DW_TAG_base_type
	.long	.Linfo_string37         # DW_AT_name
	.byte	7                       # DW_AT_encoding
	.byte	8                       # DW_AT_byte_size
	.byte	3                       # Abbrev [3] 0x801:0x5 DW_TAG_pointer_type
	.long	1470                    # DW_AT_type
	.byte	13                      # Abbrev [13] 0x806:0xb DW_TAG_typedef
	.long	428                     # DW_AT_type
	.long	.Linfo_string43         # DW_AT_name
	.byte	3                       # DW_AT_decl_file
	.byte	6                       # DW_AT_decl_line
	.byte	30                      # Abbrev [30] 0x811:0x27 DW_TAG_structure_type
	.long	.Linfo_string45         # DW_AT_name
	.byte	16                      # DW_AT_byte_size
	.byte	3                       # DW_AT_decl_file
	.byte	7                       # DW_AT_decl_line
	.byte	31                      # Abbrev [31] 0x819:0xf DW_TAG_member
	.long	.Linfo_string42         # DW_AT_name
	.long	2054                    # DW_AT_type
	.byte	3                       # DW_AT_decl_file
	.byte	9                       # DW_AT_decl_line
	.byte	2                       # DW_AT_data_member_location
	.byte	35
	.byte	0
	.byte	1                       # DW_AT_accessibility
                                        # DW_ACCESS_public
	.byte	31                      # Abbrev [31] 0x828:0xf DW_TAG_member
	.long	.Linfo_string44         # DW_AT_name
	.long	428                     # DW_AT_type
	.byte	3                       # DW_AT_decl_file
	.byte	10                      # DW_AT_decl_line
	.byte	2                       # DW_AT_data_member_location
	.byte	35
	.byte	8
	.byte	1                       # DW_AT_accessibility
                                        # DW_ACCESS_public
	.byte	0                       # End Of Children Mark
	.byte	3                       # Abbrev [3] 0x838:0x5 DW_TAG_pointer_type
	.long	45                      # DW_AT_type
	.byte	32                      # Abbrev [32] 0x83d:0x15 DW_TAG_subroutine_type
	.long	918                     # DW_AT_type
                                        # DW_AT_prototyped
	.byte	33                      # Abbrev [33] 0x842:0x5 DW_TAG_formal_parameter
	.long	918                     # DW_AT_type
	.byte	33                      # Abbrev [33] 0x847:0x5 DW_TAG_formal_parameter
	.long	2104                    # DW_AT_type
	.byte	33                      # Abbrev [33] 0x84c:0x5 DW_TAG_formal_parameter
	.long	2104                    # DW_AT_type
	.byte	0                       # End Of Children Mark
	.byte	3                       # Abbrev [3] 0x852:0x5 DW_TAG_pointer_type
	.long	2109                    # DW_AT_type
	.byte	34                      # Abbrev [34] 0x857:0x1 DW_TAG_subroutine_type
                                        # DW_AT_prototyped
	.byte	3                       # Abbrev [3] 0x858:0x5 DW_TAG_pointer_type
	.long	2135                    # DW_AT_type
	.byte	0                       # End Of Children Mark
.L.debug_info_end0:
	.section	.debug_abbrev,"",@progbits
.L.debug_abbrev_begin:
	.byte	1                       # Abbreviation Code
	.byte	17                      # DW_TAG_compile_unit
	.byte	1                       # DW_CHILDREN_yes
	.byte	37                      # DW_AT_producer
	.byte	14                      # DW_FORM_strp
	.byte	19                      # DW_AT_language
	.byte	5                       # DW_FORM_data2
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	17                      # DW_AT_low_pc
	.byte	1                       # DW_FORM_addr
	.byte	16                      # DW_AT_stmt_list
	.byte	6                       # DW_FORM_data4
	.byte	27                      # DW_AT_comp_dir
	.byte	14                      # DW_FORM_strp
	.ascii	 "\341\177"             # DW_AT_APPLE_optimized
	.byte	25                      # DW_FORM_flag_present
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	2                       # Abbreviation Code
	.byte	36                      # DW_TAG_base_type
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	62                      # DW_AT_encoding
	.byte	11                      # DW_FORM_data1
	.byte	11                      # DW_AT_byte_size
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	3                       # Abbreviation Code
	.byte	15                      # DW_TAG_pointer_type
	.byte	0                       # DW_CHILDREN_no
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	4                       # Abbreviation Code
	.byte	52                      # DW_TAG_variable
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	63                      # DW_AT_external
	.byte	25                      # DW_FORM_flag_present
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	2                       # DW_AT_location
	.byte	10                      # DW_FORM_block1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	5                       # Abbreviation Code
	.byte	46                      # DW_TAG_subprogram
	.byte	1                       # DW_CHILDREN_yes
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	39                      # DW_AT_prototyped
	.byte	25                      # DW_FORM_flag_present
	.byte	63                      # DW_AT_external
	.byte	25                      # DW_FORM_flag_present
	.byte	17                      # DW_AT_low_pc
	.byte	1                       # DW_FORM_addr
	.byte	18                      # DW_AT_high_pc
	.byte	1                       # DW_FORM_addr
	.byte	64                      # DW_AT_frame_base
	.byte	10                      # DW_FORM_block1
	.ascii	 "\347\177"             # DW_AT_APPLE_omit_frame_ptr
	.byte	25                      # DW_FORM_flag_present
	.byte	32                      # DW_AT_inline
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	6                       # Abbreviation Code
	.byte	5                       # DW_TAG_formal_parameter
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	2                       # DW_AT_location
	.byte	6                       # DW_FORM_data4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	7                       # Abbreviation Code
	.byte	11                      # DW_TAG_lexical_block
	.byte	1                       # DW_CHILDREN_yes
	.byte	17                      # DW_AT_low_pc
	.byte	1                       # DW_FORM_addr
	.byte	18                      # DW_AT_high_pc
	.byte	1                       # DW_FORM_addr
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	8                       # Abbreviation Code
	.byte	29                      # DW_TAG_inlined_subroutine
	.byte	1                       # DW_CHILDREN_yes
	.byte	49                      # DW_AT_abstract_origin
	.byte	19                      # DW_FORM_ref4
	.byte	17                      # DW_AT_low_pc
	.byte	1                       # DW_FORM_addr
	.byte	18                      # DW_AT_high_pc
	.byte	1                       # DW_FORM_addr
	.byte	88                      # DW_AT_call_file
	.byte	11                      # DW_FORM_data1
	.byte	89                      # DW_AT_call_line
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	9                       # Abbreviation Code
	.byte	5                       # DW_TAG_formal_parameter
	.byte	0                       # DW_CHILDREN_no
	.byte	49                      # DW_AT_abstract_origin
	.byte	19                      # DW_FORM_ref4
	.byte	28                      # DW_AT_const_value
	.byte	10                      # DW_FORM_block1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	10                      # Abbreviation Code
	.byte	5                       # DW_TAG_formal_parameter
	.byte	0                       # DW_CHILDREN_no
	.byte	49                      # DW_AT_abstract_origin
	.byte	19                      # DW_FORM_ref4
	.byte	2                       # DW_AT_location
	.byte	6                       # DW_FORM_data4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	11                      # Abbreviation Code
	.byte	46                      # DW_TAG_subprogram
	.byte	1                       # DW_CHILDREN_yes
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	39                      # DW_AT_prototyped
	.byte	25                      # DW_FORM_flag_present
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	63                      # DW_AT_external
	.byte	25                      # DW_FORM_flag_present
	.byte	17                      # DW_AT_low_pc
	.byte	1                       # DW_FORM_addr
	.byte	18                      # DW_AT_high_pc
	.byte	1                       # DW_FORM_addr
	.byte	64                      # DW_AT_frame_base
	.byte	10                      # DW_FORM_block1
	.ascii	 "\347\177"             # DW_AT_APPLE_omit_frame_ptr
	.byte	25                      # DW_FORM_flag_present
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	12                      # Abbreviation Code
	.byte	52                      # DW_TAG_variable
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	2                       # DW_AT_location
	.byte	6                       # DW_FORM_data4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	13                      # Abbreviation Code
	.byte	22                      # DW_TAG_typedef
	.byte	0                       # DW_CHILDREN_no
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	14                      # Abbreviation Code
	.byte	52                      # DW_TAG_variable
	.byte	0                       # DW_CHILDREN_no
	.byte	49                      # DW_AT_abstract_origin
	.byte	19                      # DW_FORM_ref4
	.byte	2                       # DW_AT_location
	.byte	6                       # DW_FORM_data4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	15                      # Abbreviation Code
	.byte	52                      # DW_TAG_variable
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	2                       # DW_AT_location
	.byte	10                      # DW_FORM_block1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	16                      # Abbreviation Code
	.byte	5                       # DW_TAG_formal_parameter
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	5                       # DW_FORM_data2
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	28                      # DW_AT_const_value
	.byte	10                      # DW_FORM_block1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	17                      # Abbreviation Code
	.byte	52                      # DW_TAG_variable
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	5                       # DW_FORM_data2
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	2                       # DW_AT_location
	.byte	6                       # DW_FORM_data4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	18                      # Abbreviation Code
	.byte	5                       # DW_TAG_formal_parameter
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	5                       # DW_FORM_data2
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	2                       # DW_AT_location
	.byte	6                       # DW_FORM_data4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	19                      # Abbreviation Code
	.byte	15                      # DW_TAG_pointer_type
	.byte	0                       # DW_CHILDREN_no
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	20                      # Abbreviation Code
	.byte	46                      # DW_TAG_subprogram
	.byte	1                       # DW_CHILDREN_yes
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	39                      # DW_AT_prototyped
	.byte	25                      # DW_FORM_flag_present
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	63                      # DW_AT_external
	.byte	25                      # DW_FORM_flag_present
	.byte	17                      # DW_AT_low_pc
	.byte	1                       # DW_FORM_addr
	.byte	18                      # DW_AT_high_pc
	.byte	1                       # DW_FORM_addr
	.byte	64                      # DW_AT_frame_base
	.byte	10                      # DW_FORM_block1
	.ascii	 "\347\177"             # DW_AT_APPLE_omit_frame_ptr
	.byte	25                      # DW_FORM_flag_present
	.byte	32                      # DW_AT_inline
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	21                      # Abbreviation Code
	.byte	29                      # DW_TAG_inlined_subroutine
	.byte	1                       # DW_CHILDREN_yes
	.byte	49                      # DW_AT_abstract_origin
	.byte	19                      # DW_FORM_ref4
	.byte	85                      # DW_AT_ranges
	.byte	6                       # DW_FORM_data4
	.byte	88                      # DW_AT_call_file
	.byte	11                      # DW_FORM_data1
	.byte	89                      # DW_AT_call_line
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	22                      # Abbreviation Code
	.byte	46                      # DW_TAG_subprogram
	.byte	1                       # DW_CHILDREN_yes
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	5                       # DW_FORM_data2
	.byte	39                      # DW_AT_prototyped
	.byte	25                      # DW_FORM_flag_present
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	32                      # DW_AT_inline
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	23                      # Abbreviation Code
	.byte	5                       # DW_TAG_formal_parameter
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	5                       # DW_FORM_data2
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	24                      # Abbreviation Code
	.byte	11                      # DW_TAG_lexical_block
	.byte	1                       # DW_CHILDREN_yes
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	25                      # Abbreviation Code
	.byte	52                      # DW_TAG_variable
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	5                       # DW_FORM_data2
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	26                      # Abbreviation Code
	.byte	38                      # DW_TAG_const_type
	.byte	0                       # DW_CHILDREN_no
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	27                      # Abbreviation Code
	.byte	36                      # DW_TAG_base_type
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	11                      # DW_AT_byte_size
	.byte	11                      # DW_FORM_data1
	.byte	62                      # DW_AT_encoding
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	28                      # Abbreviation Code
	.byte	1                       # DW_TAG_array_type
	.byte	1                       # DW_CHILDREN_yes
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	29                      # Abbreviation Code
	.byte	33                      # DW_TAG_subrange_type
	.byte	0                       # DW_CHILDREN_no
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	47                      # DW_AT_upper_bound
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	30                      # Abbreviation Code
	.byte	19                      # DW_TAG_structure_type
	.byte	1                       # DW_CHILDREN_yes
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	11                      # DW_AT_byte_size
	.byte	11                      # DW_FORM_data1
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	31                      # Abbreviation Code
	.byte	13                      # DW_TAG_member
	.byte	0                       # DW_CHILDREN_no
	.byte	3                       # DW_AT_name
	.byte	14                      # DW_FORM_strp
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	58                      # DW_AT_decl_file
	.byte	11                      # DW_FORM_data1
	.byte	59                      # DW_AT_decl_line
	.byte	11                      # DW_FORM_data1
	.byte	56                      # DW_AT_data_member_location
	.byte	10                      # DW_FORM_block1
	.byte	50                      # DW_AT_accessibility
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	32                      # Abbreviation Code
	.byte	21                      # DW_TAG_subroutine_type
	.byte	1                       # DW_CHILDREN_yes
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	39                      # DW_AT_prototyped
	.byte	25                      # DW_FORM_flag_present
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	33                      # Abbreviation Code
	.byte	5                       # DW_TAG_formal_parameter
	.byte	0                       # DW_CHILDREN_no
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	34                      # Abbreviation Code
	.byte	21                      # DW_TAG_subroutine_type
	.byte	0                       # DW_CHILDREN_no
	.byte	39                      # DW_AT_prototyped
	.byte	25                      # DW_FORM_flag_present
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	0                       # EOM(3)
.L.debug_abbrev_end:
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
	.quad	.Lfunc_begin0
	.quad	.Ltmp2
.Lset0 = .Ltmp180-.Ltmp179              # Loc expr size
	.short	.Lset0
.Ltmp179:
	.byte	85                      # DW_OP_reg5
.Ltmp180:
	.quad	0
	.quad	0
.Ldebug_loc2:
	.quad	.Lfunc_begin0
	.quad	.Ltmp2
.Lset1 = .Ltmp182-.Ltmp181              # Loc expr size
	.short	.Lset1
.Ltmp181:
	.byte	85                      # DW_OP_reg5
.Ltmp182:
	.quad	0
	.quad	0
.Ldebug_loc4:
	.quad	.Lfunc_begin1
	.quad	.Ltmp6
.Lset2 = .Ltmp184-.Ltmp183              # Loc expr size
	.short	.Lset2
.Ltmp183:
	.byte	85                      # DW_OP_reg5
.Ltmp184:
	.quad	0
	.quad	0
.Ldebug_loc6:
	.quad	.Lfunc_begin1
	.quad	.Ltmp4
.Lset3 = .Ltmp186-.Ltmp185              # Loc expr size
	.short	.Lset3
.Ltmp185:
	.byte	85                      # DW_OP_reg5
.Ltmp186:
	.quad	0
	.quad	0
.Ldebug_loc8:
	.quad	.Ltmp4
	.quad	.Ltmp6
.Lset4 = .Ltmp188-.Ltmp187              # Loc expr size
	.short	.Lset4
.Ltmp187:
	.byte	82                      # DW_OP_reg2
.Ltmp188:
	.quad	.Ltmp9
	.quad	.Ltmp11
.Lset5 = .Ltmp190-.Ltmp189              # Loc expr size
	.short	.Lset5
.Ltmp189:
	.byte	82                      # DW_OP_reg2
.Ltmp190:
	.quad	.Ltmp22
	.quad	.Ltmp24
.Lset6 = .Ltmp192-.Ltmp191              # Loc expr size
	.short	.Lset6
.Ltmp191:
	.byte	82                      # DW_OP_reg2
.Ltmp192:
	.quad	0
	.quad	0
.Ldebug_loc12:
	.quad	.Ltmp5
	.quad	.Ltmp6
.Lset7 = .Ltmp194-.Ltmp193              # Loc expr size
	.short	.Lset7
.Ltmp193:
	.byte	80                      # DW_OP_reg0
.Ltmp194:
	.quad	.Ltmp9
	.quad	.Ltmp11
.Lset8 = .Ltmp196-.Ltmp195              # Loc expr size
	.short	.Lset8
.Ltmp195:
	.byte	80                      # DW_OP_reg0
.Ltmp196:
	.quad	.Ltmp23
	.quad	.Ltmp24
.Lset9 = .Ltmp198-.Ltmp197              # Loc expr size
	.short	.Lset9
.Ltmp197:
	.byte	80                      # DW_OP_reg0
.Ltmp198:
	.quad	0
	.quad	0
.Ldebug_loc16:
	.quad	.Lfunc_begin2
	.quad	.Ltmp32
.Lset10 = .Ltmp200-.Ltmp199             # Loc expr size
	.short	.Lset10
.Ltmp199:
	.byte	85                      # DW_OP_reg5
.Ltmp200:
	.quad	.Ltmp32
	.quad	.Ltmp36
.Lset11 = .Ltmp202-.Ltmp201             # Loc expr size
	.short	.Lset11
.Ltmp201:
	.byte	83                      # DW_OP_reg3
.Ltmp202:
	.quad	0
	.quad	0
.Ldebug_loc19:
	.quad	.Ltmp35
	.quad	.Ltmp37
.Lset12 = .Ltmp204-.Ltmp203             # Loc expr size
	.short	.Lset12
.Ltmp203:
	.byte	80                      # DW_OP_reg0
.Ltmp204:
	.quad	0
	.quad	0
.Ldebug_loc21:
	.quad	.Lfunc_begin3
	.quad	.Ltmp40
.Lset13 = .Ltmp206-.Ltmp205             # Loc expr size
	.short	.Lset13
.Ltmp205:
	.byte	85                      # DW_OP_reg5
.Ltmp206:
	.quad	0
	.quad	0
.Ldebug_loc23:
	.quad	.Ltmp42
	.quad	.Ltmp43
.Lset14 = .Ltmp208-.Ltmp207             # Loc expr size
	.short	.Lset14
.Ltmp207:
	.byte	80                      # DW_OP_reg0
.Ltmp208:
	.quad	0
	.quad	0
.Ldebug_loc25:
	.quad	.Lfunc_begin4
	.quad	.Ltmp45
.Lset15 = .Ltmp210-.Ltmp209             # Loc expr size
	.short	.Lset15
.Ltmp209:
	.byte	85                      # DW_OP_reg5
.Ltmp210:
	.quad	.Ltmp45
	.quad	.Ltmp46
.Lset16 = .Ltmp212-.Ltmp211             # Loc expr size
	.short	.Lset16
.Ltmp211:
	.byte	81                      # DW_OP_reg1
.Ltmp212:
	.quad	.Ltmp47
	.quad	.Ltmp51
.Lset17 = .Ltmp214-.Ltmp213             # Loc expr size
	.short	.Lset17
.Ltmp213:
	.byte	81                      # DW_OP_reg1
.Ltmp214:
	.quad	0
	.quad	0
.Ldebug_loc29:
	.quad	.Ltmp48
	.quad	.Ltmp51
.Lset18 = .Ltmp216-.Ltmp215             # Loc expr size
	.short	.Lset18
.Ltmp215:
	.byte	85                      # DW_OP_reg5
.Ltmp216:
	.quad	0
	.quad	0
.Ldebug_loc31:
	.quad	.Lfunc_begin5
	.quad	.Ltmp55
.Lset19 = .Ltmp218-.Ltmp217             # Loc expr size
	.short	.Lset19
.Ltmp217:
	.byte	85                      # DW_OP_reg5
.Ltmp218:
	.quad	0
	.quad	0
.Ldebug_loc34:
	.quad	.Lfunc_begin6
	.quad	.Ltmp58
.Lset20 = .Ltmp220-.Ltmp219             # Loc expr size
	.short	.Lset20
.Ltmp219:
	.byte	85                      # DW_OP_reg5
.Ltmp220:
	.quad	0
	.quad	0
.Ldebug_loc36:
	.quad	.Lfunc_begin7
	.quad	.Ltmp63
.Lset21 = .Ltmp222-.Ltmp221             # Loc expr size
	.short	.Lset21
.Ltmp221:
	.byte	85                      # DW_OP_reg5
.Ltmp222:
	.quad	0
	.quad	0
.Ldebug_loc38:
	.quad	.Lfunc_begin7
	.quad	.Ltmp62
.Lset22 = .Ltmp224-.Ltmp223             # Loc expr size
	.short	.Lset22
.Ltmp223:
	.byte	85                      # DW_OP_reg5
.Ltmp224:
	.quad	0
	.quad	0
.Ldebug_loc40:
	.quad	.Ltmp61
	.quad	.Ltmp63
.Lset23 = .Ltmp226-.Ltmp225             # Loc expr size
	.short	.Lset23
.Ltmp225:
	.byte	80                      # DW_OP_reg0
.Ltmp226:
	.quad	.Ltmp65
	.quad	.Ltmp66
.Lset24 = .Ltmp228-.Ltmp227             # Loc expr size
	.short	.Lset24
.Ltmp227:
	.byte	80                      # DW_OP_reg0
.Ltmp228:
	.quad	.Ltmp67
	.quad	.Ltmp68
.Lset25 = .Ltmp230-.Ltmp229             # Loc expr size
	.short	.Lset25
.Ltmp229:
	.byte	80                      # DW_OP_reg0
.Ltmp230:
	.quad	.Ltmp69
	.quad	.Ltmp70
.Lset26 = .Ltmp232-.Ltmp231             # Loc expr size
	.short	.Lset26
.Ltmp231:
	.byte	80                      # DW_OP_reg0
.Ltmp232:
	.quad	.Ltmp71
	.quad	.Ltmp72
.Lset27 = .Ltmp234-.Ltmp233             # Loc expr size
	.short	.Lset27
.Ltmp233:
	.byte	80                      # DW_OP_reg0
.Ltmp234:
	.quad	.Ltmp73
	.quad	.Ltmp74
.Lset28 = .Ltmp236-.Ltmp235             # Loc expr size
	.short	.Lset28
.Ltmp235:
	.byte	80                      # DW_OP_reg0
.Ltmp236:
	.quad	.Ltmp75
	.quad	.Ltmp76
.Lset29 = .Ltmp238-.Ltmp237             # Loc expr size
	.short	.Lset29
.Ltmp237:
	.byte	80                      # DW_OP_reg0
.Ltmp238:
	.quad	.Ltmp77
	.quad	.Ltmp77
.Lset30 = .Ltmp240-.Ltmp239             # Loc expr size
	.short	.Lset30
.Ltmp239:
	.byte	80                      # DW_OP_reg0
.Ltmp240:
	.quad	0
	.quad	0
.Ldebug_loc49:
	.quad	.Ltmp61
	.quad	.Ltmp63
.Lset31 = .Ltmp242-.Ltmp241             # Loc expr size
	.short	.Lset31
.Ltmp241:
	.byte	80                      # DW_OP_reg0
.Ltmp242:
	.quad	.Ltmp77
	.quad	.Ltmp78
.Lset32 = .Ltmp244-.Ltmp243             # Loc expr size
	.short	.Lset32
.Ltmp243:
	.byte	80                      # DW_OP_reg0
.Ltmp244:
	.quad	0
	.quad	0
.Ldebug_loc52:
	.quad	.Ltmp97
	.quad	.Ltmp117
.Lset33 = .Ltmp246-.Ltmp245             # Loc expr size
	.short	.Lset33
.Ltmp245:
	.byte	85                      # DW_OP_reg5
.Ltmp246:
	.quad	.Ltmp117
	.quad	.Ltmp117
.Lset34 = .Ltmp248-.Ltmp247             # Loc expr size
	.short	.Lset34
.Ltmp247:
	.byte	80                      # DW_OP_reg0
.Ltmp248:
	.quad	.Ltmp118
	.quad	.Ltmp122
.Lset35 = .Ltmp250-.Ltmp249             # Loc expr size
	.short	.Lset35
.Ltmp249:
	.byte	80                      # DW_OP_reg0
.Ltmp250:
	.quad	0
	.quad	0
.Ldebug_loc56:
	.quad	.Lfunc_begin8
	.quad	.Ltmp121
.Lset36 = .Ltmp252-.Ltmp251             # Loc expr size
	.short	.Lset36
.Ltmp251:
	.byte	16                      # DW_OP_constu
	.byte	0
.Ltmp252:
	.quad	.Ltmp121
	.quad	.Ltmp122
.Lset37 = .Ltmp254-.Ltmp253             # Loc expr size
	.short	.Lset37
.Ltmp253:
	.byte	81                      # DW_OP_reg1
.Ltmp254:
	.quad	0
	.quad	0
.Ldebug_loc59:
	.quad	.Lfunc_begin8
	.quad	.Lfunc_end8
.Lset38 = .Ltmp256-.Ltmp255             # Loc expr size
	.short	.Lset38
.Ltmp255:
	.byte	16                      # DW_OP_constu
	.byte	1
.Ltmp256:
	.quad	0
	.quad	0
.Ldebug_loc62:
	.quad	.Lfunc_begin8
	.quad	.Ltmp83
.Lset39 = .Ltmp258-.Ltmp257             # Loc expr size
	.short	.Lset39
.Ltmp257:
	.byte	85                      # DW_OP_reg5
.Ltmp258:
	.quad	.Ltmp96
	.quad	.Ltmp100
.Lset40 = .Ltmp260-.Ltmp259             # Loc expr size
	.short	.Lset40
.Ltmp259:
	.byte	85                      # DW_OP_reg5
.Ltmp260:
	.quad	0
	.quad	0
.Ldebug_loc65:
	.quad	.Lfunc_begin8
	.quad	.Ltmp83
.Lset41 = .Ltmp262-.Ltmp261             # Loc expr size
	.short	.Lset41
.Ltmp261:
	.byte	85                      # DW_OP_reg5
.Ltmp262:
	.quad	0
	.quad	0
.Ldebug_loc67:
	.quad	.Lfunc_begin8
	.quad	.Ltmp83
.Lset42 = .Ltmp264-.Ltmp263             # Loc expr size
	.short	.Lset42
.Ltmp263:
	.byte	85                      # DW_OP_reg5
.Ltmp264:
	.quad	.Ltmp96
	.quad	.Ltmp97
.Lset43 = .Ltmp266-.Ltmp265             # Loc expr size
	.short	.Lset43
.Ltmp265:
	.byte	85                      # DW_OP_reg5
.Ltmp266:
	.quad	0
	.quad	0
.Ldebug_loc70:
	.quad	.Ltmp81
	.quad	.Ltmp83
.Lset44 = .Ltmp268-.Ltmp267             # Loc expr size
	.short	.Lset44
.Ltmp267:
	.byte	81                      # DW_OP_reg1
.Ltmp268:
	.quad	.Ltmp84
	.quad	.Ltmp85
.Lset45 = .Ltmp270-.Ltmp269             # Loc expr size
	.short	.Lset45
.Ltmp269:
	.byte	82                      # DW_OP_reg2
.Ltmp270:
	.quad	.Ltmp86
	.quad	.Ltmp87
.Lset46 = .Ltmp272-.Ltmp271             # Loc expr size
	.short	.Lset46
.Ltmp271:
	.byte	82                      # DW_OP_reg2
.Ltmp272:
	.quad	.Ltmp88
	.quad	.Ltmp89
.Lset47 = .Ltmp274-.Ltmp273             # Loc expr size
	.short	.Lset47
.Ltmp273:
	.byte	82                      # DW_OP_reg2
.Ltmp274:
	.quad	.Ltmp90
	.quad	.Ltmp91
.Lset48 = .Ltmp276-.Ltmp275             # Loc expr size
	.short	.Lset48
.Ltmp275:
	.byte	82                      # DW_OP_reg2
.Ltmp276:
	.quad	.Ltmp92
	.quad	.Ltmp93
.Lset49 = .Ltmp278-.Ltmp277             # Loc expr size
	.short	.Lset49
.Ltmp277:
	.byte	82                      # DW_OP_reg2
.Ltmp278:
	.quad	.Ltmp94
	.quad	.Ltmp95
.Lset50 = .Ltmp280-.Ltmp279             # Loc expr size
	.short	.Lset50
.Ltmp279:
	.byte	82                      # DW_OP_reg2
.Ltmp280:
	.quad	.Ltmp96
	.quad	.Ltmp96
.Lset51 = .Ltmp282-.Ltmp281             # Loc expr size
	.short	.Lset51
.Ltmp281:
	.byte	82                      # DW_OP_reg2
.Ltmp282:
	.quad	.Ltmp96
	.quad	.Ltmp100
.Lset52 = .Ltmp284-.Ltmp283             # Loc expr size
	.short	.Lset52
.Ltmp283:
	.byte	81                      # DW_OP_reg1
.Ltmp284:
	.quad	0
	.quad	0
.Ldebug_loc80:
	.quad	.Ltmp81
	.quad	.Ltmp83
.Lset53 = .Ltmp286-.Ltmp285             # Loc expr size
	.short	.Lset53
.Ltmp285:
	.byte	81                      # DW_OP_reg1
.Ltmp286:
	.quad	.Ltmp102
	.quad	.Ltmp103
.Lset54 = .Ltmp288-.Ltmp287             # Loc expr size
	.short	.Lset54
.Ltmp287:
	.byte	81                      # DW_OP_reg1
.Ltmp288:
	.quad	.Ltmp104
	.quad	.Ltmp105
.Lset55 = .Ltmp290-.Ltmp289             # Loc expr size
	.short	.Lset55
.Ltmp289:
	.byte	81                      # DW_OP_reg1
.Ltmp290:
	.quad	.Ltmp106
	.quad	.Ltmp107
.Lset56 = .Ltmp292-.Ltmp291             # Loc expr size
	.short	.Lset56
.Ltmp291:
	.byte	81                      # DW_OP_reg1
.Ltmp292:
	.quad	.Ltmp108
	.quad	.Ltmp109
.Lset57 = .Ltmp294-.Ltmp293             # Loc expr size
	.short	.Lset57
.Ltmp293:
	.byte	81                      # DW_OP_reg1
.Ltmp294:
	.quad	.Ltmp110
	.quad	.Ltmp111
.Lset58 = .Ltmp296-.Ltmp295             # Loc expr size
	.short	.Lset58
.Ltmp295:
	.byte	81                      # DW_OP_reg1
.Ltmp296:
	.quad	.Ltmp112
	.quad	.Ltmp113
.Lset59 = .Ltmp298-.Ltmp297             # Loc expr size
	.short	.Lset59
.Ltmp297:
	.byte	81                      # DW_OP_reg1
.Ltmp298:
	.quad	.Ltmp114
	.quad	.Ltmp114
.Lset60 = .Ltmp300-.Ltmp299             # Loc expr size
	.short	.Lset60
.Ltmp299:
	.byte	81                      # DW_OP_reg1
.Ltmp300:
	.quad	0
	.quad	0
.Ldebug_loc89:
	.quad	.Ltmp81
	.quad	.Ltmp83
.Lset61 = .Ltmp302-.Ltmp301             # Loc expr size
	.short	.Lset61
.Ltmp301:
	.byte	80                      # DW_OP_reg0
.Ltmp302:
	.quad	0
	.quad	0
.Ldebug_loc91:
	.quad	.Ltmp81
	.quad	.Ltmp82
.Lset62 = .Ltmp304-.Ltmp303             # Loc expr size
	.short	.Lset62
.Ltmp303:
	.byte	80                      # DW_OP_reg0
.Ltmp304:
	.quad	.Ltmp96
	.quad	.Ltmp100
.Lset63 = .Ltmp306-.Ltmp305             # Loc expr size
	.short	.Lset63
.Ltmp305:
	.byte	80                      # DW_OP_reg0
.Ltmp306:
	.quad	0
	.quad	0
.Ldebug_loc94:
	.quad	.Ltmp98
	.quad	.Ltmp99
.Lset64 = .Ltmp308-.Ltmp307             # Loc expr size
	.short	.Lset64
.Ltmp307:
	.byte	82                      # DW_OP_reg2
.Ltmp308:
	.quad	0
	.quad	0
.Ldebug_loc96:
	.quad	.Ltmp122
	.quad	.Ltmp124
.Lset65 = .Ltmp310-.Ltmp309             # Loc expr size
	.short	.Lset65
.Ltmp309:
	.byte	80                      # DW_OP_reg0
.Ltmp310:
	.quad	.Ltmp137
	.quad	.Ltmp140
.Lset66 = .Ltmp312-.Ltmp311             # Loc expr size
	.short	.Lset66
.Ltmp311:
	.byte	80                      # DW_OP_reg0
.Ltmp312:
	.quad	0
	.quad	0
.Ldebug_loc99:
	.quad	.Ltmp122
	.quad	.Ltmp124
.Lset67 = .Ltmp314-.Ltmp313             # Loc expr size
	.short	.Lset67
.Ltmp313:
	.byte	80                      # DW_OP_reg0
.Ltmp314:
	.quad	0
	.quad	0
.Ldebug_loc101:
	.quad	.Ltmp122
	.quad	.Ltmp124
.Lset68 = .Ltmp316-.Ltmp315             # Loc expr size
	.short	.Lset68
.Ltmp315:
	.byte	80                      # DW_OP_reg0
.Ltmp316:
	.quad	.Ltmp137
	.quad	.Ltmp140
.Lset69 = .Ltmp318-.Ltmp317             # Loc expr size
	.short	.Lset69
.Ltmp317:
	.byte	80                      # DW_OP_reg0
.Ltmp318:
	.quad	0
	.quad	0
.Ldebug_loc104:
	.quad	.Ltmp123
	.quad	.Ltmp124
.Lset70 = .Ltmp320-.Ltmp319             # Loc expr size
	.short	.Lset70
.Ltmp319:
	.byte	82                      # DW_OP_reg2
.Ltmp320:
	.quad	.Ltmp125
	.quad	.Ltmp126
.Lset71 = .Ltmp322-.Ltmp321             # Loc expr size
	.short	.Lset71
.Ltmp321:
	.byte	84                      # DW_OP_reg4
.Ltmp322:
	.quad	.Ltmp127
	.quad	.Ltmp128
.Lset72 = .Ltmp324-.Ltmp323             # Loc expr size
	.short	.Lset72
.Ltmp323:
	.byte	84                      # DW_OP_reg4
.Ltmp324:
	.quad	.Ltmp129
	.quad	.Ltmp130
.Lset73 = .Ltmp326-.Ltmp325             # Loc expr size
	.short	.Lset73
.Ltmp325:
	.byte	84                      # DW_OP_reg4
.Ltmp326:
	.quad	.Ltmp131
	.quad	.Ltmp132
.Lset74 = .Ltmp328-.Ltmp327             # Loc expr size
	.short	.Lset74
.Ltmp327:
	.byte	84                      # DW_OP_reg4
.Ltmp328:
	.quad	.Ltmp133
	.quad	.Ltmp134
.Lset75 = .Ltmp330-.Ltmp329             # Loc expr size
	.short	.Lset75
.Ltmp329:
	.byte	84                      # DW_OP_reg4
.Ltmp330:
	.quad	.Ltmp135
	.quad	.Ltmp136
.Lset76 = .Ltmp332-.Ltmp331             # Loc expr size
	.short	.Lset76
.Ltmp331:
	.byte	84                      # DW_OP_reg4
.Ltmp332:
	.quad	.Ltmp137
	.quad	.Ltmp137
.Lset77 = .Ltmp334-.Ltmp333             # Loc expr size
	.short	.Lset77
.Ltmp333:
	.byte	84                      # DW_OP_reg4
.Ltmp334:
	.quad	.Ltmp137
	.quad	.Ltmp140
.Lset78 = .Ltmp336-.Ltmp335             # Loc expr size
	.short	.Lset78
.Ltmp335:
	.byte	82                      # DW_OP_reg2
.Ltmp336:
	.quad	0
	.quad	0
.Ldebug_loc114:
	.quad	.Ltmp123
	.quad	.Ltmp124
.Lset79 = .Ltmp338-.Ltmp337             # Loc expr size
	.short	.Lset79
.Ltmp337:
	.byte	82                      # DW_OP_reg2
.Ltmp338:
	.quad	.Ltmp141
	.quad	.Ltmp142
.Lset80 = .Ltmp340-.Ltmp339             # Loc expr size
	.short	.Lset80
.Ltmp339:
	.byte	82                      # DW_OP_reg2
.Ltmp340:
	.quad	.Ltmp143
	.quad	.Ltmp144
.Lset81 = .Ltmp342-.Ltmp341             # Loc expr size
	.short	.Lset81
.Ltmp341:
	.byte	82                      # DW_OP_reg2
.Ltmp342:
	.quad	.Ltmp145
	.quad	.Ltmp146
.Lset82 = .Ltmp344-.Ltmp343             # Loc expr size
	.short	.Lset82
.Ltmp343:
	.byte	82                      # DW_OP_reg2
.Ltmp344:
	.quad	.Ltmp147
	.quad	.Ltmp148
.Lset83 = .Ltmp346-.Ltmp345             # Loc expr size
	.short	.Lset83
.Ltmp345:
	.byte	82                      # DW_OP_reg2
.Ltmp346:
	.quad	.Ltmp149
	.quad	.Ltmp150
.Lset84 = .Ltmp348-.Ltmp347             # Loc expr size
	.short	.Lset84
.Ltmp347:
	.byte	82                      # DW_OP_reg2
.Ltmp348:
	.quad	.Ltmp151
	.quad	.Ltmp152
.Lset85 = .Ltmp350-.Ltmp349             # Loc expr size
	.short	.Lset85
.Ltmp349:
	.byte	82                      # DW_OP_reg2
.Ltmp350:
	.quad	.Ltmp153
	.quad	.Ltmp154
.Lset86 = .Ltmp352-.Ltmp351             # Loc expr size
	.short	.Lset86
.Ltmp351:
	.byte	82                      # DW_OP_reg2
.Ltmp352:
	.quad	0
	.quad	0
.Ldebug_loc123:
	.quad	.Ltmp123
	.quad	.Ltmp124
.Lset87 = .Ltmp354-.Ltmp353             # Loc expr size
	.short	.Lset87
.Ltmp353:
	.byte	82                      # DW_OP_reg2
.Ltmp354:
	.quad	0
	.quad	0
.Ldebug_loc125:
	.quad	.Ltmp123
	.quad	.Ltmp124
.Lset88 = .Ltmp356-.Ltmp355             # Loc expr size
	.short	.Lset88
.Ltmp355:
	.byte	82                      # DW_OP_reg2
.Ltmp356:
	.quad	.Ltmp137
	.quad	.Ltmp140
.Lset89 = .Ltmp358-.Ltmp357             # Loc expr size
	.short	.Lset89
.Ltmp357:
	.byte	82                      # DW_OP_reg2
.Ltmp358:
	.quad	0
	.quad	0
.Ldebug_loc128:
	.quad	.Ltmp138
	.quad	.Ltmp139
.Lset90 = .Ltmp360-.Ltmp359             # Loc expr size
	.short	.Lset90
.Ltmp359:
	.byte	84                      # DW_OP_reg4
.Ltmp360:
	.quad	0
	.quad	0
.Ldebug_loc130:
	.quad	.Lfunc_begin9
	.quad	.Ltmp157
.Lset91 = .Ltmp362-.Ltmp361             # Loc expr size
	.short	.Lset91
.Ltmp361:
	.byte	85                      # DW_OP_reg5
.Ltmp362:
	.quad	.Ltmp157
	.quad	.Ltmp161
.Lset92 = .Ltmp364-.Ltmp363             # Loc expr size
	.short	.Lset92
.Ltmp363:
	.byte	81                      # DW_OP_reg1
.Ltmp364:
	.quad	0
	.quad	0
.Ldebug_loc134:
	.quad	.Lfunc_begin10
	.quad	.Ltmp166
.Lset93 = .Ltmp366-.Ltmp365             # Loc expr size
	.short	.Lset93
.Ltmp365:
	.byte	85                      # DW_OP_reg5
.Ltmp366:
	.quad	.Ltmp166
	.quad	.Ltmp177
.Lset94 = .Ltmp368-.Ltmp367             # Loc expr size
	.short	.Lset94
.Ltmp367:
	.byte	88                      # DW_OP_reg8
.Ltmp368:
	.quad	0
	.quad	0
.Ldebug_loc137:
	.quad	.Lfunc_begin10
	.quad	.Ltmp170
.Lset95 = .Ltmp370-.Ltmp369             # Loc expr size
	.short	.Lset95
.Ltmp369:
	.byte	84                      # DW_OP_reg4
.Ltmp370:
	.quad	0
	.quad	0
.Ldebug_loc139:
	.quad	.Lfunc_begin10
	.quad	.Ltmp171
.Lset96 = .Ltmp372-.Ltmp371             # Loc expr size
	.short	.Lset96
.Ltmp371:
	.byte	81                      # DW_OP_reg1
.Ltmp372:
	.quad	0
	.quad	0
.Ldebug_loc141:
	.quad	.Lfunc_begin10
	.quad	.Ltmp168
.Lset97 = .Ltmp374-.Ltmp373             # Loc expr size
	.short	.Lset97
.Ltmp373:
	.byte	82                      # DW_OP_reg2
.Ltmp374:
	.quad	0
	.quad	0
.Ldebug_loc143:
	.quad	.Lfunc_begin10
	.quad	.Ltmp166
.Lset98 = .Ltmp376-.Ltmp375             # Loc expr size
	.short	.Lset98
.Ltmp375:
	.byte	88                      # DW_OP_reg8
.Ltmp376:
	.quad	0
	.quad	0
.Ldebug_loc145:
	.quad	.Lfunc_begin10
	.quad	.Ltmp177
.Lset99 = .Ltmp378-.Ltmp377             # Loc expr size
	.short	.Lset99
.Ltmp377:
	.byte	89                      # DW_OP_reg9
.Ltmp378:
	.quad	0
	.quad	0
.Ldebug_loc147:
	.quad	.Ltmp168
	.quad	.Ltmp169
.Lset100 = .Ltmp380-.Ltmp379            # Loc expr size
	.short	.Lset100
.Ltmp379:
	.byte	80                      # DW_OP_reg0
.Ltmp380:
	.quad	0
	.quad	0
.Ldebug_loc149:
	.quad	.Ltmp169
	.quad	.Ltmp172
.Lset101 = .Ltmp382-.Ltmp381            # Loc expr size
	.short	.Lset101
.Ltmp381:
	.byte	80                      # DW_OP_reg0
.Ltmp382:
	.quad	0
	.quad	0
.Ldebug_loc151:
	.quad	.Ltmp173
	.quad	.Ltmp177
.Lset102 = .Ltmp384-.Ltmp383            # Loc expr size
	.short	.Lset102
.Ltmp383:
	.byte	82                      # DW_OP_reg2
.Ltmp384:
	.quad	0
	.quad	0
.Ldebug_loc153:
	.quad	.Ltmp173
	.quad	.Ltmp175
.Lset103 = .Ltmp386-.Ltmp385            # Loc expr size
	.short	.Lset103
.Ltmp385:
	.byte	82                      # DW_OP_reg2
.Ltmp386:
	.quad	0
	.quad	0
.Ldebug_loc155:
	.section	.debug_aranges,"",@progbits
	.section	.debug_ranges,"",@progbits
	.quad	.Lfunc_begin8
	.quad	.Ltmp96
	.quad	.Ltmp97
	.quad	.Ltmp100
	.quad	0
	.quad	0
	.quad	.Ltmp119
	.quad	.Ltmp120
	.quad	.Ltmp140
	.quad	.Ltmp154
	.quad	0
	.quad	0
	.section	.debug_macinfo,"",@progbits
	.section	.debug_str,"MS",@progbits,1
.Linfo_string0:
	.asciz	 "clang version 3.3 (trunk 171645)"
.Linfo_string1:
	.asciz	 "libc.c"
.Linfo_string2:
	.asciz	 "/home/theusurper/llvm/testcase/prog2"
.Linfo_string3:
	.asciz	 "current_brk"
.Linfo_string4:
	.asciz	 "char"
.Linfo_string5:
	.asciz	 "llexit"
.Linfo_string6:
	.asciz	 "llstrlen"
.Linfo_string7:
	.asciz	 "long long unsigned int"
.Linfo_string8:
	.asciz	 "size_t"
.Linfo_string9:
	.asciz	 "puts"
.Linfo_string10:
	.asciz	 "long long int"
.Linfo_string11:
	.asciz	 "putchar"
.Linfo_string12:
	.asciz	 "malloc"
.Linfo_string13:
	.asciz	 "isspace"
.Linfo_string14:
	.asciz	 "isdigit"
.Linfo_string15:
	.asciz	 "LLreadchar"
.Linfo_string16:
	.asciz	 "INT"
.Linfo_string17:
	.asciz	 "atol"
.Linfo_string18:
	.asciz	 "long int"
.Linfo_string19:
	.asciz	 "time"
.Linfo_string20:
	.asciz	 "time_t"
.Linfo_string21:
	.asciz	 "__libc_start_main"
.Linfo_string22:
	.asciz	 "__syscall2"
.Linfo_string23:
	.asciz	 "__syscall3"
.Linfo_string24:
	.asciz	 "__syscall1"
.Linfo_string25:
	.asciz	 "n"
.Linfo_string26:
	.asciz	 "a1"
.Linfo_string27:
	.asciz	 "ret"
.Linfo_string28:
	.asciz	 "s"
.Linfo_string29:
	.asciz	 "v"
.Linfo_string30:
	.asciz	 "p"
.Linfo_string31:
	.asciz	 "l"
.Linfo_string32:
	.asciz	 "a2"
.Linfo_string33:
	.asciz	 "a3"
.Linfo_string34:
	.asciz	 "c"
.Linfo_string35:
	.asciz	 "int"
.Linfo_string36:
	.asciz	 "size"
.Linfo_string37:
	.asciz	 "long unsigned int"
.Linfo_string38:
	.asciz	 "r"
.Linfo_string39:
	.asciz	 "neg"
.Linfo_string40:
	.asciz	 "t"
.Linfo_string41:
	.asciz	 "ts"
.Linfo_string42:
	.asciz	 "tv_sec"
.Linfo_string43:
	.asciz	 "__time_t"
.Linfo_string44:
	.asciz	 "tv_nsec"
.Linfo_string45:
	.asciz	 "timespec"
.Linfo_string46:
	.asciz	 "main"
.Linfo_string47:
	.asciz	 "argc"
.Linfo_string48:
	.asciz	 "argv"
.Linfo_string49:
	.asciz	 "init"
.Linfo_string50:
	.asciz	 "fini"
.Linfo_string51:
	.asciz	 "ldso_fini"
.Linfo_string52:
	.asciz	 "envp"

	.section	".note.GNU-stack","",@progbits
