@echo off

rem 运行 compile.bat
echo Running compile.bat:
call .\compile.bat
if %errorlevel% neq 0 (
    echo Running compile.bat failed.
    exit /b %errorlevel%
)

rem 运行 compete.bat
echo Running compete.bat:
call .\compete.bat
if %errorlevel% neq 0 (
    echo Running compete.bat failed.
    exit /b %errorlevel%
)

rem 运行 stat.py
echo Running stat.py:
python .\stat.py
if %errorlevel% neq 0 (
    echo Running stat.py failed.
    exit /b %errorlevel%
)

echo All scripts ran successfully.
exit /b 0