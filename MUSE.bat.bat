```bat
@echo off
title MUSE Spotify Controller

cd /d "%USERPROFILE%\Desktop"

start "" "spotify:"

timeout /t 3 /nobreak >nul

py -3.12 "C:\Users\kruna\Desktop\muse_spotifyv5.py"

pause
```