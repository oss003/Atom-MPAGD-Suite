copy ..\cc65\%1.atm AGDCOD1
copy ..\cc65\part2 AGDCOD2

dfsdisk create test.ssd -t 40 -q $20 -L AGDDISK
dfsdisk write test.ssd -f AGDCOD1 -q $20 -l $0300 -e $0300
dfsdisk write test.ssd -f AGDCOD2 -q $20 -l $0b00 -e $0300
