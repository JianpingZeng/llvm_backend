; RUN: llc -mcpu=core2 -mtriple=i686-pc-win32 -O0 < %s | FileCheck --check-prefix=X86 %s
; RUN: llc -mcpu=core2 -mtriple=i686-pc-win32 -o - -O0 < %s | llvm-mc -triple=i686-pc-win32 -filetype=obj | llvm-readobj -s -sr -codeview-linetables | FileCheck --check-prefix=OBJ32 %s
; RUN: llc -mcpu=core2 -mtriple=x86_64-pc-win32 -O0 < %s | FileCheck --check-prefix=X64 %s
; RUN: llc -mcpu=core2 -mtriple=x86_64-pc-win32 -o - -O0 < %s | llvm-mc -triple=x86_64-pc-win32 -filetype=obj | llvm-readobj -s -sr -codeview-linetables | FileCheck --check-prefix=OBJ64 %s

; This LL file was generated by running clang on the following code:
; D:\asm.c:
;  1 void g(void);
;  2
;  3 void f(void) {
;  4   __asm align 4;
;  5   g();
;  6 }

; X86: _f:
; X86-NEXT: # BB
; X86-NEXT: [[ASM_LINE:^L.*]]:{{$}}
; X86:      [[CALL_LINE:^L.*]]:{{$}}
; X86-NEXT: calll   _g
; X86-NEXT: [[RETURN_STMT:.*]]:
; X86-NEXT: ret
; X86-NEXT: [[END_OF_F:.*]]:
;
; X86: .section        .debug$S,"rn"
; X86-NEXT: .long   4
; X86-NEXT: .long   242
; X86-NEXT: .long [[F2_END:.*]]-[[F2_START:.*]]
; X86-NEXT: [[F2_START]]:
; X86-NEXT: .secrel32 _f
; X86-NEXT: .secidx _f
; X86-NEXT: .long [[END_OF_F]]-_f
; X86-NEXT: [[FILE_SEGMENT_START:[^:]*]]:
; X86-NEXT: .long   0
; X86-NEXT: .long   3
; X86-NEXT: .long [[FILE_SEGMENT_END:.*]]-[[FILE_SEGMENT_START]]
; X86-NEXT: .long [[ASM_LINE]]-_f
; X86-NEXT: .long   4
; X86-NEXT: .long [[CALL_LINE]]-_f
; X86-NEXT: .long   5
; X86-NEXT: .long [[RETURN_STMT]]-_f
; X86-NEXT: .long   6
; X86-NEXT: [[FILE_SEGMENT_END]]:
; X86-NEXT: [[F2_END]]:
; File index to string table offset subsection
; X86-NEXT: .long   244
; X86-NEXT: .long   8
; X86-NEXT: .long   1
; X86-NEXT: .long   0
; String table
; X86-NEXT: .long   243
; X86-NEXT: .long   10
; X86-NEXT: .byte   0
; X86-NEXT: .ascii  "D:\\asm.c"
; X86-NEXT: .byte   0
; Padding
; X86-NEXT: .zero   2

; OBJ32:    Section {
; OBJ32:      Name: .debug$S (2E 64 65 62 75 67 24 53)
; OBJ32:      Characteristics [ (0x42100040)
; OBJ32:      ]
; OBJ32:      Relocations [
; OBJ32-NEXT:   0xC IMAGE_REL_I386_SECREL _f
; OBJ32-NEXT:   0x10 IMAGE_REL_I386_SECTION _f
; OBJ32-NEXT: ]
; OBJ32:      FunctionLineTable [
; OBJ32-NEXT:   Name: _f
; OBJ32-NEXT:   CodeSize: 0x6
; OBJ32-NEXT:   FilenameSegment [
; OBJ32-NEXT:     Filename: D:\asm.c
; FIXME: An empty __asm stmt creates an extra entry.
; We seem to know that these offsets are the same statically during the
; execution of endModule().  See PR18679 for the details.
; OBJ32-NEXT:     +0x0: 4
; OBJ32-NEXT:     +0x0: 5
; OBJ32-NEXT:     +0x5: 6
; OBJ32-NEXT:   ]
; OBJ32-NEXT: ]
; OBJ32:    }

