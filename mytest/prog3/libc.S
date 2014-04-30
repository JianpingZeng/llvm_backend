	.file	"libc.bc"
	.section	.debug_info,"",@progbits
.Lsection_info:
	.section	.debug_abbrev,"",@progbits
.Lsection_abbrev:
	.section	.debug_aranges,"",@progbits
	.section	.debug_macinfo,"",@progbits
	.section	.debug_line,"",@progbits
.Lsection_line:
	.section	.debug_loc,"",@progbits
	.section	.debug_pubnames,"",@progbits
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
	#DEBUG_VALUE: llexit:s <- RDI
	#DEBUG_VALUE: __syscall1:n <- 60
	#DEBUG_VALUE: __syscall1:a1 <- RDI
	movl	$60, %eax
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
	#DEBUG_VALUE: llstrlen:s <- RDI
	#DEBUG_VALUE: llstrlen:v <- RDI
	.loc	1 13 0 prologue_end     # libc.c:13:0
	movq	%rdi, %rcx
	andq	$-8, %rcx
.Ltmp4:
	#DEBUG_VALUE: llstrlen:p <- RCX
	.loc	1 14 0                  # libc.c:14:0
	andq	$7, %rdi
.Ltmp5:
	movq	%rdi, %rax
	negq	%rax
.Ltmp6:
	#DEBUG_VALUE: llstrlen:l <- RAX
	cmpq	$6, %rdi
	jbe	.LBB1_1
.Ltmp7:
# BB#22:                                # %entry.sw.default_crit_edge
	#DEBUG_VALUE: llstrlen:p <- RCX
	#DEBUG_VALUE: llstrlen:l <- RAX
	.loc	1 31 0                  # libc.c:31:0
	movq	(%rcx), %rdx
	jmp	.LBB1_23
.Ltmp8:
.LBB1_1:                                # %entry
	#DEBUG_VALUE: llstrlen:p <- RCX
	#DEBUG_VALUE: llstrlen:l <- RAX
	jmpq	*.LJTI1_0(,%rdi,8)
.LBB1_2:                                # %entry.sw.bb3_crit_edge
	#DEBUG_VALUE: llstrlen:p <- RCX
	#DEBUG_VALUE: llstrlen:l <- RAX
	.loc	1 19 0                  # libc.c:19:0
.Ltmp9:
	movq	(%rcx), %rdx
	jmp	.LBB1_5
.Ltmp10:
.LBB1_7:                                # %entry.sw.bb8_crit_edge
	#DEBUG_VALUE: llstrlen:p <- RCX
	#DEBUG_VALUE: llstrlen:l <- RAX
	.loc	1 21 0                  # libc.c:21:0
	movq	(%rcx), %rdx
	jmp	.LBB1_8
.Ltmp11:
.LBB1_10:                               # %entry.sw.bb14_crit_edge
	#DEBUG_VALUE: llstrlen:p <- RCX
	#DEBUG_VALUE: llstrlen:l <- RAX
	.loc	1 23 0                  # libc.c:23:0
	movq	(%rcx), %rdx
	jmp	.LBB1_11
.Ltmp12:
.LBB1_13:                               # %entry.sw.bb20_crit_edge
	#DEBUG_VALUE: llstrlen:p <- RCX
	#DEBUG_VALUE: llstrlen:l <- RAX
	.loc	1 25 0                  # libc.c:25:0
	movq	(%rcx), %rdx
	jmp	.LBB1_14
.Ltmp13:
.LBB1_16:                               # %entry.sw.bb26_crit_edge
	#DEBUG_VALUE: llstrlen:p <- RCX
	#DEBUG_VALUE: llstrlen:l <- RAX
	.loc	1 27 0                  # libc.c:27:0
	movq	(%rcx), %rdx
	jmp	.LBB1_17
.Ltmp14:
.LBB1_19:                               # %entry.sw.bb32_crit_edge
	#DEBUG_VALUE: llstrlen:p <- RCX
	#DEBUG_VALUE: llstrlen:l <- RAX
	.loc	1 29 0                  # libc.c:29:0
	movq	(%rcx), %rdx
	jmp	.LBB1_20
.Ltmp15:
.LBB1_3:                                # %if.end42
	.loc	1 33 0                  # libc.c:33:0
	addq	$8, %rcx
.Ltmp16:
	#DEBUG_VALUE: llstrlen:p <- RCX
	.loc	1 32 0                  # libc.c:32:0
	addq	$8, %rax
.Ltmp17:
	#DEBUG_VALUE: llstrlen:l <- RAX
.LBB1_4:                                # %do.body
	.loc	1 17 0                  # libc.c:17:0
	movq	(%rcx), %rdx
	testb	%dl, %dl
	je	.LBB1_25
.Ltmp18:
.LBB1_5:                                # %sw.bb3
	.loc	1 19 0                  # libc.c:19:0
	testb	$-1, %dh
	je	.LBB1_6
.Ltmp19:
.LBB1_8:                                # %sw.bb8
	.loc	1 21 0                  # libc.c:21:0
	testl	$16711680, %edx         # imm = 0xFF0000
	je	.LBB1_9
.Ltmp20:
.LBB1_11:                               # %sw.bb14
	.loc	1 23 0                  # libc.c:23:0
	testl	$-16777216, %edx        # imm = 0xFFFFFFFFFF000000
	je	.LBB1_12
.Ltmp21:
.LBB1_14:                               # %sw.bb20
	movabsq	$1095216660480, %rsi    # imm = 0xFF00000000
	.loc	1 25 0                  # libc.c:25:0
.Ltmp22:
	testq	%rsi, %rdx
	je	.LBB1_15
.Ltmp23:
.LBB1_17:                               # %sw.bb26
	movabsq	$280375465082880, %rsi  # imm = 0xFF0000000000
	.loc	1 27 0                  # libc.c:27:0
.Ltmp24:
	testq	%rsi, %rdx
	je	.LBB1_18
.Ltmp25:
.LBB1_20:                               # %sw.bb32
	movabsq	$71776119061217280, %rsi # imm = 0xFF000000000000
	.loc	1 29 0                  # libc.c:29:0
.Ltmp26:
	testq	%rsi, %rdx
	je	.LBB1_21
.Ltmp27:
.LBB1_23:                               # %sw.default
	.loc	1 31 0                  # libc.c:31:0
	shrq	$56, %rdx
	jne	.LBB1_3
# BB#24:                                # %if.then40
	addq	$7, %rax
.Ltmp28:
.LBB1_25:                               # %sw.epilog
	.loc	1 36 0                  # libc.c:36:0
	ret
.LBB1_6:                                # %if.then6
	.loc	1 19 0                  # libc.c:19:0
.Ltmp29:
	incq	%rax
.Ltmp30:
	.loc	1 36 0                  # libc.c:36:0
	ret
.LBB1_9:                                # %if.then11
	.loc	1 21 0                  # libc.c:21:0
.Ltmp31:
	addq	$2, %rax
.Ltmp32:
	.loc	1 36 0                  # libc.c:36:0
	ret
.LBB1_12:                               # %if.then17
	.loc	1 23 0                  # libc.c:23:0
.Ltmp33:
	addq	$3, %rax
.Ltmp34:
	.loc	1 36 0                  # libc.c:36:0
	ret
.LBB1_15:                               # %if.then23
	.loc	1 25 0                  # libc.c:25:0
.Ltmp35:
	addq	$4, %rax
.Ltmp36:
	.loc	1 36 0                  # libc.c:36:0
	ret
.LBB1_18:                               # %if.then29
	.loc	1 27 0                  # libc.c:27:0
.Ltmp37:
	addq	$5, %rax
.Ltmp38:
	.loc	1 36 0                  # libc.c:36:0
	ret
.LBB1_21:                               # %if.then35
	.loc	1 29 0                  # libc.c:29:0
.Ltmp39:
	addq	$6, %rax
.Ltmp40:
	.loc	1 36 0                  # libc.c:36:0
	ret
.Ltmp41:
.Ltmp42:
	.size	llstrlen, .Ltmp42-llstrlen
.Lfunc_end1:
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
.Lfunc_begin2:
	.loc	1 38 0                  # libc.c:38:0
# BB#0:                                 # %entry
	pushq	%rbx
.Ltmp45:
	.cfi_def_cfa_offset 16
.Ltmp46:
	.cfi_offset %rbx, -16
	#DEBUG_VALUE: puts:s <- RDI
	movq	%rdi, %rbx
.Ltmp47:
	#DEBUG_VALUE: puts:s <- RBX
	.loc	1 39 0 prologue_end     # libc.c:39:0
	callq	llstrlen
	movq	%rax, %rcx
.Ltmp48:
	#DEBUG_VALUE: __syscall3:a1 <- 1
	#DEBUG_VALUE: __syscall3:n <- 1
	movl	$1, %eax
	movl	$1, %edi
	.loc	2 339 0                 # ./syscall.h:339:0
.Ltmp49:
	movq	%rbx, %rsi
	movq	%rcx, %rdx
	#APP
	syscall
	#NO_APP
.Ltmp50:
	#DEBUG_VALUE: __syscall3:ret <- RAX
	.loc	1 39 9                  # libc.c:39:9
	popq	%rbx
.Ltmp51:
	ret
.Ltmp52:
.Ltmp53:
	.size	puts, .Ltmp53-puts
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
	#DEBUG_VALUE: putchar:c <- RDI
	.loc	1 44 0 prologue_end     # libc.c:44:0
	movq	%rdi, -8(%rsp)
	leaq	-8(%rsp), %rsi
.Ltmp54:
	#DEBUG_VALUE: __syscall3:a1 <- 1
	#DEBUG_VALUE: __syscall3:n <- 1
	movl	$1, %eax
	movl	$1, %edi
.Ltmp55:
	movl	$1, %edx
	.loc	2 339 0                 # ./syscall.h:339:0
.Ltmp56:
	#APP
	syscall
	#NO_APP
.Ltmp57:
	#DEBUG_VALUE: __syscall3:ret <- RAX
	.loc	1 45 9                  # libc.c:45:9
	ret
.Ltmp58:
.Ltmp59:
	.size	putchar, .Ltmp59-putchar
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
	#DEBUG_VALUE: malloc:size <- RDI
	movq	%rdi, %rdx
.Ltmp60:
	#DEBUG_VALUE: malloc:size <- RDX
	.loc	1 50 0 prologue_end     # libc.c:50:0
	addq	$7, %rdx
.Ltmp61:
	andq	$-8, %rdx
.Ltmp62:
	#DEBUG_VALUE: malloc:size <- RDX
	movq	current_brk(%rip), %rdi
	.loc	1 51 0                  # libc.c:51:0
	addq	%rdx, %rdi
	movq	%rdi, current_brk(%rip)
.Ltmp63:
	#DEBUG_VALUE: __syscall1:a1 <- RDI
	#DEBUG_VALUE: __syscall1:n <- 12
	movl	$12, %eax
	.loc	2 324 0                 # ./syscall.h:324:0
.Ltmp64:
	#APP
	syscall
	#NO_APP
.Ltmp65:
	.loc	1 53 0                  # libc.c:53:0
	movq	current_brk(%rip), %rax
	subq	%rdx, %rax
	ret
.Ltmp66:
.Ltmp67:
	.size	malloc, .Ltmp67-malloc
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
	#DEBUG_VALUE: isspace:c <- RDI
	movb	$1, %al
	cmpq	$32, %rdi
	je	.LBB5_2
