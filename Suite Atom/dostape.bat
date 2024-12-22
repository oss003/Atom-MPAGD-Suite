@echo off
cwsdpmi
echo Creating wav file ...
call .\tape\atm2wav .\atomul~1\mmc\%1 8bit
echo Creating csw file ...
call .\tape\csw .\atomul~1\mmc\%1.wav ".\atomul~1\mmc\%1.csw"
rem rename %1.csw %2.csw
del .\atomul~1\mmc\%1.wav
echo Ready