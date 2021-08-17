@echo off
if exist %CD%\cpdb\compile_commands.json (
    if exist %CD%\build\x64-%1\bin\PrototypeGenerator.exe (
        echo %CD%\build\x64-%1\bin\PrototypeGenerator.exe
        %CD%\build\x64-%1\bin\PrototypeGenerator.exe
    )
)