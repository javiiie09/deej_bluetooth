@echo off
taskkill /f /im "deej.exe"
sleep 10
start "" "%~dp0deej.exe"
