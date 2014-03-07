set(LLVM_TARGET_DEFINITIONS Cse523.td)

tablegen(LLVM Cse523GenRegisterInfo.inc -gen-register-info)
tablegen(LLVM Cse523GenCallingConv.inc -gen-callingconv)

set(sources
  Cse523MachineFunctionInfo.cpp
  Cse523RegisterInfo.cpp
  Cse523Subtarget.cpp
  Cse523TargetMachine.cpp
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
