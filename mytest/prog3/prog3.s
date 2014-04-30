	.file	"prog3.bc"
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
	.file	1 "prog3.c"
	.text
	.globl	_main
	.align	16, 0x90
	.type	_main,@function
_main:                                  # @_main
	.cfi_startproc
.Lfunc_begin0:
	.loc	1 88 0                  # prog3.c:88:0
# BB#0:                                 # %entry
	.loc	1 91 0 prologue_end     # prog3.c:91:0
	pushq	%rax
.Ltmp1:
	.cfi_def_cfa_offset 16
	#DEBUG_VALUE: _main:argc <- RDI
	#DEBUG_VALUE: _main:argv <- RSI
	movq	powers(%rip), %rax
.Ltmp2:
	#DEBUG_VALUE: _main:sum <- RDI
	movq	$-99, %rcx
	cmpq	$99, %rax
	leaq	1(%rax), %rdi
.Ltmp3:
	.loc	1 92 0                  # prog3.c:92:0
	cmovgq	%rcx, %rdi
.Ltmp4:
	.loc	1 94 0                  # prog3.c:94:0
	movq	%rdi, pi+8(%rip)
	.loc	1 96 0                  # prog3.c:96:0
	addq	$48, %rdi
	callq	putchar
	xorl	%eax, %eax
	.loc	1 97 0                  # prog3.c:97:0
	popq	%rdx
	ret
.Ltmp5:
.Ltmp6:
	.size	_main, .Ltmp6-_main
.Lfunc_end0:
	.cfi_endproc

	.type	powers,@object          # @powers
	.comm	powers,800,16
	.type	pi,@object              # @pi
	.comm	pi,800,16
.Ltext_end:
	.data
.Ldata_end:
	.text
.Lsection_end1:
	.section	.debug_info,"",@progbits
.L.debug_info_begin0:
	.long	213                     # Length of Compilation Unit Info
	.short	4                       # DWARF version number
	.long	.L.debug_abbrev_begin   # Offset Into Abbrev. Section
	.byte	8                       # Address Size (in bytes)
	.byte	1                       # Abbrev [1] 0xb:0xce DW_TAG_compile_unit
	.long	.Linfo_string0          # DW_AT_producer
	.short	12                      # DW_AT_language
	.long	.Linfo_string1          # DW_AT_name
	.quad	0                       # DW_AT_low_pc
	.long	.Lsection_line          # DW_AT_stmt_list
	.long	.Linfo_string2          # DW_AT_comp_dir
                                        # DW_AT_APPLE_optimized
	.byte	2                       # Abbrev [2] 0x26:0x7 DW_TAG_base_type
	.long	.Linfo_string4          # DW_AT_name
	.byte	5                       # DW_AT_encoding
	.byte	8                       # DW_AT_byte_size
	.byte	3                       # Abbrev [3] 0x2d:0xb DW_TAG_typedef
	.long	38                      # DW_AT_type
	.long	.Linfo_string5          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	78                      # DW_AT_decl_line
	.byte	4                       # Abbrev [4] 0x38:0x7 DW_TAG_base_type
	.long	.Linfo_string6          # DW_AT_name
	.byte	4                       # DW_AT_byte_size
	.byte	5                       # DW_AT_encoding
	.byte	5                       # Abbrev [5] 0x3f:0xc DW_TAG_array_type
	.long	45                      # DW_AT_type
	.byte	6                       # Abbrev [6] 0x44:0x6 DW_TAG_subrange_type
	.long	56                      # DW_AT_type
	.byte	99                      # DW_AT_upper_bound
	.byte	0                       # End Of Children Mark
	.byte	7                       # Abbrev [7] 0x4b:0x15 DW_TAG_variable
	.long	.Linfo_string3          # DW_AT_name
	.long	63                      # DW_AT_type
                                        # DW_AT_external
	.byte	1                       # DW_AT_decl_file
	.byte	85                      # DW_AT_decl_line
	.byte	9                       # DW_AT_location
	.byte	3
	.quad	pi
	.byte	7                       # Abbrev [7] 0x60:0x15 DW_TAG_variable
	.long	.Linfo_string7          # DW_AT_name
	.long	63                      # DW_AT_type
                                        # DW_AT_external
	.byte	1                       # DW_AT_decl_file
	.byte	85                      # DW_AT_decl_line
	.byte	9                       # DW_AT_location
	.byte	3
	.quad	powers
	.byte	8                       # Abbrev [8] 0x75:0x47 DW_TAG_subprogram
	.long	.Linfo_string8          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	87                      # DW_AT_decl_line
                                        # DW_AT_prototyped
	.long	38                      # DW_AT_type
                                        # DW_AT_external
	.quad	.Lfunc_begin0           # DW_AT_low_pc
	.quad	.Lfunc_end0             # DW_AT_high_pc
	.byte	1                       # DW_AT_frame_base
	.byte	87
                                        # DW_AT_APPLE_omit_frame_ptr
	.byte	9                       # Abbrev [9] 0x92:0xd DW_TAG_formal_parameter
	.long	.Linfo_string9          # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	87                      # DW_AT_decl_line
	.long	38                      # DW_AT_type
	.byte	1                       # DW_AT_location
	.byte	85
	.byte	9                       # Abbrev [9] 0x9f:0xd DW_TAG_formal_parameter
	.long	.Linfo_string10         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	87                      # DW_AT_decl_line
	.long	200                     # DW_AT_type
	.byte	1                       # DW_AT_location
	.byte	84
	.byte	10                      # Abbrev [10] 0xac:0xf DW_TAG_variable
	.long	.Linfo_string12         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	89                      # DW_AT_decl_line
	.long	205                     # DW_AT_type
	.long	.Ldebug_loc0            # DW_AT_location
	.byte	0                       # End Of Children Mark
	.byte	2                       # Abbrev [2] 0xbc:0x7 DW_TAG_base_type
	.long	.Linfo_string11         # DW_AT_name
	.byte	6                       # DW_AT_encoding
	.byte	1                       # DW_AT_byte_size
	.byte	11                      # Abbrev [11] 0xc3:0x5 DW_TAG_pointer_type
	.long	188                     # DW_AT_type
	.byte	11                      # Abbrev [11] 0xc8:0x5 DW_TAG_pointer_type
	.long	195                     # DW_AT_type
	.byte	3                       # Abbrev [3] 0xcd:0xb DW_TAG_typedef
	.long	38                      # DW_AT_type
	.long	.Linfo_string13         # DW_AT_name
	.byte	1                       # DW_AT_decl_file
	.byte	79                      # DW_AT_decl_line
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
	.byte	4                       # Abbreviation Code
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
	.byte	5                       # Abbreviation Code
	.byte	1                       # DW_TAG_array_type
	.byte	1                       # DW_CHILDREN_yes
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	6                       # Abbreviation Code
	.byte	33                      # DW_TAG_subrange_type
	.byte	0                       # DW_CHILDREN_no
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	47                      # DW_AT_upper_bound
	.byte	11                      # DW_FORM_data1
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	7                       # Abbreviation Code
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
	.byte	8                       # Abbreviation Code
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
	.byte	9                       # Abbreviation Code
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
	.byte	10                      # Abbreviation Code
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
	.byte	11                      # Abbreviation Code
	.byte	15                      # DW_TAG_pointer_type
	.byte	0                       # DW_CHILDREN_no
	.byte	73                      # DW_AT_type
	.byte	19                      # DW_FORM_ref4
	.byte	0                       # EOM(1)
	.byte	0                       # EOM(2)
	.byte	0                       # EOM(3)
