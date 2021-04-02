@echo off
set package_name=%1
set destination=%2
shift
shift

IF EXIST %destination% (
    rd /s /q %destination%
)
IF EXIST deploy_debug (
    rd /s /q deploy_debug
)
IF EXIST deploy_release (
    rd /s /q deploy_release
)

cpack -G TGZ -C Release
cmake -E tar xf %package_name%.tar.gz
ren %package_name%  deploy_release
cpack -G TGZ -C Debug
cmake -E tar xf %package_name%.tar.gz
ren %package_name%  deploy_debug

xcopy deploy_debug %destination% /e /d /I 
xcopy deploy_release %destination% /e /d /y /I

echo "created %destination%"