# BB#1:                                 # %lor.rhs
.Ltmp68:
	#DEBUG_VALUE: isspace:c <- RDI
	.loc	1 59 0 prologue_end     # libc.c:59:0
	addq	$-9, %rdi
.Ltmp69:
	cmpq	$5, %rdi
	setb	%al
.LBB5_2:                                # %lor.end
	movzbl	%al, %eax
	ret
.Ltmp70:
.Ltmp71:
	.size	isspace, .Ltmp71-isspace
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
	#DEBUG_VALUE: isdigit:c <- RDI
	.loc	1 64 0 prologue_end     # libc.c:64:0
	addq	$-48, %rdi
.Ltmp72:
	cmpq	$10, %rdi
	sbbq	%rax, %rax
	andq	$1, %rax
	ret
.Ltmp73:
.Ltmp74:
	.size	isdigit, .Ltmp74-isdigit
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
	#DEBUG_VALUE: LLreadchar:c <- RDI
	#DEBUG_VALUE: LLreadchar:v <- RDI
	.loc	1 70 0 prologue_end     # libc.c:70:0
	movq	%rdi, %rax
	andq	$-8, %rax
.Ltmp75:
	#DEBUG_VALUE: LLreadchar:p <- RAX
	.loc	1 71 0                  # libc.c:71:0
	movq	(%rax), %rax
.Ltmp76:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	.loc	1 72 0                  # libc.c:72:0
	andq	$7, %rdi
.Ltmp77:
	decq	%rdi
	cmpq	$6, %rdi
	ja	.LBB7_9
.Ltmp78:
# BB#1:                                 # %entry
	#DEBUG_VALUE: LLreadchar:r <- RAX
	jmpq	*.LJTI7_0(,%rdi,8)
.LBB7_2:                                # %sw.bb2
.Ltmp79:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	.loc	1 74 0                  # libc.c:74:0
	shrq	$8, %rax
.Ltmp80:
	.loc	1 82 0                  # libc.c:82:0
	movzbl	%al, %eax
	ret
.LBB7_3:                                # %sw.bb4
.Ltmp81:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	.loc	1 75 0                  # libc.c:75:0
	shrq	$16, %rax
.Ltmp82:
	.loc	1 82 0                  # libc.c:82:0
	movzbl	%al, %eax
	ret
.LBB7_4:                                # %sw.bb6
.Ltmp83:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	.loc	1 76 0                  # libc.c:76:0
	shrq	$24, %rax
.Ltmp84:
	.loc	1 82 0                  # libc.c:82:0
	movzbl	%al, %eax
	ret
.LBB7_5:                                # %sw.bb8
.Ltmp85:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	.loc	1 77 0                  # libc.c:77:0
	shrq	$32, %rax
.Ltmp86:
	.loc	1 82 0                  # libc.c:82:0
	movzbl	%al, %eax
	ret
.LBB7_6:                                # %sw.bb10
.Ltmp87:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	.loc	1 78 0                  # libc.c:78:0
	shrq	$40, %rax
.Ltmp88:
	.loc	1 82 0                  # libc.c:82:0
	movzbl	%al, %eax
	ret
.LBB7_7:                                # %sw.bb12
.Ltmp89:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	.loc	1 79 0                  # libc.c:79:0
	shrq	$48, %rax
.Ltmp90:
	.loc	1 82 0                  # libc.c:82:0
	movzbl	%al, %eax
	ret
.LBB7_8:                                # %sw.bb14
.Ltmp91:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	.loc	1 80 0                  # libc.c:80:0
	shrq	$56, %rax
.Ltmp92:
.LBB7_9:                                # %sw.epilog
	.loc	1 82 0                  # libc.c:82:0
	movzbl	%al, %eax
	ret
.Ltmp93:
.Ltmp94:
	.size	LLreadchar, .Ltmp94-LLreadchar
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
	#DEBUG_VALUE: atol:s <- RDI
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	movabsq	$4294983168, %rcx       # imm = 0x100003E00
	jmp	.LBB8_1
.Ltmp95:
	.align	16, 0x90
.LBB8_12:                               # %while.body
                                        #   in Loop: Header=BB8_1 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 89 0 prologue_end     # libc.c:89:0
	incq	%rdi
.Ltmp96:
	#DEBUG_VALUE: atol:s <- RDI
.LBB8_1:                                # %while.cond
                                        # =>This Inner Loop Header: Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	#DEBUG_VALUE: LLreadchar:c <- RDI
	#DEBUG_VALUE: LLreadchar:v <- RDI
	#DEBUG_VALUE: LLreadchar:v <- RDI
	.loc	1 70 0                  # libc.c:70:0
	movq	%rdi, %rax
	andq	$-8, %rax
.Ltmp97:
	#DEBUG_VALUE: LLreadchar:p <- RAX
	#DEBUG_VALUE: LLreadchar:p <- RAX
	.loc	1 71 0                  # libc.c:71:0
	movq	(%rax), %rax
.Ltmp98:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	#DEBUG_VALUE: LLreadchar:r <- RAX
	.loc	1 72 0                  # libc.c:72:0
	movq	%rdi, %rdx
	andq	$7, %rdx
	decq	%rdx
	cmpq	$6, %rdx
	movq	%rax, %rsi
	ja	.LBB8_10
.Ltmp99:
# BB#2:                                 # %while.cond
                                        #   in Loop: Header=BB8_1 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	jmpq	*.LJTI8_0(,%rdx,8)
.LBB8_3:                                # %sw.bb2.i
                                        #   in Loop: Header=BB8_1 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 74 0                  # libc.c:74:0
	movq	%rax, %rsi
	shrq	$8, %rsi
.Ltmp100:
	#DEBUG_VALUE: LLreadchar:r <- RSI
	jmp	.LBB8_10
.Ltmp101:
.LBB8_4:                                # %sw.bb4.i
                                        #   in Loop: Header=BB8_1 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 75 0                  # libc.c:75:0
	movq	%rax, %rsi
	shrq	$16, %rsi
.Ltmp102:
	#DEBUG_VALUE: LLreadchar:r <- RSI
	jmp	.LBB8_10
.Ltmp103:
.LBB8_5:                                # %sw.bb6.i
                                        #   in Loop: Header=BB8_1 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 76 0                  # libc.c:76:0
	movq	%rax, %rsi
	shrq	$24, %rsi
.Ltmp104:
	#DEBUG_VALUE: LLreadchar:r <- RSI
	jmp	.LBB8_10
.Ltmp105:
.LBB8_6:                                # %sw.bb8.i
                                        #   in Loop: Header=BB8_1 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 77 0                  # libc.c:77:0
	movq	%rax, %rsi
	shrq	$32, %rsi
.Ltmp106:
	#DEBUG_VALUE: LLreadchar:r <- RSI
	jmp	.LBB8_10
.Ltmp107:
.LBB8_7:                                # %sw.bb10.i
                                        #   in Loop: Header=BB8_1 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 78 0                  # libc.c:78:0
	movq	%rax, %rsi
	shrq	$40, %rsi
.Ltmp108:
	#DEBUG_VALUE: LLreadchar:r <- RSI
	jmp	.LBB8_10
.Ltmp109:
.LBB8_8:                                # %sw.bb12.i
                                        #   in Loop: Header=BB8_1 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 79 0                  # libc.c:79:0
	movq	%rax, %rsi
	shrq	$48, %rsi
.Ltmp110:
	#DEBUG_VALUE: LLreadchar:r <- RSI
	jmp	.LBB8_10
.Ltmp111:
.LBB8_9:                                # %sw.bb14.i
                                        #   in Loop: Header=BB8_1 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 80 0                  # libc.c:80:0
	movq	%rax, %rsi
	shrq	$56, %rsi
.Ltmp112:
	#DEBUG_VALUE: LLreadchar:r <- RSI
	.align	16, 0x90
.LBB8_10:                               # %LLreadchar.exit
                                        #   in Loop: Header=BB8_1 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	#DEBUG_VALUE: LLreadchar:c <- RDI
	#DEBUG_VALUE: LLreadchar:v <- RDI
	#DEBUG_VALUE: LLreadchar:r <- RAX
	.loc	1 82 0                  # libc.c:82:0
	movzbl	%sil, %esi
.Ltmp113:
	#DEBUG_VALUE: isspace:c <- RSI
	cmpq	$32, %rsi
	ja	.LBB8_13
.Ltmp114:
# BB#11:                                # %LLreadchar.exit
                                        #   in Loop: Header=BB8_1 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	btq	%rsi, %rcx
	jb	.LBB8_12
.LBB8_13:                               # %while.end
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 72 0                  # libc.c:72:0
.Ltmp115:
	cmpq	$6, %rdx
	ja	.LBB8_22
# BB#14:                                # %while.end
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	jmpq	*.LJTI8_1(,%rdx,8)
.LBB8_15:                               # %sw.bb2.i63
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 74 0                  # libc.c:74:0
	shrq	$8, %rax
.Ltmp116:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	jmp	.LBB8_22
.Ltmp117:
.LBB8_16:                               # %sw.bb4.i65
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 75 0                  # libc.c:75:0
	shrq	$16, %rax
.Ltmp118:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	jmp	.LBB8_22
.Ltmp119:
.LBB8_17:                               # %sw.bb6.i67
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 76 0                  # libc.c:76:0
	shrq	$24, %rax
.Ltmp120:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	jmp	.LBB8_22
.Ltmp121:
.LBB8_18:                               # %sw.bb8.i69
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 77 0                  # libc.c:77:0
	shrq	$32, %rax
.Ltmp122:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	jmp	.LBB8_22
.Ltmp123:
.LBB8_19:                               # %sw.bb10.i71
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 78 0                  # libc.c:78:0
	shrq	$40, %rax
.Ltmp124:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	jmp	.LBB8_22
.Ltmp125:
.LBB8_20:                               # %sw.bb12.i73
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 79 0                  # libc.c:79:0
	shrq	$48, %rax
.Ltmp126:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	jmp	.LBB8_22
.Ltmp127:
.LBB8_21:                               # %sw.bb14.i75
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 80 0                  # libc.c:80:0
	shrq	$56, %rax
.Ltmp128:
	#DEBUG_VALUE: LLreadchar:r <- RAX
.LBB8_22:                               # %LLreadchar.exit78
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	xorl	%r8d, %r8d
	.loc	1 82 0                  # libc.c:82:0
	movzbl	%al, %eax
	cmpq	$43, %rax
	je	.LBB8_25
.Ltmp129:
# BB#23:                                # %LLreadchar.exit78
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	cmpq	$45, %rax
	jne	.LBB8_26
# BB#24:                                # %sw.bb
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	movl	$1, %r8d
.Ltmp130:
	#DEBUG_VALUE: atol:neg <- 1
.LBB8_25:                               # %sw.bb3
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	incq	%rdi
.LBB8_26:                               # %while.cond5.preheader
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	xorl	%edx, %edx
	jmp	.LBB8_27
	.align	16, 0x90
.LBB8_46:                               # %LLreadchar.exit40
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 96 0                  # libc.c:96:0
	incq	%rdi
.Ltmp131:
	#DEBUG_VALUE: atol:s <- RDI
	addq	%rdx, %rdx
	leaq	(%rdx,%rdx,4), %rdx
	addq	$48, %rdx
	.loc	1 82 0                  # libc.c:82:0
.Ltmp132:
	movzbl	%cl, %eax
.Ltmp133:
	.loc	1 96 0                  # libc.c:96:0
	subq	%rax, %rdx
.Ltmp134:
	#DEBUG_VALUE: atol:n <- RDX
