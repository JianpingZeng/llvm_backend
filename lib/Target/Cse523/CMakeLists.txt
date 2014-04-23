set(LLVM_TARGET_DEFINITIONS Cse523.td)

tablegen(LLVM Cse523GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM Cse523GenInstrInfo.inc -gen-instr-info)
tablegen(LLVM Cse523GenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM Cse523GenAsmWriter1.inc -gen-asm-writer -asmwriternum=1)
tablegen(LLVM Cse523GenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM Cse523GenDAGISel.inc -gen-dag-isel)
tablegen(LLVM Cse523GenFaeq_starttISel.inc -gen-fast-isel)
tablegen(LLVM Cse523GenCallingConv.inc -gen-callingconv)
tablegen(LLVM Cse523GenSubtargetInfo.inc -gen-subtarget)
add_public_tablegen_target(Cse523CommonTableGen)

set(sources
  Cse523AsmPrinter.cpp
  Cse523COFFMachineModuleInfo.cpp
  Cse523CodeEmitter.cpp
  Cse523FastISel.cpp
  Cse523FrameLowering.cpp
  Cse523ISelDAGToDAG.cpp
  Cse523ISelLowering.cpp
  Cse523InstrInfo.cpp
  Cse523MachineFunctionInfo.cpp
  Cse523MCInstLower.cpp
  Cse523RegisterInfo.cpp
  Cse523SelectionDAGInfo.cpp
  Cse523Subtarget.cpp
  Cse523TargetMachine.cpp
  Cse523TargetObjectFile.cpp
  Cse523TargetTransformInfo.cpp
  )

if( CMAKE_CL_64 )
  enable_language(ASM_MASM)
  ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Cse523CompilationCallback_Win64.obj
    MAIN_DEPENDENCY Cse523CompilationCallback_Win64.asm
    COMMAND ${CMAKE_ASM_MASM_COMPILER} /Fo ${CMAKE_CURRENT_BINARY_DIR}/Cse523CompilationCallback_Win64.obj /c ${CMAKE_CURRENT_SOURCE_DIR}/Cse523CompilationCallback_Win64.asm
   )
   set(sources ${sources} ${CMAKE_CURRENT_BINARY_DIR}/Cse523CompilationCallback_Win64.obj)
endif()

add_llvm_target(Cse523CodeGen ${sources})

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(InstPrinter)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
add_subdirectory(Utils)
