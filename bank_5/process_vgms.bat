ECHO OFF
:: process_vgms.bat
:: Import all vgm files from a specified <folder>. Then use Sverx' vgm2psg
:: converter on each file, to make it a psg file ready for psglib.
:: Clean up by deleting the vgm file before moving on to the next.
:: NOTE: vgm2psg must be in a subdirectory named "tools".

SET folder=C:\Users\ANSJ\Documents\SMS\DefleMask2016\songs\Output\SMS\astroswab

:: Copy from deflemask folder to this folder.
COPY %folder%\*.vgm

:: Process each file.
FOR /f %%f IN ('dir /b %folder%\*.vgm') DO CALL :PROCESS_FILE %%f

GOTO :EOF

:PROCESS_FILE
  :: Trim the .vgm file extension.
  SET mystring=%1
  SET mystring=%mystring:~0,-4%
  :: Convert vgm to psg.
  %~dp0\tools\vgm2psg %mystring%.vgm %mystring%.psg
  :: Clean up.
  DEL %mystring%.vgm
GOTO :eof

:EOF