.LBB8_27:                               # %while.cond5
                                        # =>This Inner Loop Header: Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	#DEBUG_VALUE: LLreadchar:c <- RDI
	#DEBUG_VALUE: LLreadchar:v <- RDI
	#DEBUG_VALUE: LLreadchar:v <- RDI
	.loc	1 70 0                  # libc.c:70:0
	movq	%rdi, %rax
	andq	$-8, %rax
.Ltmp135:
	#DEBUG_VALUE: LLreadchar:p <- RAX
	#DEBUG_VALUE: LLreadchar:p <- RAX
	.loc	1 71 0                  # libc.c:71:0
	movq	(%rax), %rcx
.Ltmp136:
	#DEBUG_VALUE: LLreadchar:r <- RCX
	#DEBUG_VALUE: LLreadchar:r <- RCX
	.loc	1 72 0                  # libc.c:72:0
	movq	%rdi, %rsi
	andq	$7, %rsi
	decq	%rsi
	cmpq	$6, %rsi
	movq	%rcx, %rax
.Ltmp137:
	ja	.LBB8_36
.Ltmp138:
# BB#28:                                # %while.cond5
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	jmpq	*.LJTI8_2(,%rsi,8)
.LBB8_29:                               # %sw.bb2.i44
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 74 0                  # libc.c:74:0
	movq	%rcx, %rax
	shrq	$8, %rax
.Ltmp139:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	jmp	.LBB8_36
.Ltmp140:
.LBB8_30:                               # %sw.bb4.i46
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 75 0                  # libc.c:75:0
	movq	%rcx, %rax
	shrq	$16, %rax
.Ltmp141:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	jmp	.LBB8_36
.Ltmp142:
.LBB8_31:                               # %sw.bb6.i48
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 76 0                  # libc.c:76:0
	movq	%rcx, %rax
	shrq	$24, %rax
.Ltmp143:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	jmp	.LBB8_36
.Ltmp144:
.LBB8_32:                               # %sw.bb8.i50
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 77 0                  # libc.c:77:0
	movq	%rcx, %rax
	shrq	$32, %rax
.Ltmp145:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	jmp	.LBB8_36
.Ltmp146:
.LBB8_33:                               # %sw.bb10.i52
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 78 0                  # libc.c:78:0
	movq	%rcx, %rax
	shrq	$40, %rax
.Ltmp147:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	jmp	.LBB8_36
.Ltmp148:
.LBB8_34:                               # %sw.bb12.i54
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 79 0                  # libc.c:79:0
	movq	%rcx, %rax
	shrq	$48, %rax
.Ltmp149:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	jmp	.LBB8_36
.Ltmp150:
.LBB8_35:                               # %sw.bb14.i56
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 80 0                  # libc.c:80:0
	movq	%rcx, %rax
	shrq	$56, %rax
.Ltmp151:
	#DEBUG_VALUE: LLreadchar:r <- RAX
	.align	16, 0x90
.LBB8_36:                               # %LLreadchar.exit59
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	#DEBUG_VALUE: LLreadchar:c <- RDI
	#DEBUG_VALUE: LLreadchar:v <- RDI
	#DEBUG_VALUE: LLreadchar:r <- RCX
	.loc	1 82 0                  # libc.c:82:0
	movzbl	%al, %eax
.Ltmp152:
	#DEBUG_VALUE: isdigit:c <- RAX
	.loc	1 64 0                  # libc.c:64:0
	addq	$-48, %rax
.Ltmp153:
	cmpq	$9, %rax
.Ltmp154:
	.loc	1 95 0                  # libc.c:95:0
	ja	.LBB8_47
.Ltmp155:
# BB#37:                                # %while.body9
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 72 0                  # libc.c:72:0
	cmpq	$6, %rsi
	ja	.LBB8_46
# BB#38:                                # %while.body9
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	jmpq	*.LJTI8_3(,%rsi,8)
.LBB8_39:                               # %sw.bb2.i25
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 74 0                  # libc.c:74:0
	shrq	$8, %rcx
.Ltmp156:
	#DEBUG_VALUE: LLreadchar:r <- RCX
	jmp	.LBB8_46
.Ltmp157:
.LBB8_40:                               # %sw.bb4.i27
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 75 0                  # libc.c:75:0
	shrq	$16, %rcx
.Ltmp158:
	#DEBUG_VALUE: LLreadchar:r <- RCX
	jmp	.LBB8_46
.Ltmp159:
.LBB8_41:                               # %sw.bb6.i29
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 76 0                  # libc.c:76:0
	shrq	$24, %rcx
.Ltmp160:
	#DEBUG_VALUE: LLreadchar:r <- RCX
	jmp	.LBB8_46
.Ltmp161:
.LBB8_42:                               # %sw.bb8.i31
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 77 0                  # libc.c:77:0
	shrq	$32, %rcx
.Ltmp162:
	#DEBUG_VALUE: LLreadchar:r <- RCX
	jmp	.LBB8_46
.Ltmp163:
.LBB8_43:                               # %sw.bb10.i33
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 78 0                  # libc.c:78:0
	shrq	$40, %rcx
.Ltmp164:
	#DEBUG_VALUE: LLreadchar:r <- RCX
	jmp	.LBB8_46
.Ltmp165:
.LBB8_44:                               # %sw.bb12.i35
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 79 0                  # libc.c:79:0
	shrq	$48, %rcx
.Ltmp166:
	#DEBUG_VALUE: LLreadchar:r <- RCX
	jmp	.LBB8_46
.Ltmp167:
.LBB8_45:                               # %sw.bb14.i37
                                        #   in Loop: Header=BB8_27 Depth=1
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 80 0                  # libc.c:80:0
	shrq	$56, %rcx
.Ltmp168:
	#DEBUG_VALUE: LLreadchar:r <- RCX
	jmp	.LBB8_46
.Ltmp169:
.LBB8_47:                               # %while.end13
	#DEBUG_VALUE: atol:n <- 0
	#DEBUG_VALUE: atol:neg <- 0
	.loc	1 97 0                  # libc.c:97:0
	movq	%rdx, %rax
	negq	%rax
.Ltmp170:
	#DEBUG_VALUE: atol:n <- RAX
	testq	%r8, %r8
	cmovneq	%rdx, %rax
.Ltmp171:
	.loc	1 98 0                  # libc.c:98:0
	ret
.Ltmp172:
.Ltmp173:
	.size	atol, .Ltmp173-atol
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
	.quad	.LBB8_15
	.quad	.LBB8_16
	.quad	.LBB8_17
	.quad	.LBB8_18
	.quad	.LBB8_19
	.quad	.LBB8_20
	.quad	.LBB8_21
.LJTI8_2:
	.quad	.LBB8_29
	.quad	.LBB8_30
	.quad	.LBB8_31
	.quad	.LBB8_32
	.quad	.LBB8_33
	.quad	.LBB8_34
	.quad	.LBB8_35
.LJTI8_3:
	.quad	.LBB8_39
	.quad	.LBB8_40
	.quad	.LBB8_41
	.quad	.LBB8_42
	.quad	.LBB8_43
	.quad	.LBB8_44
	.quad	.LBB8_45

	.text
	.globl	time
	.align	16, 0x90
	.type	time,@function
time:                                   # @time
	.cfi_startproc
.Lfunc_begin9:
	.loc	1 102 0                 # libc.c:102:0
# BB#0:                                 # %entry
	#DEBUG_VALUE: time:t <- RDI
	movq	%rdi, %rdx
.Ltmp174:
	#DEBUG_VALUE: time:t <- RDX
	leaq	-16(%rsp), %rsi
.Ltmp175:
	#DEBUG_VALUE: __syscall2:a1 <- 0
	#DEBUG_VALUE: __syscall2:n <- 228
	movl	$228, %eax
	xorl	%edi, %edi
	.loc	2 331 0 prologue_end    # ./syscall.h:331:0
.Ltmp176:
	#APP
	syscall
	#NO_APP
.Ltmp177:
	.loc	1 106 0                 # libc.c:106:0
	movq	-16(%rsp), %rax
	testq	%rdx, %rdx
	je	.LBB9_2
# BB#1:                                 # %if.then
.Ltmp178:
	#DEBUG_VALUE: time:t <- RDX
	.loc	1 105 0                 # libc.c:105:0
	movq	%rax, (%rdx)
.Ltmp179:
.LBB9_2:                                # %if.end
	.loc	1 106 0                 # libc.c:106:0
	ret
.Ltmp180:
.Ltmp181:
	.size	time, .Ltmp181-time
.Lfunc_end9:
	.file	3 "./libc.h"
	.cfi_endproc

	.globl	__libc_start_main
	.align	16, 0x90
	.type	__libc_start_main,@function
__libc_start_main:                      # @__libc_start_main
	.cfi_startproc
.Lfunc_begin10:
	.loc	1 113 0                 # libc.c:113:0
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp183:
	.cfi_def_cfa_offset 16
	#DEBUG_VALUE: __libc_start_main:main <- RDI
	#DEBUG_VALUE: __libc_start_main:argc <- RSI
	#DEBUG_VALUE: __libc_start_main:argv <- RDX
	#DEBUG_VALUE: __libc_start_main:init <- RCX
	#DEBUG_VALUE: __libc_start_main:fini <- R8
	#DEBUG_VALUE: __libc_start_main:ldso_fini <- R9
	movq	%rdi, %r8
.Ltmp184:
	#DEBUG_VALUE: __syscall1:a1 <- 0
	#DEBUG_VALUE: __syscall1:n <- 12
	#DEBUG_VALUE: __libc_start_main:main <- R8
	movl	$12, %eax
	xorl	%edi, %edi
	.loc	2 324 0 prologue_end    # ./syscall.h:324:0
.Ltmp185:
	#APP
	syscall
	#NO_APP
.Ltmp186:
	#DEBUG_VALUE: __syscall1:ret <- RAX
	.loc	1 124 23                # libc.c:124:23
	movq	%rax, current_brk(%rip)
	.loc	1 127 0                 # libc.c:127:0
	movq	%rsi, %rdi
.Ltmp187:
	#DEBUG_VALUE: __libc_start_main:argc <- RDI
	movq	%rdx, %rsi
.Ltmp188:
	#DEBUG_VALUE: __libc_start_main:argv <- RSI
	xorl	%edx, %edx
	callq	*%r8
.Ltmp189:
	movq	%rax, %rcx
.Ltmp190:
	#DEBUG_VALUE: __syscall1:a1 <- RCX
	#DEBUG_VALUE: __syscall1:n <- 60
	#DEBUG_VALUE: llexit:s <- RCX
	movl	$60, %eax
	.loc	2 324 0                 # ./syscall.h:324:0
.Ltmp191:
	movq	%rcx, %rdi
.Ltmp192:
	#APP
	syscall
	#NO_APP
.Ltmp193:
	xorl	%eax, %eax
.Ltmp194:
	.loc	1 131 0                 # libc.c:131:0
	popq	%rdx
	ret
