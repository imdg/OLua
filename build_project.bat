cmake .
cmake --build . --config Release
xcopy .\api .\bin\Release /y /q
