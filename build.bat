@ECHO OFF
::******************************************************
:: File: build.bat
::
:: Description: This build script is used to generate
::      project configuration, and execute a build, 
::      This can be used for command line builds for  
::      developers or for continuous integration builds.
::      
::******************************************************
:: Copyright 2020 - Third Pole Therapeutics
::******************************************************
REM Default Software Path Locations
set PYTHON_LOC=C:\ProgramData\Anaconda3
set RUBY_LOC=C:\Ruby25-x64
REM ********** CodeSonar Parameters **********
set CS_TOOL="C:\Program Files\GrammaTech\CodeSonar\codesonar\bin\codesonar.exe"
REM CodeSonar Credentials
set CS_CREDS=-auth password -hubuser devuser -hubpwfile pwfile
set CS_HUB=10.200.0.42:7340
REM Default Parameters - Generate Configuration, Build mcb1 normal
set CLEAN=0
set CLEANREPO=0
set MAKEPROJECT=1
set BUILD=1
set UNITTEST=0
set RANDOMUT=0
set COVERAGE=0
set COMPLEXITY=0
set PCLINT=0
set CODESONAR=0
set TARGET=normal
ECHO *** Dealing with Arguments: %* ***
set argCount=0
:ARGUMENTLOOP
if not "%1"=="" (
    set /A argCount+=1
    if /I "%1"=="clean" (
        set CLEAN=1
    )
    if /I "%1"=="cleanrepo" (
        set CLEANREPO=1
    )
    if /I "%1"=="all" (
        set MAKEPROJECT=1
        set BUILD=1
        set UNITTEST=1
        set COVERAGE=1
        set COMPLEXITY=1
        set PCLINT=1
    )
    if /I "%1"=="config" (
        set MAKEPROJECT=1
        set BUILD=0
    )
    if /I "%1"=="build" (
        set MAKEPROJECT=0
        set BUILD=1
    )
    if /I "%1"=="codesonar" (
        set MAKEPROJECT=1
        set BUILD=0
        set CODESONAR=1
    )
    if /I "%1"=="unittest" (
        set MAKEPROJECT=1
        set BUILD=0
        set UNITTEST=1
    )
    if /I "%1"=="utonly" (
        set MAKEPROJECT=0
        set BUILD=0
        set UNITTEST=1
    )
    if /I "%1"=="randomut" (
        set SYNERGY=1
        set BUILD=0
        set RANDOMUT=1
    )
    if /I "%1"=="coverage" (
        set COVERAGE=1
    )
    if /I "%1"=="pmccabe" (
        set MAKEPROJECT=0
        set BUILD=0
        set UNITTEST=0
        set COMPLEXITY=1
    )
    if /I "%1"=="pclint" (
        set MAKEPROJECT=0
        set BUILD=0
        set UNITTEST=0
        set COMPLEXITY=0
        set PCLINT=1
    )
    if /I "%1"=="target" (
        set TARGET=%2
        shift
    )
    if /I "%1"=="options" (
        goto :PRINTOPTIONS
    )
    if /I "%1"=="help" (
        goto :PRINTARGSHELP	
    )
    shift
    GOTO :ARGUMENTLOOP
)
:DONEARGS
ECHO *** Handled %argCount% Arguments ***
ECHO *** Build Arguments ***
ECHO    TARGET: %TARGET%

ECHO *** Setting Up Build Workspace ***
set CURRENTPATH=%CD%
IF NOT DEFINED WORKSPACE GOTO SetupLocalBuild
ECHO --- WORKSPACE DEFINED - JENKINS BUILD. ---
set JENKINS=1
set REPOLOC=%WORKSPACE%
IF EXIST ..\spm set REPOLOC=%WORKSPACE%\spm
set CS_CREDS=-auth password -hubuser builduser -hubpwfile C:\codesonarpw\pwfile
set CS_PROJECT=spm_codesonar
IF NOT DEFINED BUILD_NUMBER GOTO EnvironmentError
IF NOT DEFINED SYSTEM_TOOLS_REPO GOTO EnvironmentError
IF NOT DEFINED SPM_HASH set SPM_HASH=0
GOTO BuildSetupComplete

:SetupLocalBuild
ECHO --- WORKSPACE NOT DEFINED - LOCAL BUILD. ---
set JENKINS=0
set REPOLOC=%CD%
copy spm_codesonar.conf "%USERNAME%_spm_codesonar.conf"
set CS_PROJECT="%USERNAME%_spm_codesonar"