.Ltmp195:
.Ltmp196:
	.size	__libc_start_main, .Ltmp196-__libc_start_main
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
.L.debug_info_begin0:
	.long	1891                    # Length of Compilation Unit Info
	.short	4                       # DWARF version number
	.long	.L.debug_abbrev_begin   # Offset Into Abbrev. Section
	.byte	8                       # Address Size (in bytes)
	.byte	1                       # Abbrev [1] 0xb:0x75c DW_TAG_compile_unit
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
	.byte	5                       # Abbrev [5] 0x47:0x4d DW_TAG_subprogram
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
	.byte	6                       # Abbrev [6] 0x61:0xd DW_TAG_formal_parameter
	.long	.Linfo_string28         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	6                       # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	1                       # DW_AT_location
	.byte	85
	.byte	7                       # Abbrev [7] 0x6e:0x25 DW_TAG_inlined_subroutine
	.long	1711                    # DW_AT_abstract_origin
	.quad	.Ltmp0                  # DW_AT_low_pc
	.quad	.Ltmp1                  # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	7                       # DW_AT_call_line
	.byte	8                       # Abbrev [8] 0x85:0x6 DW_TAG_formal_parameter
	.long	1724                    # DW_AT_abstract_origin
	.byte	60                      # DW_AT_const_value
	.byte	9                       # Abbrev [9] 0x8b:0x7 DW_TAG_formal_parameter
	.long	1736                    # DW_AT_abstract_origin
	.byte	1                       # DW_AT_location
	.byte	85
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	10                      # Abbrev [10] 0x94:0x5a DW_TAG_subprogram
	.long	.Linfo_string6          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	10                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	245                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin1           # DW_AT_low_pc
	.quad	.Lfunc_end1             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	11                      # Abbrev [11] 0xb1:0xf DW_TAG_formal_parameter
	.long	.Linfo_string28         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	10                      # DW_AT_decl_line
	.long	1766                    # DW_AT_type
	.long	.Ldebug_loc0            # DW_AT_location
	.byte	12                      # Abbrev [12] 0xc0:0xf DW_TAG_variable
	.long	.Linfo_string29         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	12                      # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.long	.Ldebug_loc2            # DW_AT_location
	.byte	12                      # Abbrev [12] 0xcf:0xf DW_TAG_variable
	.long	.Linfo_string30         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	13                      # DW_AT_decl_line
	.long	1771                    # DW_AT_type
	.long	.Ldebug_loc4            # DW_AT_location
	.byte	12                      # Abbrev [12] 0xde:0xf DW_TAG_variable
	.long	.Linfo_string31         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	11                      # DW_AT_decl_line
	.long	752                     # DW_AT_type
	.long	.Ldebug_loc7            # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	2                       # Abbrev [2] 0xee:0x7 DW_TAG_base_type
	.long	.Linfo_string7          # DW_AT_name
	.byte	7                       # DW_AT_encoding
	.byte	8                       # DW_AT_byte_size
	.byte	13                      # Abbrev [13] 0xf5:0xb DW_TAG_typedef
	.long	238                     # DW_AT_type
	.long	.Linfo_string8          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	15                      # DW_AT_decl_line
	.byte	10                      # Abbrev [10] 0x100:0x58 DW_TAG_subprogram
	.long	.Linfo_string9          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	38                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	344                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin2           # DW_AT_low_pc
	.quad	.Lfunc_end2             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	11                      # Abbrev [11] 0x11d:0xf DW_TAG_formal_parameter
	.long	.Linfo_string28         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	38                      # DW_AT_decl_line
	.long	1766                    # DW_AT_type
	.long	.Ldebug_loc10           # DW_AT_location
	.byte	7                       # Abbrev [7] 0x12c:0x2b DW_TAG_inlined_subroutine
	.long	1637                    # DW_AT_abstract_origin
	.quad	.Ltmp49                 # DW_AT_low_pc
	.quad	.Ltmp50                 # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	39                      # DW_AT_call_line
	.byte	8                       # Abbrev [8] 0x143:0x6 DW_TAG_formal_parameter
	.long	1650                    # DW_AT_abstract_origin
	.byte	1                       # DW_AT_const_value
	.byte	8                       # Abbrev [8] 0x149:0x6 DW_TAG_formal_parameter
	.long	1662                    # DW_AT_abstract_origin
	.byte	1                       # DW_AT_const_value
	.byte	14                      # Abbrev [14] 0x14f:0x7 DW_TAG_variable
	.long	1698                    # DW_AT_abstract_origin
	.byte	1                       # DW_AT_location
	.byte	80
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	2                       # Abbrev [2] 0x158:0x7 DW_TAG_base_type
	.long	.Linfo_string10         # DW_AT_name
	.byte	5                       # DW_AT_encoding
	.byte	8                       # DW_AT_byte_size
	.byte	10                      # Abbrev [10] 0x15f:0x7b DW_TAG_subprogram
	.long	.Linfo_string11         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	42                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	344                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin3           # DW_AT_low_pc
	.quad	.Lfunc_end3             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	11                      # Abbrev [11] 0x17c:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	42                      # DW_AT_decl_line
	.long	752                     # DW_AT_type
	.long	.Ldebug_loc13           # DW_AT_location
	.byte	15                      # Abbrev [15] 0x18b:0xe DW_TAG_variable
	.long	.Linfo_string28         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	43                      # DW_AT_decl_line
	.long	1783                    # DW_AT_type
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	120
	.byte	7                       # Abbrev [7] 0x199:0x40 DW_TAG_inlined_subroutine
	.long	1637                    # DW_AT_abstract_origin
	.quad	.Ltmp56                 # DW_AT_low_pc
	.quad	.Ltmp57                 # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	45                      # DW_AT_call_line
	.byte	16                      # Abbrev [16] 0x1b0:0xd DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	1                       # DW_AT_const_value
	.byte	16                      # Abbrev [16] 0x1bd:0xd DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	1                       # DW_AT_const_value
	.byte	17                      # Abbrev [17] 0x1ca:0xe DW_TAG_variable
	.long	.Linfo_string27         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	338                     # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.byte	1                       # DW_AT_location
	.byte	80
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	10                      # Abbrev [10] 0x1da:0x60 DW_TAG_subprogram
	.long	.Linfo_string12         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	49                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	570                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin4           # DW_AT_low_pc
	.quad	.Lfunc_end4             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	11                      # Abbrev [11] 0x1f7:0xf DW_TAG_formal_parameter
	.long	.Linfo_string36         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	49                      # DW_AT_decl_line
	.long	1795                    # DW_AT_type
	.long	.Ldebug_loc15           # DW_AT_location
	.byte	7                       # Abbrev [7] 0x206:0x33 DW_TAG_inlined_subroutine
	.long	1711                    # DW_AT_abstract_origin
	.quad	.Ltmp64                 # DW_AT_low_pc
	.quad	.Ltmp65                 # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	52                      # DW_AT_call_line
	.byte	16                      # Abbrev [16] 0x21d:0xd DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	12                      # DW_AT_const_value
	.byte	18                      # Abbrev [18] 0x22a:0xe DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	1                       # DW_AT_location
	.byte	85
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	19                      # Abbrev [19] 0x23a:0x1 DW_TAG_pointer_type
	.byte	10                      # Abbrev [10] 0x23b:0x2d DW_TAG_subprogram
	.long	.Linfo_string13         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	57                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	344                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin5           # DW_AT_low_pc
	.quad	.Lfunc_end5             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	11                      # Abbrev [11] 0x258:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	57                      # DW_AT_decl_line
	.long	752                     # DW_AT_type
	.long	.Ldebug_loc19           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	10                      # Abbrev [10] 0x268:0x2d DW_TAG_subprogram
	.long	.Linfo_string14         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	62                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	344                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin6           # DW_AT_low_pc
	.quad	.Lfunc_end6             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	11                      # Abbrev [11] 0x285:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	62                      # DW_AT_decl_line
	.long	752                     # DW_AT_type
	.long	.Ldebug_loc22           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	20                      # Abbrev [20] 0x295:0x5b DW_TAG_subprogram
	.long	.Linfo_string15         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	67                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	752                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin7           # DW_AT_low_pc
	.quad	.Lfunc_end7             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	1                       # DW_AT_inline
	.byte	11                      # Abbrev [11] 0x2b3:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	67                      # DW_AT_decl_line
	.long	1766                    # DW_AT_type
	.long	.Ldebug_loc24           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x2c2:0xf DW_TAG_variable
	.long	.Linfo_string29         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	69                      # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.long	.Ldebug_loc26           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x2d1:0xf DW_TAG_variable
	.long	.Linfo_string30         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	70                      # DW_AT_decl_line
	.long	1771                    # DW_AT_type
	.long	.Ldebug_loc28           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x2e0:0xf DW_TAG_variable
	.long	.Linfo_string38         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	68                      # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.long	.Ldebug_loc30           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	13                      # Abbrev [13] 0x2f0:0xb DW_TAG_typedef
	.long	344                     # DW_AT_type
	.long	.Linfo_string16         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	4                       # DW_AT_decl_line
	.byte	10                      # Abbrev [10] 0x2fb:0x18f DW_TAG_subprogram
	.long	.Linfo_string17         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	85                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	1162                    # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin8           # DW_AT_low_pc
	.quad	.Lfunc_end8             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	11                      # Abbrev [11] 0x318:0xf DW_TAG_formal_parameter
	.long	.Linfo_string28         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	85                      # DW_AT_decl_line
	.long	1766                    # DW_AT_type
	.long	.Ldebug_loc39           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x327:0xf DW_TAG_variable
	.long	.Linfo_string25         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	87                      # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.long	.Ldebug_loc43           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x336:0xf DW_TAG_variable
	.long	.Linfo_string39         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	88                      # DW_AT_decl_line
	.long	752                     # DW_AT_type
	.long	.Ldebug_loc48           # DW_AT_location
	.byte	7                       # Abbrev [7] 0x345:0x63 DW_TAG_inlined_subroutine
	.long	661                     # DW_AT_abstract_origin
	.quad	.Ltmp96                 # DW_AT_low_pc
	.quad	.Ltmp114                # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	89                      # DW_AT_call_line
	.byte	11                      # Abbrev [11] 0x35c:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	67                      # DW_AT_decl_line
	.long	1766                    # DW_AT_type
	.long	.Ldebug_loc53           # DW_AT_location
	.byte	11                      # Abbrev [11] 0x36b:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	57                      # DW_AT_decl_line
	.long	752                     # DW_AT_type
	.long	.Ldebug_loc84           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x37a:0xf DW_TAG_variable
	.long	.Linfo_string29         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	69                      # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.long	.Ldebug_loc58           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x389:0xf DW_TAG_variable
	.long	.Linfo_string30         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	70                      # DW_AT_decl_line
	.long	1771                    # DW_AT_type
	.long	.Ldebug_loc61           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x398:0xf DW_TAG_variable
	.long	.Linfo_string38         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	68                      # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.long	.Ldebug_loc65           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	7                       # Abbrev [7] 0x3a8:0x45 DW_TAG_inlined_subroutine
	.long	661                     # DW_AT_abstract_origin
	.quad	.Ltmp115                # DW_AT_low_pc
	.quad	.Ltmp129                # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	90                      # DW_AT_call_line
	.byte	12                      # Abbrev [12] 0x3bf:0xf DW_TAG_variable
	.long	.Linfo_string29         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	69                      # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.long	.Ldebug_loc56           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x3ce:0xf DW_TAG_variable
	.long	.Linfo_string30         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	70                      # DW_AT_decl_line
	.long	1771                    # DW_AT_type
	.long	.Ldebug_loc63           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x3dd:0xf DW_TAG_variable
	.long	.Linfo_string38         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	68                      # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.long	.Ldebug_loc75           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	21                      # Abbrev [21] 0x3ed:0x39 DW_TAG_inlined_subroutine
	.long	661                     # DW_AT_abstract_origin
	.long	.Ldebug_range           # DW_AT_ranges
	.byte	1                       # DW_AT_call_file
	.byte	96                      # DW_AT_call_line
	.byte	12                      # Abbrev [12] 0x3f8:0xf DW_TAG_variable
	.long	.Linfo_string29         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	69                      # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.long	.Ldebug_loc89           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x407:0xf DW_TAG_variable
	.long	.Linfo_string30         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	70                      # DW_AT_decl_line
	.long	1771                    # DW_AT_type
	.long	.Ldebug_loc96           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x416:0xf DW_TAG_variable
	.long	.Linfo_string38         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	68                      # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.long	.Ldebug_loc108          # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	7                       # Abbrev [7] 0x426:0x63 DW_TAG_inlined_subroutine
	.long	661                     # DW_AT_abstract_origin
	.quad	.Ltmp134                # DW_AT_low_pc
	.quad	.Ltmp154                # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	95                      # DW_AT_call_line
	.byte	11                      # Abbrev [11] 0x43d:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	67                      # DW_AT_decl_line
	.long	1766                    # DW_AT_type
	.long	.Ldebug_loc86           # DW_AT_location
	.byte	11                      # Abbrev [11] 0x44c:0xf DW_TAG_formal_parameter
	.long	.Linfo_string34         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	62                      # DW_AT_decl_line
	.long	752                     # DW_AT_type
	.long	.Ldebug_loc117          # DW_AT_location
	.byte	12                      # Abbrev [12] 0x45b:0xf DW_TAG_variable
	.long	.Linfo_string29         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	69                      # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.long	.Ldebug_loc91           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x46a:0xf DW_TAG_variable
	.long	.Linfo_string30         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	70                      # DW_AT_decl_line
	.long	1771                    # DW_AT_type
	.long	.Ldebug_loc94           # DW_AT_location
	.byte	12                      # Abbrev [12] 0x479:0xf DW_TAG_variable
	.long	.Linfo_string38         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	68                      # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.long	.Ldebug_loc98           # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	2                       # Abbrev [2] 0x48a:0x7 DW_TAG_base_type
	.long	.Linfo_string18         # DW_AT_name
	.byte	5                       # DW_AT_encoding
	.byte	8                       # DW_AT_byte_size
	.byte	10                      # Abbrev [10] 0x491:0x6e DW_TAG_subprogram
	.long	.Linfo_string19         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	101                     # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	1279                    # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin9           # DW_AT_low_pc
	.quad	.Lfunc_end9             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	11                      # Abbrev [11] 0x4ae:0xf DW_TAG_formal_parameter
	.long	.Linfo_string40         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	101                     # DW_AT_decl_line
	.long	1802                    # DW_AT_type
	.long	.Ldebug_loc119          # DW_AT_location
	.byte	15                      # Abbrev [15] 0x4bd:0xe DW_TAG_variable
	.long	.Linfo_string41         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	103                     # DW_AT_decl_line
	.long	1818                    # DW_AT_type
	.byte	2                       # DW_AT_location
	.byte	145
	.byte	112
	.byte	7                       # Abbrev [7] 0x4cb:0x33 DW_TAG_inlined_subroutine
	.long	1575                    # DW_AT_abstract_origin
	.quad	.Ltmp176                # DW_AT_low_pc
	.quad	.Ltmp177                # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	104                     # DW_AT_call_line
	.byte	16                      # Abbrev [16] 0x4e2:0xe DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	328                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.ascii	 "\344\001"             # DW_AT_const_value
	.byte	16                      # Abbrev [16] 0x4f0:0xd DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	328                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	0                       # DW_AT_const_value
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	13                      # Abbrev [13] 0x4ff:0xb DW_TAG_typedef
	.long	344                     # DW_AT_type
	.long	.Linfo_string20         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	14                      # DW_AT_decl_line
	.byte	10                      # Abbrev [10] 0x50a:0x11d DW_TAG_subprogram
	.long	.Linfo_string21         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	109                     # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	752                     # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin10          # DW_AT_low_pc
	.quad	.Lfunc_end10            # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	11                      # Abbrev [11] 0x527:0xf DW_TAG_formal_parameter
	.long	.Linfo_string46         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	110                     # DW_AT_decl_line
	.long	1883                    # DW_AT_type
	.long	.Ldebug_loc123          # DW_AT_location
	.byte	11                      # Abbrev [11] 0x536:0xf DW_TAG_formal_parameter
	.long	.Linfo_string47         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	110                     # DW_AT_decl_line
	.long	752                     # DW_AT_type
	.long	.Ldebug_loc126          # DW_AT_location
	.byte	11                      # Abbrev [11] 0x545:0xf DW_TAG_formal_parameter
	.long	.Linfo_string48         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	110                     # DW_AT_decl_line
	.long	1857                    # DW_AT_type
	.long	.Ldebug_loc129          # DW_AT_location
	.byte	11                      # Abbrev [11] 0x554:0xf DW_TAG_formal_parameter
	.long	.Linfo_string49         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	111                     # DW_AT_decl_line
	.long	1883                    # DW_AT_type
	.long	.Ldebug_loc132          # DW_AT_location
	.byte	11                      # Abbrev [11] 0x563:0xf DW_TAG_formal_parameter
	.long	.Linfo_string50         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	111                     # DW_AT_decl_line
	.long	1889                    # DW_AT_type
	.long	.Ldebug_loc134          # DW_AT_location
	.byte	6                       # Abbrev [6] 0x572:0xd DW_TAG_formal_parameter
	.long	.Linfo_string51         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	112                     # DW_AT_decl_line
	.long	1889                    # DW_AT_type
	.byte	1                       # DW_AT_location
	.byte	89
	.byte	22                      # Abbrev [22] 0x57f:0xb DW_TAG_variable
	.long	.Linfo_string52         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	114                     # DW_AT_decl_line
	.long	1857                    # DW_AT_type
	.byte	7                       # Abbrev [7] 0x58a:0x42 DW_TAG_inlined_subroutine
	.long	1711                    # DW_AT_abstract_origin
	.quad	.Ltmp185                # DW_AT_low_pc
	.quad	.Ltmp186                # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	124                     # DW_AT_call_line
	.byte	16                      # Abbrev [16] 0x5a1:0xd DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	12                      # DW_AT_const_value
	.byte	16                      # Abbrev [16] 0x5ae:0xd DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	0                       # DW_AT_const_value
	.byte	23                      # Abbrev [23] 0x5bb:0x10 DW_TAG_variable
	.long	.Linfo_string27         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	323                     # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.long	.Ldebug_loc136          # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	7                       # Abbrev [7] 0x5cc:0x5a DW_TAG_inlined_subroutine
	.long	71                      # DW_AT_abstract_origin
	.quad	.Ltmp191                # DW_AT_low_pc
	.quad	.Ltmp194                # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	127                     # DW_AT_call_line
	.byte	11                      # Abbrev [11] 0x5e3:0xf DW_TAG_formal_parameter
	.long	.Linfo_string28         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	6                       # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.long	.Ldebug_loc138          # DW_AT_location
	.byte	7                       # Abbrev [7] 0x5f2:0x33 DW_TAG_inlined_subroutine
	.long	1711                    # DW_AT_abstract_origin
	.quad	.Ltmp191                # DW_AT_low_pc
	.quad	.Ltmp194                # DW_AT_high_pc
	.byte	1                       # DW_AT_call_file
	.byte	7                       # DW_AT_call_line
	.byte	16                      # Abbrev [16] 0x609:0xd DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	60                      # DW_AT_const_value
	.byte	18                      # Abbrev [18] 0x616:0xe DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	1                       # DW_AT_location
	.byte	82
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	0                       # End Of Children Mark
	.byte	24                      # Abbrev [24] 0x627:0x3e DW_TAG_subprogram
	.long	.Linfo_string22         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	328                     # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	344                     # DW_AT_type
	.byte	1                       # DW_AT_inline
	.byte	25                      # Abbrev [25] 0x634:0xc DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	328                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	25                      # Abbrev [25] 0x640:0xc DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	328                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	25                      # Abbrev [25] 0x64c:0xc DW_TAG_formal_parameter
	.long	.Linfo_string32         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	328                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	26                      # Abbrev [26] 0x658:0xc DW_TAG_variable
	.long	.Linfo_string27         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	330                     # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.byte	0                       # End Of Children Mark
	.byte	24                      # Abbrev [24] 0x665:0x4a DW_TAG_subprogram
	.long	.Linfo_string23         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	344                     # DW_AT_type
	.byte	1                       # DW_AT_inline
	.byte	25                      # Abbrev [25] 0x672:0xc DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	25                      # Abbrev [25] 0x67e:0xc DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	25                      # Abbrev [25] 0x68a:0xc DW_TAG_formal_parameter
	.long	.Linfo_string32         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	25                      # Abbrev [25] 0x696:0xc DW_TAG_formal_parameter
	.long	.Linfo_string33         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	336                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	26                      # Abbrev [26] 0x6a2:0xc DW_TAG_variable
	.long	.Linfo_string27         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	338                     # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.byte	0                       # End Of Children Mark
	.byte	24                      # Abbrev [24] 0x6af:0x32 DW_TAG_subprogram
	.long	.Linfo_string24         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	344                     # DW_AT_type
	.byte	1                       # DW_AT_inline
	.byte	25                      # Abbrev [25] 0x6bc:0xc DW_TAG_formal_parameter
	.long	.Linfo_string25         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	25                      # Abbrev [25] 0x6c8:0xc DW_TAG_formal_parameter
	.long	.Linfo_string26         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	321                     # DW_AT_decl_line
	.long	344                     # DW_AT_type
	.byte	26                      # Abbrev [26] 0x6d4:0xc DW_TAG_variable
	.long	.Linfo_string27         # DW_AT_name
	.byte	2                       # DW_AT_decl_file
	.short	323                     # DW_AT_decl_line
	.long	238                     # DW_AT_type
	.byte	0                       # End Of Children Mark
	.byte	27                      # Abbrev [27] 0x6e1:0x5 DW_TAG_const_type
	.long	38                      # DW_AT_type
	.byte	3                       # Abbrev [3] 0x6e6:0x5 DW_TAG_pointer_type
	.long	1761                    # DW_AT_type
	.byte	3                       # Abbrev [3] 0x6eb:0x5 DW_TAG_pointer_type
	.long	238                     # DW_AT_type
	.byte	28                      # Abbrev [28] 0x6f0:0x7 DW_TAG_base_type
	.long	.Linfo_string35         # DW_AT_name
	.byte	4                       # DW_AT_byte_size
	.byte	5                       # DW_AT_encoding
	.byte	29                      # Abbrev [29] 0x6f7:0xc DW_TAG_array_type
	.long	752                     # DW_AT_type
	.byte	30                      # Abbrev [30] 0x6fc:0x6 DW_TAG_subrange_type
	.long	1776                    # DW_AT_type
	.byte	0                       # DW_AT_upper_bound
	.byte	0                       # End Of Children Mark
	.byte	2                       # Abbrev [2] 0x703:0x7 DW_TAG_base_type
	.long	.Linfo_string37         # DW_AT_name
	.byte	7                       # DW_AT_encoding
	.byte	8                       # DW_AT_byte_size
	.byte	3                       # Abbrev [3] 0x70a:0x5 DW_TAG_pointer_type
	.long	1279                    # DW_AT_type
	.byte	13                      # Abbrev [13] 0x70f:0xb DW_TAG_typedef
	.long	344                     # DW_AT_type
	.long	.Linfo_string43         # DW_AT_name
	.byte	3                       # DW_AT_decl_file
	.byte	6                       # DW_AT_decl_line
	.byte	31                      # Abbrev [31] 0x71a:0x27 DW_TAG_structure_type
	.long	.Linfo_string45         # DW_AT_name
	.byte	16                      # DW_AT_byte_size
	.byte	3                       # DW_AT_decl_file
	.byte	7                       # DW_AT_decl_line
	.byte	32                      # Abbrev [32] 0x722:0xf DW_TAG_member
	.long	.Linfo_string42         # DW_AT_name
	.long	1807                    # DW_AT_type
	.byte	3                       # DW_AT_decl_file
	.byte	9                       # DW_AT_decl_line
	.byte	2                       # DW_AT_data_member_location
	.byte	35
	.byte	0
	.byte	1                       # DW_AT_accessibility
                                        # DW_ACCESS_public
	.byte	32                      # Abbrev [32] 0x731:0xf DW_TAG_member
	.long	.Linfo_string44         # DW_AT_name
	.long	344                     # DW_AT_type
	.byte	3                       # DW_AT_decl_file
	.byte	10                      # DW_AT_decl_line
	.byte	2                       # DW_AT_data_member_location
	.byte	35
	.byte	8
	.byte	1                       # DW_AT_accessibility
                                        # DW_ACCESS_public
	.byte	0                       # End Of Children Mark
	.byte	3                       # Abbrev [3] 0x741:0x5 DW_TAG_pointer_type
	.long	45                      # DW_AT_type
	.byte	33                      # Abbrev [33] 0x746:0x15 DW_TAG_subroutine_type
	.long	752                     # DW_AT_type
                                        # DW_AT_prototyped
	.byte	34                      # Abbrev [34] 0x74b:0x5 DW_TAG_formal_parameter
	.long	752                     # DW_AT_type
	.byte	34                      # Abbrev [34] 0x750:0x5 DW_TAG_formal_parameter
	.long	1857                    # DW_AT_type
	.byte	34                      # Abbrev [34] 0x755:0x5 DW_TAG_formal_parameter
	.long	1857                    # DW_AT_type
	.byte	0                       # End Of Children Mark
	.byte	3                       # Abbrev [3] 0x75b:0x5 DW_TAG_pointer_type
	.long	1862                    # DW_AT_type
	.byte	35                      # Abbrev [35] 0x760:0x1 DW_TAG_subroutine_type
                                        # DW_AT_prototyped
	.byte	3                       # Abbrev [3] 0x761:0x5 DW_TAG_pointer_type
	.long	1888                    # DW_AT_type
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
	.byte	23                      # DW_FORM_sec_offset
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
	.byte	10                      # DW_FORM_block1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	7                       # Abbreviation Code
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
	.byte	8                       # Abbreviation Code
	.byte	5                       # DW_TAG_formal_parameter
	.byte	0                       # DW_CHILDREN_no
	.byte	49                      # DW_AT_abstract_origin
	.byte	19                      # DW_FORM_ref4
	.byte	28                      # DW_AT_const_value
	.byte	13                      # DW_FORM_sdata
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	9                       # Abbreviation Code
	.byte	5                       # DW_TAG_formal_parameter
	.byte	0                       # DW_CHILDREN_no
	.byte	49                      # DW_AT_abstract_origin
	.byte	19                      # DW_FORM_ref4
	.byte	2                       # DW_AT_location
	.byte	10                      # DW_FORM_block1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	10                      # Abbreviation Code
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
	.byte	11                      # Abbreviation Code
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
	.byte	10                      # DW_FORM_block1
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
	.byte	13                      # DW_FORM_sdata
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
	.byte	10                      # DW_FORM_block1
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
	.byte	10                      # DW_FORM_block1
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
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	23                      # Abbreviation Code
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
	.byte	24                      # Abbreviation Code
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
	.byte	25                      # Abbreviation Code
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
	.byte	26                      # Abbreviation Code
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
	.byte	27                      # Abbreviation Code
	.byte	38                      # DW_TAG_const_type
	.byte	0                       # DW_CHILDREN_no
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	28                      # Abbreviation Code
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
	.byte	29                      # Abbreviation Code
	.byte	1                       # DW_TAG_array_type
	.byte	1                       # DW_CHILDREN_yes
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	30                      # Abbreviation Code
	.byte	33                      # DW_TAG_subrange_type
	.byte	0                       # DW_CHILDREN_no
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	47                      # DW_AT_upper_bound
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	31                      # Abbreviation Code
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
	.byte	32                      # Abbreviation Code
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
	.byte	33                      # Abbreviation Code
	.byte	21                      # DW_TAG_subroutine_type
	.byte	1                       # DW_CHILDREN_yes
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	39                      # DW_AT_prototyped
	.byte	25                      # DW_FORM_flag_present
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	34                      # Abbreviation Code
	.byte	5                       # DW_TAG_formal_parameter
	.byte	0                       # DW_CHILDREN_no
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	35                      # Abbreviation Code
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
	.quad	.Lfunc_begin1
	.quad	.Ltmp7