.L.debug_abbrev_end:
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
	.quad	.Ltmp2
	.quad	.Ltmp3
.Lset0 = .Ltmp8-.Ltmp7                  # Loc expr size
	.short	.Lset0
.Ltmp7:
	.byte	85                      # DW_OP_reg5
.Ltmp8:
	.quad	0
	.quad	0
.Ldebug_loc2:
	.section	.debug_aranges,"",@progbits
	.section	.debug_ranges,"",@progbits
	.section	.debug_macinfo,"",@progbits
	.section	.debug_pubnames,"",@progbits
.Lset1 = .Lpubnames_end0-.Lpubnames_begin0 # Length of Public Names Info
	.long	.Lset1
.Lpubnames_begin0:
	.short	2                       # DWARF Version
	.long	.L.debug_info_begin0    # Offset of Compilation Unit Info
.Lset2 = .L.debug_info_end0-.L.debug_info_begin0 # Compilation Unit Length
	.long	.Lset2
	.long	96                      # DIE offset
	.asciz	 "powers"               # External Name
	.long	117                     # DIE offset
	.asciz	 "_main"                # External Name
	.long	75                      # DIE offset
	.asciz	 "pi"                   # External Name
	.long	0                       # End Mark
.Lpubnames_end0:
	.section	.debug_pubtypes,"",@progbits
.Lset3 = .Lpubtypes_end0-.Lpubtypes_begin0 # Length of Public Types Info
	.long	.Lset3
.Lpubtypes_begin0:
	.short	2                       # DWARF Version
	.long	.L.debug_info_begin0    # Offset of Compilation Unit Info
.Lset4 = .L.debug_info_end0-.L.debug_info_begin0 # Compilation Unit Length
	.long	.Lset4
	.long	0                       # End Mark
.Lpubtypes_end0:
	.section	.debug_str,"MS",@progbits,1
.Linfo_string0:
	.asciz	 "clang version 3.4 (trunk 193450) (llvm/trunk 193903)"
.Linfo_string1:
	.asciz	 "prog3.c"
.Linfo_string2:
	.asciz	 "/home/aakshintala/llvm/testcase/prog3"
.Linfo_string3:
	.asciz	 "pi"
.Linfo_string4:
	.asciz	 "long long int"
.Linfo_string5:
	.asciz	 "SHORT"
.Linfo_string6:
	.asciz	 "int"
.Linfo_string7:
	.asciz	 "powers"
.Linfo_string8:
	.asciz	 "_main"
.Linfo_string9:
	.asciz	 "argc"
.Linfo_string10:
	.asciz	 "argv"
.Linfo_string11:
	.asciz	 "char"
.Linfo_string12:
	.asciz	 "sum"
.Linfo_string13:
	.asciz	 "LONG"

	.section	".note.GNU-stack","",@progbits
