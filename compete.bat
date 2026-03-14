@echo off
setlocal enabledelayedexpansion

set "max_filename="

rem 遍历所有的DLL文件
for %%F in (.\dll\*) do (
    set "filename=%%F"

    rem 如果是第一个文件或者当前的日期和时间大于最大的日期和时间，则更新最大值
    if not defined max_filename (
        set "max_filename=!filename!"
    ) else (
        if !filename! gtr !max_filename! (
            set "max_filename=!filename!"
        )
    )
)

if not defined max_filename (
    echo 没有找到DLL文件
    exit /b 1
)

rem 运行最新的DLL文件
for /r ".\TestCases\" %%j in (*) do (
    echo Compete !max_filename:~-30,-4! %%~nj
    compete "!max_filename!" "%%~j" ".\CompeteResult\!max_filename:~-30,-4!-%%~nj.txt" 1
)

pause
