@echo off
setlocal enabledelayedexpansion

rem 获取当前的年月日时间
for /f "tokens=1-4 delims=/ " %%a in ('date /t') do (
    set "year=%%a"
    set "month=%%b"
    set "day=%%c"
)
set "curr_time=%TIME%"
set "hour=!curr_time:~0,2!"
set "minute=!curr_time:~3,2!"
set "second=!curr_time:~6,2!"

rem 设置文件名
set "filename=2022011223_%year%%month%%day%_%hour%%minute%%second%.dll"

rem 编译和链接
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
cl .\SourceCode\Strategy\*.cpp /GS /GL /W3 /Gy /Zc:wchar_t /Zi /Gm- /O2 /Fd".\\TempResult\\vc142.pdb" /Zc:inline /fp:precise /D "_WINDLL" /D "_UNICODE" /D "UNICODE" /errorReport:prompt /WX- /Zc:forScope /Gd /Oi /MD /FC /Fa".\\TempResult\\" /EHsc /nologo /Fo".\\TempResult\\" /c /TP /Fp".\\TempResult\\Strategy.pch" /diagnostics:column
link .\TempResult\*.obj /OUT:".\\TempResult\%filename%" /DLL /MANIFEST /LTCG:incremental /NXCOMPAT /PDB:".\\TempResult\Strategy.pdb" /DYNAMICBASE /DEBUG /MACHINE:X64 /OPT:REF /SUBSYSTEM:CONSOLE /MANIFESTUAC:"level='asInvoker' uiAccess='false'" /ManifestFile:".\\TempResult\Strategy.dll.intermediate.manifest" /OPT:ICF /ERRORREPORT:PROMPT /NOLOGO /TLBID:1

rem 复制和重命名DLL
copy ".\TempResult\%filename%" ".\dll\"

rem 更新资源
mt /outputresource:".\\dll\%filename%;#2" /manifest ".\\TempResult\Strategy.dll.intermediate.manifest" /nologo

rem 清理临时文件
del /q ".\TempResult\*.*"
