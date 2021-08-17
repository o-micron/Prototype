@echo off
if exist %CD%\cpdb\compile_commands.json (
    if exist %CD%\build\x64-%1\bin\PrototypeTranspiler.exe (
        copy %CD%\build\x64-%1\bin\PrototypeTranspiler.exe %CD%\PrototypeCompiler\llvm-project\build\%1\bin
        for %%f in (%CD%\PrototypeTraitSystem\include\PrototypeTraitSystem\*.h) do (
            %CD%\PrototypeCompiler\llvm-project\build\%1\bin\PrototypeTranspiler.exe -p=%CD%\cpdb\compile_commands.json %%f
        )
        for %%f in (%CD%\PrototypeTraitSystem\src\*.cpp) do (
            %CD%\PrototypeCompiler\llvm-project\build\%1\bin\PrototypeTranspiler.exe -p=%CD%\cpdb\compile_commands.json %%f
        )
    )
)