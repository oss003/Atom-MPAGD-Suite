@echo on
atm2uef %1.raw %1
gzip %1.raw
rename %1.raw.gz %1.uef

