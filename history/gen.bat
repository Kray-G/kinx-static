@echo off
if "%1"=="" exit /b 1

if not exist %1 (
    echo No history folder of %1.
    exit /b 1
)

REM Move to the folder.
cd %1

REM Generating a parser.
if exist kinxstatic.y (
    echo Generating a parser.
    ..\..\utility\kmyacc -v -d -b kxs kinxstatic.y
)

if exist Makefile.msc (
    echo make.
    nmake -f Makefile.msc
    if exist kiss.exe copy kiss.exe ..
)

REM Exit.
cd ..
exit /b 0
