echo off
REM bmp_2_ribbon.bat
REM Use bmp2tile to convert a 256x144 bmp to assets for a Game Gear background,
REM optimized for use with gg_lib macro LOAD_RIBBON.
REM Collects these assets in [bmp filename]_assets.inc.
REM Last updated 2016-12-28

SET filename=%~n1

bmp2tile.exe %1 -savepalette %filename%Palette.inc -palgg -fullpalette -savetiles %filename%Tiles.inc -savetilemap %filename%Tilemap.inc -exit

REM Write palette
SET incfile=%filename%_ribbon.inc
ECHO %filename%Palette: > %incfile%
TYPE %filename%Palette.inc >> %incfile%

REM Write tilemap
ECHO.>> %incfile%
ECHO %filename%Tilemap: >> %incfile%
TYPE %filename%Tilemap.inc >> %incfile%

REM Write tiles
ECHO.>> %incfile%
ECHO %filename%Tiles: >> %incfile%
TYPE %filename%Tiles.inc >> %incfile%

REM Clean up mess
del %filename%Palette.inc
del %filename%Tiles.inc
del %filename%Tilemap.inc
