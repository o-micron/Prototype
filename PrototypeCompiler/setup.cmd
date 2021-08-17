git clone --depth 1 --single-branch -b release/12.x https://github.com/llvm/llvm-project.git
cd llvm-project
mkdir build
cd build
cmake -DLLVM_ENABLE_PROJECTS=clang -Thost=x64 ../llvm
cmake --build . --config Release