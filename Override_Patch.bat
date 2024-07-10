@echo off
setlocal

REM Define source and destination directories
set "source_dir=Edk2_Patch"
set "destination_dir=edk2"

REM Check if source directory exists
if not exist "%source_dir%" (
    echo Source directory "%source_dir%" does not exist.
    exit /b 1
)

REM Create destination directory if it does not exist
if not exist "%destination_dir%" (
    mkdir "%destination_dir%"
    echo Destination directory "%destination_dir%" created.
)

REM Copy files and directories from source to destination
xcopy "%source_dir%\*" "%destination_dir%\" /s /e /y /i

REM Check the exit code of xcopy to determine if the operation was successful
if %errorlevel% equ 0 (
    echo Patch integration was completed successfully from "%source_dir%" to "%destination_dir%".
) else (
    echo An error occurred while copying files.
)

endlocal
exit /b 0