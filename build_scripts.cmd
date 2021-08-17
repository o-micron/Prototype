rd /s /q "build\x64-Release\PrototypePlugins"
cmake -S . --preset=x64-Release
cmake --build build/x64-Release --target PrototypePlugins --config Release