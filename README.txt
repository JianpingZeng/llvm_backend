LLVM Backend for a Reduced X86-64 Instruction Set (Cse523 Backend)
==================================================================

Build Instructions:

1. Clone the entire source code into your local directory:
    git clone git@github.com:sahilpparmar/llvm_backend.git
    
2. Create a built folder inside the llvm_backend folder:
    cd llvm_backend
    mkdir built
    cd built

3. Configure the build for our Cse523 backend:
    ../configure --enable-targets=cse523
    (Make sure you dont have existing clang binaries in your PATH)
    
4. Build both frontend (clang) and backend (llc) compiler sources:
    make [-j]
    
5. Add the generated compiler-tools bin folder to the PATH
    export $PATH=<path_to_llvm_backend>/built/Debug+Asserts/bit

--------------------------------------------------------------------
    
Using CLANG and LLC:

Refer to sample tests (prog1/2/3/4/5) at location:
    <path_to_llvm_backend>/mytest
    
For running the tests:
    cd <testname>
    sh build.sh
    ./<testname>

For more details, refer the example below:
http://llvm.org/docs/GettingStarted.html#an-example-using-the-llvm-tool-chain