.Lset0 = .Ltmp198-.Ltmp197              # Loc expr size
	.short	.Lset0
.Ltmp197:
	.byte	85                      # DW_OP_reg5
.Ltmp198:
	.quad	0
	.quad	0
.Ldebug_loc2:
	.quad	.Lfunc_begin1
	.quad	.Ltmp5
.Lset1 = .Ltmp200-.Ltmp199              # Loc expr size
	.short	.Lset1
.Ltmp199:
	.byte	85                      # DW_OP_reg5
.Ltmp200:
	.quad	0
	.quad	0
.Ldebug_loc4:
	.quad	.Ltmp4
	.quad	.Ltmp15
.Lset2 = .Ltmp202-.Ltmp201              # Loc expr size
	.short	.Lset2
.Ltmp201:
	.byte	82                      # DW_OP_reg2
.Ltmp202:
	.quad	.Ltmp16
	.quad	.Ltmp17
.Lset3 = .Ltmp204-.Ltmp203              # Loc expr size
	.short	.Lset3
.Ltmp203:
	.byte	82                      # DW_OP_reg2
.Ltmp204:
	.quad	0
	.quad	0
.Ldebug_loc7:
	.quad	.Ltmp6
	.quad	.Ltmp15
.Lset4 = .Ltmp206-.Ltmp205              # Loc expr size
	.short	.Lset4