:BuildSetupComplete
cd %REPOLOC%
ECHO *** GitHub Repository Location: %REPOLOC% ***

:Clean
IF %CLEAN% EQU 1 (
    ECHO *** Executing Clean Functionality ***
    ECHO *** CLEANING UNSTAGED FILES PRESS Ctrl-C TO EXIT ***
    TIMEOUT /T 10 /NOBREAK
    IF ERRORLEVEL 0 GOTO DoClean
    GOTO Done
:DoClean
    git clean -df
    GOTO Done
)

:CleanRepo
IF %CLEANREPO% EQU 1 (
    ECHO *** Executing Clean Repo Functionality ***
    ECHO *** CLEANING UNSTAGED AND IGNORED FILES PRESS Ctrl-C TO EXIT ***
    TIMEOUT /T 10 /NOBREAK
    IF ERRORLEVEL 0 GOTO DoCleanRepo
    GOTO Done
:DoCleanRepo
    git clean -fdx
    GOTO Done
)

IF %MAKEPROJECT% EQU 1 (
    ECHO *** Generating Project Configuration ***
    call "C:\Program Files (x86)\Microchip\MPLABX\v5.35\mplab_platform\bin\prjMakefilesGenerator.bat" "%REPOLOC%"
    if %ERRORLEVEL% NEQ 0 GOTO ReportMakeProjError
)

IF %BUILD% EQU 1 (
    set ProjectName=SPM
    IF %JENKINS% EQU 1 (
	%PYTHON_LOC%\python.exe %SYSTEM_TOOLS_REPO%\generate_include.py version.h SPM_HASH
        if %ERRORLEVEL% NEQ 0 GOTO ReportGenVerError
    )
    ECHO *** Building SPM with target %TARGET% ***
    "C:\Program Files (x86)\Microchip\MPLABX\v5.35\gnuBins\GnuWin32\bin\make.exe" -f nbproject/Makefile-%TARGET%.mk SUBPROJECTS= .build-conf
    if %ERRORLEVEL% NEQ 0 GOTO ReportMPLError

    IF %JENKINS% EQU 1 (
        REM create SHA-256 hash for output file and store in same location as output file.
        %PYTHON_LOC%\python.exe %SYSTEM_TOOLS_REPO%\generate_sha2.py dist\%TARGET%\production\SPM_crc.hex
        if %ERRORLEVEL% NEQ 0 GOTO ReportGenCRCError
        REM rename output files to be unique to hardware
        REM set TEMPDIR=%CD%
        REM CD dist\%TARGET%\production
        REM for %%f in (SPM_*.*) do (
        REM     echo Renaming Output File: %%f to %TARGET%_%%f
        REM     copy "%%f" "%TARGET%_%%f"
        REM )
        REM CD %TEMPDIR%
    )
)

IF %CODESONAR% EQU 1 (
    ECHO *** Running CodeSonar Analysis***
    CD %REPOLOC%
    ECHO *** build source files with codesonar analysis. ***
    %CS_TOOL% analyze -foreground -clean %CS_PROJECT% %CS_CREDS% %CS_HUB% "C:\Program Files (x86)\Microchip\MPLABX\v5.35\gnuBins\GnuWin32\bin\make.exe" -f nbproject/Makefile-%TARGET%.mk SUBPROJECTS= .build-conf
    if %ERRORLEVEL% NEQ 0 GOTO ReportCSError
)

IF %UNITTEST% EQU 1 (
    ECHO *** Executing UnitTests ***
    CD %REPOLOC%\UnityUnitTests
    %RUBY_LOC%\bin\ceedling.bat
    if %ERRORLEVEL% NEQ 0 GOTO ReportUTError
    if %COVERAGE% EQU 1 (
        ECHO *** Executing UnitTest HTML Coverage Report ***
        %RUBY_LOC%\bin\ceedling.bat gcov:all utils:gcov
        ECHO *** Executing UnitTest Cobertura Coverage Report ***
        %PYTHON_LOC%\python.exe %PYTHON_LOC%\Scripts\gcovr -p -b -e "\||^build.*|.*vendor.*|.*test.*|.*lib.*" --xml -r . -o "build/artifacts/gcov/coverage.xml"
        if %ERRORLEVEL% NEQ 0 GOTO ReportUTError
    )
    CD %REPOLOC%
)