; X64: f:
; X64-NEXT: [[START:.*]]:{{$}}
; X64-NEXT: # BB
; X64-NEXT: subq    $40, %rsp
; X64-NEXT: [[ASM_LINE:.*]]:{{$}}
; X64:      [[CALL_LINE:.*]]:{{$}}
; X64-NEXT: callq   g
; X64-NEXT: [[EPILOG_AND_RET:.*]]:
; X64-NEXT: addq    $40, %rsp
; X64-NEXT: ret
; X64-NEXT: [[END_OF_F:.*]]:
;
; X64: .section        .debug$S,"rn"
; X64-NEXT: .long   4
; X64-NEXT: .long   242
; X64-NEXT: .long [[F2_END:.*]]-[[F2_START:.*]]
; X64-NEXT: [[F2_START]]:
; X64-NEXT: .secrel32 f
; X64-NEXT: .secidx f
; X64-NEXT: .long [[END_OF_F]]-f
; X64-NEXT: [[FILE_SEGMENT_START:[^:]*]]:
; X64-NEXT: .long   0
; X64-NEXT: .long   4
; X64-NEXT: .long [[FILE_SEGMENT_END:.*]]-[[FILE_SEGMENT_START]]
; X64-NEXT: .long [[START]]-f
; X64-NEXT: .long   3
; X64-NEXT: .long [[ASM_LINE]]-f
; X64-NEXT: .long   4
; X64-NEXT: .long [[CALL_LINE]]-f
; X64-NEXT: .long   5
; X64-NEXT: .long [[EPILOG_AND_RET]]-f
; X64-NEXT: .long   6
; X64-NEXT: [[FILE_SEGMENT_END]]:
; X64-NEXT: [[F2_END]]:
; File index to string table offset subsection
; X64-NEXT: .long   244
; X64-NEXT: .long   8
; X64-NEXT: .long   1
; X64-NEXT: .long   0
; String table
; X64-NEXT: .long   243
; X64-NEXT: .long   10
; X64-NEXT: .byte   0
; X64-NEXT: .ascii  "D:\\asm.c"
; X64-NEXT: .byte   0
; Padding
; X64-NEXT: .zero   2

; OBJ64:    Section {
; OBJ64:      Name: .debug$S (2E 64 65 62 75 67 24 53)
; OBJ64:      Characteristics [ (0x42100040)
; OBJ64:      ]
; OBJ64:      Relocations [
; OBJ64-NEXT:   0xC IMAGE_REL_AMD64_SECREL f
; OBJ64-NEXT:   0x10 IMAGE_REL_AMD64_SECTION f
; OBJ64-NEXT: ]
; OBJ64:      FunctionLineTable [
; OBJ64-NEXT:   Name: f
; OBJ64-NEXT:   CodeSize: 0xE
; OBJ64-NEXT:   FilenameSegment [
; OBJ64-NEXT:     Filename: D:\asm.c
; OBJ64-NEXT:     +0x0: 3
; FIXME: An empty __asm stmt creates an extra entry.
; See PR18679 for the details.
; OBJ64-NEXT:     +0x4: 4
; OBJ64-NEXT:     +0x4: 5
; OBJ64-NEXT:     +0x9: 6
; OBJ64-NEXT:   ]
; OBJ64-NEXT: ]
; OBJ64:    }

; Function Attrs: nounwind
define void @f() #0 {
entry:
  call void asm sideeffect inteldialect ".align 4", "~{dirflag},~{fpsr},~{flags}"() #2, !dbg !12
  call void @g(), !dbg !13
  ret void, !dbg !14
}

declare void @g() #1

attributes #0 = { nounwind "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="false" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "no-realign-stack" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #2 = { nounwind }

!llvm.dbg.cu = !{!0}
!llvm.module.flags = !{!9, !10}
!llvm.ident = !{!11}

!0 = metadata !{i32 786449, metadata !1, i32 12, metadata !"clang version 3.5 ", i1 false, metadata !"", i32 0, metadata !2, metadata !2, metadata !3, metadata !2, metadata !2, metadata !""} ; [ DW_TAG_compile_unit ] [D:\/<unknown>] [DW_LANG_C99]
!1 = metadata !{metadata !"<unknown>", metadata !"D:\5C"}
!2 = metadata !{i32 0}
!3 = metadata !{metadata !4}
!4 = metadata !{i32 786478, metadata !5, metadata !6, metadata !"f", metadata !"f", metadata !"", i32 3, metadata !7, i1 false, i1 true, i32 0, i32 0, null, i32 256, i1 false, void ()* @f, null, null, metadata !2, i32 3} ; [ DW_TAG_subprogram ] [line 3] [def] [f]
!5 = metadata !{metadata !"asm.c", metadata !"D:\5C"}
!6 = metadata !{i32 786473, metadata !5}          ; [ DW_TAG_file_type ] [D:\/asm.c]
!7 = metadata !{i32 786453, i32 0, null, metadata !"", i32 0, i64 0, i64 0, i64 0, i32 0, null, metadata !8, i32 0, null, null, null} ; [ DW_TAG_subroutine_type ] [line 0, size 0, align 0, offset 0] [from ]
!8 = metadata !{null}
!9 = metadata !{i32 2, metadata !"Dwarf Version", i32 4}
!10 = metadata !{i32 1, metadata !"Debug Info Version", i32 1}
!11 = metadata !{metadata !"clang version 3.5 "}
!12 = metadata !{i32 4, i32 0, metadata !4, null}
!13 = metadata !{i32 5, i32 0, metadata !4, null}
!14 = metadata !{i32 6, i32 0, metadata !4, null}