.Ltmp205:
	.byte	80                      # DW_OP_reg0
.Ltmp206:
	.quad	.Ltmp17
	.quad	.Ltmp17
.Lset5 = .Ltmp208-.Ltmp207              # Loc expr size
	.short	.Lset5
.Ltmp207:
	.byte	80                      # DW_OP_reg0
.Ltmp208:
	.quad	0
	.quad	0
.Ldebug_loc10:
	.quad	.Lfunc_begin2
	.quad	.Ltmp47
.Lset6 = .Ltmp210-.Ltmp209              # Loc expr size
	.short	.Lset6
.Ltmp209:
	.byte	85                      # DW_OP_reg5
.Ltmp210:
	.quad	.Ltmp47
	.quad	.Ltmp51
.Lset7 = .Ltmp212-.Ltmp211              # Loc expr size
	.short	.Lset7
.Ltmp211:
	.byte	83                      # DW_OP_reg3
.Ltmp212:
	.quad	0
	.quad	0
.Ldebug_loc13:
	.quad	.Lfunc_begin3
	.quad	.Ltmp55
.Lset8 = .Ltmp214-.Ltmp213              # Loc expr size
	.short	.Lset8
.Ltmp213:
	.byte	85                      # DW_OP_reg5
.Ltmp214:
	.quad	0
	.quad	0
.Ldebug_loc15:
	.quad	.Lfunc_begin4
	.quad	.Ltmp60
.Lset9 = .Ltmp216-.Ltmp215              # Loc expr size
	.short	.Lset9
.Ltmp215:
	.byte	85                      # DW_OP_reg5
.Ltmp216:
	.quad	.Ltmp60
	.quad	.Ltmp61
.Lset10 = .Ltmp218-.Ltmp217             # Loc expr size
	.short	.Lset10
.Ltmp217:
	.byte	81                      # DW_OP_reg1
.Ltmp218:
	.quad	.Ltmp62
	.quad	.Lfunc_end4
.Lset11 = .Ltmp220-.Ltmp219             # Loc expr size
	.short	.Lset11
.Ltmp219:
	.byte	81                      # DW_OP_reg1
.Ltmp220:
	.quad	0
	.quad	0
.Ldebug_loc19:
	.quad	.Lfunc_begin5
	.quad	.Ltmp69
.Lset12 = .Ltmp222-.Ltmp221             # Loc expr size
	.short	.Lset12
.Ltmp221:
	.byte	85                      # DW_OP_reg5
.Ltmp222:
	.quad	0
	.quad	0
.Ldebug_loc22:
	.quad	.Lfunc_begin6
	.quad	.Ltmp72
.Lset13 = .Ltmp224-.Ltmp223             # Loc expr size
	.short	.Lset13
.Ltmp223:
	.byte	85                      # DW_OP_reg5
.Ltmp224:
	.quad	0
	.quad	0
.Ldebug_loc24:
	.quad	.Lfunc_begin7
	.quad	.Ltmp78
.Lset14 = .Ltmp226-.Ltmp225             # Loc expr size
	.short	.Lset14
.Ltmp225:
	.byte	85                      # DW_OP_reg5
.Ltmp226:
	.quad	0
	.quad	0
.Ldebug_loc26:
	.quad	.Lfunc_begin7
	.quad	.Ltmp77
.Lset15 = .Ltmp228-.Ltmp227             # Loc expr size
	.short	.Lset15
.Ltmp227:
	.byte	85                      # DW_OP_reg5
.Ltmp228:
	.quad	0
	.quad	0
.Ldebug_loc28:
	.quad	.Ltmp75
	.quad	.Ltmp76
.Lset16 = .Ltmp230-.Ltmp229             # Loc expr size
	.short	.Lset16
.Ltmp229:
	.byte	80                      # DW_OP_reg0
.Ltmp230:
	.quad	0
	.quad	0
.Ldebug_loc30:
	.quad	.Ltmp76
	.quad	.Ltmp80
.Lset17 = .Ltmp232-.Ltmp231             # Loc expr size
	.short	.Lset17
.Ltmp231:
	.byte	80                      # DW_OP_reg0
.Ltmp232:
	.quad	.Ltmp81
	.quad	.Ltmp82
.Lset18 = .Ltmp234-.Ltmp233             # Loc expr size
	.short	.Lset18
.Ltmp233:
	.byte	80                      # DW_OP_reg0
.Ltmp234:
	.quad	.Ltmp83
	.quad	.Ltmp84
.Lset19 = .Ltmp236-.Ltmp235             # Loc expr size
	.short	.Lset19
.Ltmp235:
	.byte	80                      # DW_OP_reg0
.Ltmp236:
	.quad	.Ltmp85
	.quad	.Ltmp86
.Lset20 = .Ltmp238-.Ltmp237             # Loc expr size
	.short	.Lset20
.Ltmp237:
	.byte	80                      # DW_OP_reg0
.Ltmp238:
	.quad	.Ltmp87
	.quad	.Ltmp88
.Lset21 = .Ltmp240-.Ltmp239             # Loc expr size
	.short	.Lset21
.Ltmp239:
	.byte	80                      # DW_OP_reg0
.Ltmp240:
	.quad	.Ltmp89
	.quad	.Ltmp90
.Lset22 = .Ltmp242-.Ltmp241             # Loc expr size
	.short	.Lset22
.Ltmp241:
	.byte	80                      # DW_OP_reg0
.Ltmp242:
	.quad	.Ltmp91
	.quad	.Ltmp92
