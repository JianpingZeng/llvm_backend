set(LLVM_TARGET_DEFINITIONS Cse523.td)

add_llvm_target(Cse523CodeGen
  Cse523Subtarget.cpp
  Cse523TargetMachine.cpp
  )

add_subdirectory(TargetInfo)
add_subdirectory(MCTargetDesc)
add_subdirectory(InstPrinter)
add_subdirectory(AsmParser)