IF %RANDOMUT% EQU 1 (
    ECHO *** Executing Random UnitTests ***
    CD %REPOLOC%\UnityUnitTests
    if %COVERAGE% EQU 1 (
        ECHO *** with coverage ***
        %PYTHON_LOC%\python.exe %SYSTEM_TOOLS_REPO%\ceedling_random_test_cases.py -p utest_ -c
        %PYTHON_LOC%\python.exe %PYTHON_LOC%\Scripts\gcovr -p -b -e "\||^vendor.*|^build.*|^test.|^lib." --xml -r . -o "build/artifacts/gcov/coverage.xml"
    ) else (
        ECHO *** without coverage ***
        %PYTHON_LOC%\python.exe %SYSTEM_TOOLS_REPO%\ceedling_random_test_cases.py -p utest_
    )
    if %ERRORLEVEL% NEQ 0 GOTO ReportRandUTError
    CD %REPOLOC%
)


IF %COMPLEXITY% EQU 1 (
    ECHO *** Executing pmccabe Complexity Algorithm ***
    CD %REPOLOC%
    %PYTHON_LOC%\python.exe %SYSTEM_TOOLS_REPO%\pmccabe.py
    if %ERRORLEVEL% NEQ 0 GOTO ReportMccabeError
)
IF %PCLINT% EQU 1 (
    ECHO *** Executing PC-Lint Static Analysis ***
    CD %REPOLOC%\PCLintUserOptions
    pc-lint-spm-prj.bat %REPOLOC% > pc-lint-ouput.log
    if %ERRORLEVEL% NEQ 0 GOTO ReportPcLintError
    CD %REPOLOC%
)
GOTO Done

:EnvironmentError
ECHO !!! Build Environment Error !!!
EXIT /B 1

:ReportMakeProjError
set EXITCODE=%ERRORLEVEL%
ECHO !!! Failed to Gererate Project Configuration. Err:%EXITCODE% !!!
EXIT /B %EXITCODE%

:ReportGenVerError
set EXITCODE=%ERRORLEVEL%
ECHO !!! Failed to Generate Version Err:%EXITCODE% !!!
exit /B %EXITCODE%

:ReportMPLError
set EXITCODE=%ERRORLEVEL%
ECHO !!! Failed to Build SPM Software. Err:%EXITCODE% !!!
EXIT /B %EXITCODE%

:ReportCSError
set EXITCODE=%ERRORLEVEL%
ECHO !!! Failed to Codesonar SPM Software. Err:%EXITCODE% !!!
EXIT /B %EXITCODE%

:ReportGenCRCError
set EXITCODE=%ERRORLEVEL%
ECHO !!! Error Generating CRC32. Err:%EXITCODE% !!!
exit /B %EXITCODE%

:ReportUTError
set EXITCODE=%ERRORLEVEL%
ECHO !!! Error Executing Unit Tests. Err:%EXITCODE% !!!
exit /B %EXITCODE%

:ReportRandUTError
set EXITCODE=%ERRORLEVEL%
ECHO !!! Error Executing Random Unit Tests. Err:%EXITCODE% !!!
ECHO !!! Random Unit Tests not deleted in workspace. !!!
exit /B %EXITCODE%

:ReportMccabeError
set EXITCODE=%ERRORLEVEL%
ECHO !!! Error Executing Unit Tests. Err:%EXITCODE% !!!
exit /B %EXITCODE%

:ReportPcLintError
set EXITCODE=%ERRORLEVEL%
ECHO !!! Error Executing PC-Lint Analysis. Err:%EXITCODE% !!!
exit /B %EXITCODE%

:PRINTARGSHELP
ECHO build.bat
ECHO     This script peforms build commands for this repository and branch.
ECHO     Default Options - generate configuration and build
ECHO     Parameters:
ECHO         all - generate project configuration, build, and run unit tests.
ECHO         clean - clean any artifacts from previous runs.
ECHO         config - generate project configuration.
ECHO         build - generate project configuration and build.
ECHO         unittest - generate project configuration and execute unit tests.
ECHO         buildonly - Build SPM.
ECHO         utonly - execute unit tests.
ECHO         randomut - execute random unit tests.
ECHO         coverage - add gcov report to unit test ouput.
ECHO         pmccabe - execute complexity script.
ECHO         pclint - execute pc-lint static analysis script.
ECHO         target - target selection parameter.  defaults to MCB4.
ECHO         options - gets all the options
GOTO Done

:PRINTOPTIONS
ECHO option:target:normal
exit

:Done
ECHO *** Done Executing build.bat ***