.Lset23 = .Ltmp244-.Ltmp243             # Loc expr size
	.short	.Lset23
.Ltmp243:
	.byte	80                      # DW_OP_reg0
.Ltmp244:
	.quad	0
	.quad	0
.Ldebug_loc39:
	.quad	.Lfunc_begin8
	.quad	.Ltmp95
.Lset24 = .Ltmp246-.Ltmp245             # Loc expr size
	.short	.Lset24
.Ltmp245:
	.byte	85                      # DW_OP_reg5
.Ltmp246:
	.quad	.Ltmp96
	.quad	.Ltmp96
.Lset25 = .Ltmp248-.Ltmp247             # Loc expr size
	.short	.Lset25
.Ltmp247:
	.byte	85                      # DW_OP_reg5
.Ltmp248:
	.quad	.Ltmp131
	.quad	.Ltmp134
.Lset26 = .Ltmp250-.Ltmp249             # Loc expr size
	.short	.Lset26
.Ltmp249:
	.byte	85                      # DW_OP_reg5
.Ltmp250:
	.quad	0
	.quad	0
.Ldebug_loc43:
	.quad	.Lfunc_begin8
	.quad	.Ltmp134
.Lset27 = .Ltmp252-.Ltmp251             # Loc expr size
	.short	.Lset27
.Ltmp251:
	.byte	16                      # DW_OP_constu
	.byte	0
.Ltmp252:
	.quad	.Ltmp134
	.quad	.Ltmp134
.Lset28 = .Ltmp254-.Ltmp253             # Loc expr size
	.short	.Lset28
.Ltmp253:
	.byte	81                      # DW_OP_reg1
.Ltmp254:
	.quad	.Ltmp134
	.quad	.Ltmp170
.Lset29 = .Ltmp256-.Ltmp255             # Loc expr size
	.short	.Lset29
.Ltmp255:
	.byte	16                      # DW_OP_constu
	.byte	0
.Ltmp256:
	.quad	.Ltmp170
	.quad	.Ltmp171
.Lset30 = .Ltmp258-.Ltmp257             # Loc expr size
	.short	.Lset30
.Ltmp257:
	.byte	80                      # DW_OP_reg0
.Ltmp258:
	.quad	0
	.quad	0
.Ldebug_loc48:
	.quad	.Lfunc_begin8
	.quad	.Lfunc_end8
.Lset31 = .Ltmp260-.Ltmp259             # Loc expr size
	.short	.Lset31
.Ltmp259:
	.byte	16                      # DW_OP_constu
	.byte	0
.Ltmp260:
	.quad	0
	.quad	0
.Ldebug_loc53:
	.quad	.Ltmp96
	.quad	.Ltmp99
.Lset32 = .Ltmp262-.Ltmp261             # Loc expr size
	.short	.Lset32
.Ltmp261:
	.byte	85                      # DW_OP_reg5
.Ltmp262:
	.quad	.Ltmp112
	.quad	.Ltmp114
.Lset33 = .Ltmp264-.Ltmp263             # Loc expr size
	.short	.Lset33
.Ltmp263:
	.byte	85                      # DW_OP_reg5
.Ltmp264:
	.quad	0
	.quad	0
.Ldebug_loc56:
	.quad	.Ltmp96
	.quad	.Ltmp99
.Lset34 = .Ltmp266-.Ltmp265             # Loc expr size
	.short	.Lset34
.Ltmp265:
	.byte	85                      # DW_OP_reg5
.Ltmp266:
	.quad	0
	.quad	0
.Ldebug_loc58:
	.quad	.Ltmp96
	.quad	.Ltmp99
.Lset35 = .Ltmp268-.Ltmp267             # Loc expr size
	.short	.Lset35
.Ltmp267:
	.byte	85                      # DW_OP_reg5
.Ltmp268:
	.quad	.Ltmp112
	.quad	.Ltmp114
.Lset36 = .Ltmp270-.Ltmp269             # Loc expr size
	.short	.Lset36
.Ltmp269:
	.byte	85                      # DW_OP_reg5
.Ltmp270:
	.quad	0
	.quad	0
.Ldebug_loc61:
	.quad	.Ltmp97
	.quad	.Ltmp99
.Lset37 = .Ltmp272-.Ltmp271             # Loc expr size
	.short	.Lset37
.Ltmp271:
	.byte	80                      # DW_OP_reg0
.Ltmp272:
	.quad	0
	.quad	0
.Ldebug_loc63:
	.quad	.Ltmp97
	.quad	.Ltmp98
.Lset38 = .Ltmp274-.Ltmp273             # Loc expr size
	.short	.Lset38
.Ltmp273:
	.byte	80                      # DW_OP_reg0
.Ltmp274:
	.quad	0
	.quad	0
.Ldebug_loc65:
	.quad	.Ltmp98
	.quad	.Ltmp99
.Lset39 = .Ltmp276-.Ltmp275             # Loc expr size
	.short	.Lset39
.Ltmp275:
	.byte	80                      # DW_OP_reg0
.Ltmp276:
	.quad	.Ltmp100
	.quad	.Ltmp101
.Lset40 = .Ltmp278-.Ltmp277             # Loc expr size
	.short	.Lset40
.Ltmp277:
	.byte	84                      # DW_OP_reg4
.Ltmp278:
	.quad	.Ltmp102
	.quad	.Ltmp103
.Lset41 = .Ltmp280-.Ltmp279             # Loc expr size
	.short	.Lset41
.Ltmp279:
	.byte	84                      # DW_OP_reg4
.Ltmp280:
	.quad	.Ltmp104
	.quad	.Ltmp105
.Lset42 = .Ltmp282-.Ltmp281             # Loc expr size
	.short	.Lset42
.Ltmp281:
	.byte	84                      # DW_OP_reg4
.Ltmp282:
	.quad	.Ltmp106
	.quad	.Ltmp107
.Lset43 = .Ltmp284-.Ltmp283             # Loc expr size
	.short	.Lset43
.Ltmp283:
	.byte	84                      # DW_OP_reg4
.Ltmp284:
	.quad	.Ltmp108
	.quad	.Ltmp109
.Lset44 = .Ltmp286-.Ltmp285             # Loc expr size
	.short	.Lset44
.Ltmp285:
	.byte	84                      # DW_OP_reg4
.Ltmp286:
	.quad	.Ltmp110
	.quad	.Ltmp111
.Lset45 = .Ltmp288-.Ltmp287             # Loc expr size
	.short	.Lset45
.Ltmp287:
	.byte	84                      # DW_OP_reg4
.Ltmp288:
	.quad	.Ltmp112
	.quad	.Ltmp112
.Lset46 = .Ltmp290-.Ltmp289             # Loc expr size
	.short	.Lset46
.Ltmp289:
	.byte	84                      # DW_OP_reg4
.Ltmp290:
	.quad	.Ltmp112
	.quad	.Ltmp114
.Lset47 = .Ltmp292-.Ltmp291             # Loc expr size
	.short	.Lset47
.Ltmp291:
	.byte	80                      # DW_OP_reg0
.Ltmp292:
	.quad	0
	.quad	0
.Ldebug_loc75:
	.quad	.Ltmp98
	.quad	.Ltmp99
.Lset48 = .Ltmp294-.Ltmp293             # Loc expr size
	.short	.Lset48
.Ltmp293:
	.byte	80                      # DW_OP_reg0
.Ltmp294:
	.quad	.Ltmp116
	.quad	.Ltmp117
.Lset49 = .Ltmp296-.Ltmp295             # Loc expr size
	.short	.Lset49
.Ltmp295:
	.byte	80                      # DW_OP_reg0
.Ltmp296:
	.quad	.Ltmp118
	.quad	.Ltmp119
.Lset50 = .Ltmp298-.Ltmp297             # Loc expr size
	.short	.Lset50
.Ltmp297:
	.byte	80                      # DW_OP_reg0
.Ltmp298:
	.quad	.Ltmp120
	.quad	.Ltmp121
.Lset51 = .Ltmp300-.Ltmp299             # Loc expr size
	.short	.Lset51
.Ltmp299:
	.byte	80                      # DW_OP_reg0
.Ltmp300:
	.quad	.Ltmp122
	.quad	.Ltmp123
.Lset52 = .Ltmp302-.Ltmp301             # Loc expr size
	.short	.Lset52
.Ltmp301:
	.byte	80                      # DW_OP_reg0
.Ltmp302:
	.quad	.Ltmp124
	.quad	.Ltmp125
.Lset53 = .Ltmp304-.Ltmp303             # Loc expr size
	.short	.Lset53
.Ltmp303:
	.byte	80                      # DW_OP_reg0
.Ltmp304:
	.quad	.Ltmp126
	.quad	.Ltmp127
.Lset54 = .Ltmp306-.Ltmp305             # Loc expr size
	.short	.Lset54
.Ltmp305:
	.byte	80                      # DW_OP_reg0
.Ltmp306:
	.quad	.Ltmp128
	.quad	.Ltmp128
.Lset55 = .Ltmp308-.Ltmp307             # Loc expr size
	.short	.Lset55
.Ltmp307:
	.byte	80                      # DW_OP_reg0
.Ltmp308:
	.quad	0
	.quad	0
.Ldebug_loc84:
	.quad	.Ltmp113
	.quad	.Ltmp114
.Lset56 = .Ltmp310-.Ltmp309             # Loc expr size
	.short	.Lset56
.Ltmp309:
	.byte	84                      # DW_OP_reg4
.Ltmp310:
	.quad	0
	.quad	0
.Ldebug_loc86:
	.quad	.Ltmp134
	.quad	.Ltmp138
.Lset57 = .Ltmp312-.Ltmp311             # Loc expr size
	.short	.Lset57
.Ltmp311:
	.byte	85                      # DW_OP_reg5
.Ltmp312:
	.quad	.Ltmp151
	.quad	.Ltmp155
.Lset58 = .Ltmp314-.Ltmp313             # Loc expr size
	.short	.Lset58
.Ltmp313:
	.byte	85                      # DW_OP_reg5
.Ltmp314:
	.quad	0
	.quad	0
.Ldebug_loc89:
	.quad	.Ltmp134
	.quad	.Ltmp138
.Lset59 = .Ltmp316-.Ltmp315             # Loc expr size
	.short	.Lset59
.Ltmp315:
	.byte	85                      # DW_OP_reg5
.Ltmp316:
	.quad	0
	.quad	0
.Ldebug_loc91:
	.quad	.Ltmp134
	.quad	.Ltmp138
.Lset60 = .Ltmp318-.Ltmp317             # Loc expr size
	.short	.Lset60
.Ltmp317:
	.byte	85                      # DW_OP_reg5
.Ltmp318:
	.quad	.Ltmp151
	.quad	.Ltmp155
.Lset61 = .Ltmp320-.Ltmp319             # Loc expr size
	.short	.Lset61
.Ltmp319:
	.byte	85                      # DW_OP_reg5
.Ltmp320:
	.quad	0
	.quad	0
.Ldebug_loc94:
	.quad	.Ltmp135
	.quad	.Ltmp138
.Lset62 = .Ltmp322-.Ltmp321             # Loc expr size
	.short	.Lset62
.Ltmp321:
	.byte	80                      # DW_OP_reg0
.Ltmp322:
	.quad	0
	.quad	0
.Ldebug_loc96:
	.quad	.Ltmp135
	.quad	.Ltmp137
.Lset63 = .Ltmp324-.Ltmp323             # Loc expr size
	.short	.Lset63
.Ltmp323:
	.byte	80                      # DW_OP_reg0
