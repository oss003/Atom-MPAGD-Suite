@echo off
echo Creating wav file ...
call .\tape\atm2wav %1 8bit

echo Open DOSBOX en type DOSTAPE %1

rem echo Creating csw file ...
rem call .\tape\csw .\atomulator\mmc\%1.wav ".\atomulator\mmc\%1.csw"
rem rename %1.csw %2.csw
rem del .\atomulator\mmc\%1.wav
echo Ready