c:\masm32\bin\ml /c /Zd /coff source.asm
c:\masm32\bin\Link /SUBSYSTEM:CONSOLE source.obj
echo RESULT BEGIN...
source.exe
echo ...RESULT END
PAUSE