.Ltmp324:
	.quad	0
	.quad	0
.Ldebug_loc98:
	.quad	.Ltmp136
	.quad	.Ltmp138
.Lset64 = .Ltmp326-.Ltmp325             # Loc expr size
	.short	.Lset64
.Ltmp325:
	.byte	82                      # DW_OP_reg2
.Ltmp326:
	.quad	.Ltmp139
	.quad	.Ltmp140
.Lset65 = .Ltmp328-.Ltmp327             # Loc expr size
	.short	.Lset65
.Ltmp327:
	.byte	80                      # DW_OP_reg0
.Ltmp328:
	.quad	.Ltmp141
	.quad	.Ltmp142
.Lset66 = .Ltmp330-.Ltmp329             # Loc expr size
	.short	.Lset66
.Ltmp329:
	.byte	80                      # DW_OP_reg0
.Ltmp330:
	.quad	.Ltmp143
	.quad	.Ltmp144
.Lset67 = .Ltmp332-.Ltmp331             # Loc expr size
	.short	.Lset67
.Ltmp331:
	.byte	80                      # DW_OP_reg0
.Ltmp332:
	.quad	.Ltmp145
	.quad	.Ltmp146
.Lset68 = .Ltmp334-.Ltmp333             # Loc expr size
	.short	.Lset68
.Ltmp333:
	.byte	80                      # DW_OP_reg0
.Ltmp334:
	.quad	.Ltmp147
	.quad	.Ltmp148
.Lset69 = .Ltmp336-.Ltmp335             # Loc expr size
	.short	.Lset69
.Ltmp335:
	.byte	80                      # DW_OP_reg0
.Ltmp336:
	.quad	.Ltmp149
	.quad	.Ltmp150
.Lset70 = .Ltmp338-.Ltmp337             # Loc expr size
	.short	.Lset70
.Ltmp337:
	.byte	80                      # DW_OP_reg0
.Ltmp338:
	.quad	.Ltmp151
	.quad	.Ltmp151
.Lset71 = .Ltmp340-.Ltmp339             # Loc expr size
	.short	.Lset71
.Ltmp339:
	.byte	80                      # DW_OP_reg0
.Ltmp340:
	.quad	.Ltmp151
	.quad	.Ltmp155
.Lset72 = .Ltmp342-.Ltmp341             # Loc expr size
	.short	.Lset72
.Ltmp341:
	.byte	82                      # DW_OP_reg2
.Ltmp342:
	.quad	0
	.quad	0
.Ldebug_loc108:
	.quad	.Ltmp136
	.quad	.Ltmp138
.Lset73 = .Ltmp344-.Ltmp343             # Loc expr size
	.short	.Lset73
.Ltmp343:
	.byte	82                      # DW_OP_reg2
.Ltmp344:
	.quad	.Ltmp156
	.quad	.Ltmp157
.Lset74 = .Ltmp346-.Ltmp345             # Loc expr size
	.short	.Lset74
.Ltmp345:
	.byte	82                      # DW_OP_reg2
.Ltmp346:
	.quad	.Ltmp158
	.quad	.Ltmp159
.Lset75 = .Ltmp348-.Ltmp347             # Loc expr size
	.short	.Lset75
.Ltmp347:
	.byte	82                      # DW_OP_reg2
.Ltmp348:
	.quad	.Ltmp160
	.quad	.Ltmp161
.Lset76 = .Ltmp350-.Ltmp349             # Loc expr size
	.short	.Lset76
.Ltmp349:
	.byte	82                      # DW_OP_reg2
.Ltmp350:
	.quad	.Ltmp162
	.quad	.Ltmp163
.Lset77 = .Ltmp352-.Ltmp351             # Loc expr size
	.short	.Lset77
.Ltmp351:
	.byte	82                      # DW_OP_reg2
.Ltmp352:
	.quad	.Ltmp164
	.quad	.Ltmp165
.Lset78 = .Ltmp354-.Ltmp353             # Loc expr size
	.short	.Lset78
.Ltmp353:
	.byte	82                      # DW_OP_reg2
.Ltmp354:
	.quad	.Ltmp166
	.quad	.Ltmp167
.Lset79 = .Ltmp356-.Ltmp355             # Loc expr size
	.short	.Lset79
.Ltmp355:
	.byte	82                      # DW_OP_reg2
.Ltmp356:
	.quad	.Ltmp168
	.quad	.Ltmp169
.Lset80 = .Ltmp358-.Ltmp357             # Loc expr size
	.short	.Lset80
.Ltmp357:
	.byte	82                      # DW_OP_reg2
.Ltmp358:
	.quad	0
	.quad	0
.Ldebug_loc117:
	.quad	.Ltmp152
	.quad	.Ltmp153
.Lset81 = .Ltmp360-.Ltmp359             # Loc expr size
	.short	.Lset81
.Ltmp359:
	.byte	80                      # DW_OP_reg0
.Ltmp360:
	.quad	0
	.quad	0
.Ldebug_loc119:
	.quad	.Lfunc_begin9
	.quad	.Ltmp174
.Lset82 = .Ltmp362-.Ltmp361             # Loc expr size
	.short	.Lset82
.Ltmp361:
	.byte	85                      # DW_OP_reg5
.Ltmp362:
	.quad	.Ltmp174
	.quad	.Ltmp179
.Lset83 = .Ltmp364-.Ltmp363             # Loc expr size
	.short	.Lset83
.Ltmp363:
	.byte	81                      # DW_OP_reg1
.Ltmp364:
	.quad	0
	.quad	0
.Ldebug_loc123:
	.quad	.Lfunc_begin10
	.quad	.Ltmp184
.Lset84 = .Ltmp366-.Ltmp365             # Loc expr size
	.short	.Lset84
.Ltmp365:
	.byte	85                      # DW_OP_reg5
.Ltmp366:
	.quad	.Ltmp184
	.quad	.Lfunc_end10
.Lset85 = .Ltmp368-.Ltmp367             # Loc expr size
	.short	.Lset85
.Ltmp367:
	.byte	88                      # DW_OP_reg8
.Ltmp368:
	.quad	0
	.quad	0
.Ldebug_loc126:
	.quad	.Lfunc_begin10
	.quad	.Ltmp187
.Lset86 = .Ltmp370-.Ltmp369             # Loc expr size
	.short	.Lset86
.Ltmp369:
	.byte	84                      # DW_OP_reg4
.Ltmp370:
	.quad	.Ltmp187
	.quad	.Ltmp192
.Lset87 = .Ltmp372-.Ltmp371             # Loc expr size
	.short	.Lset87
.Ltmp371:
	.byte	85                      # DW_OP_reg5
.Ltmp372:
	.quad	0
	.quad	0
.Ldebug_loc129:
	.quad	.Lfunc_begin10
	.quad	.Ltmp188
.Lset88 = .Ltmp374-.Ltmp373             # Loc expr size
	.short	.Lset88
.Ltmp373:
	.byte	81                      # DW_OP_reg1
.Ltmp374:
	.quad	.Ltmp188
	.quad	.Lfunc_end10
.Lset89 = .Ltmp376-.Ltmp375             # Loc expr size
	.short	.Lset89
.Ltmp375:
	.byte	84                      # DW_OP_reg4
.Ltmp376:
	.quad	0
	.quad	0
.Ldebug_loc132:
	.quad	.Lfunc_begin10
	.quad	.Ltmp186
.Lset90 = .Ltmp378-.Ltmp377             # Loc expr size
	.short	.Lset90
.Ltmp377:
	.byte	82                      # DW_OP_reg2
.Ltmp378:
	.quad	0
	.quad	0
.Ldebug_loc134:
	.quad	.Lfunc_begin10
	.quad	.Ltmp184
.Lset91 = .Ltmp380-.Ltmp379             # Loc expr size
	.short	.Lset91
.Ltmp379:
	.byte	88                      # DW_OP_reg8
.Ltmp380:
	.quad	0
	.quad	0
.Ldebug_loc136:
	.quad	.Ltmp186
	.quad	.Ltmp189
.Lset92 = .Ltmp382-.Ltmp381             # Loc expr size
	.short	.Lset92
.Ltmp381:
	.byte	80                      # DW_OP_reg0
.Ltmp382:
	.quad	0
	.quad	0
.Ldebug_loc138:
	.quad	.Ltmp190
	.quad	.Ltmp193
.Lset93 = .Ltmp384-.Ltmp383             # Loc expr size
	.short	.Lset93
.Ltmp383:
	.byte	82                      # DW_OP_reg2
.Ltmp384:
	.quad	0
	.quad	0
.Ldebug_loc140:
	.section	.debug_aranges,"",@progbits
	.section	.debug_ranges,"",@progbits
	.quad	.Ltmp132
	.quad	.Ltmp133
	.quad	.Ltmp155
	.quad	.Ltmp169
	.quad	0
	.quad	0
	.section	.debug_macinfo,"",@progbits
	.section	.debug_pubnames,"",@progbits
.Lset94 = .Lpubnames_end0-.Lpubnames_begin0 # Length of Public Names Info
	.long	.Lset94
.Lpubnames_begin0:
	.short	2                       # DWARF Version
	.long	.L.debug_info_begin0    # Offset of Compilation Unit Info
.Lset95 = .L.debug_info_end0-.L.debug_info_begin0 # Compilation Unit Length
	.long	.Lset95
	.long	50                      # DIE offset
	.asciz	 "current_brk"          # External Name
	.long	571                     # DIE offset
	.asciz	 "isspace"              # External Name
	.long	1290                    # DIE offset
	.asciz	 "__libc_start_main"    # External Name
	.long	1711                    # DIE offset
	.asciz	 "__syscall1"           # External Name
	.long	1575                    # DIE offset
	.asciz	 "__syscall2"           # External Name
	.long	256                     # DIE offset
	.asciz	 "puts"                 # External Name
	.long	616                     # DIE offset
	.asciz	 "isdigit"              # External Name
	.long	1169                    # DIE offset
	.asciz	 "time"                 # External Name
	.long	148                     # DIE offset
	.asciz	 "llstrlen"             # External Name
	.long	763                     # DIE offset
	.asciz	 "atol"                 # External Name
	.long	71                      # DIE offset
	.asciz	 "llexit"               # External Name
	.long	661                     # DIE offset
	.asciz	 "LLreadchar"           # External Name
	.long	1637                    # DIE offset
	.asciz	 "__syscall3"           # External Name
	.long	351                     # DIE offset
	.asciz	 "putchar"              # External Name
	.long	474                     # DIE offset
	.asciz	 "malloc"               # External Name
	.long	0                       # End Mark
.Lpubnames_end0:
	.section	.debug_pubtypes,"",@progbits
.Lset96 = .Lpubtypes_end0-.Lpubtypes_begin0 # Length of Public Types Info
	.long	.Lset96
.Lpubtypes_begin0:
	.short	2                       # DWARF Version
	.long	.L.debug_info_begin0    # Offset of Compilation Unit Info
.Lset97 = .L.debug_info_end0-.L.debug_info_begin0 # Compilation Unit Length
	.long	.Lset97
	.long	1818                    # DIE offset
	.asciz	 "timespec"             # External Name
	.long	0                       # End Mark
.Lpubtypes_end0:
	.section	.debug_str,"MS",@progbits,1
.Linfo_string0:
	.asciz	 "clang version 3.4 (trunk 193450) (llvm/trunk 193903)"
.Linfo_string1:
	.asciz	 "libc.c"
.Linfo_string2:
	.asciz	 "/home/aakshintala/llvm/testcase/prog3"
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
