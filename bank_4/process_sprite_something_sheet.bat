echo off
:: process_sprite_something_sheet.bat.
:: Go to the Dropbox Sprite Something folder and get two spritesheets.
:: One sheet for 16x16 sprites, and one for 8x8. Merge them with each other,
:: convert them to pico palette and tiles.
:: NOTE: Depends on a special pico-8 colormap_cube.png and bmp2tile
:: being present in the tools folder.

SET folder="C:\Users\ANSJ\Dropbox\Apps\Sprite Something\"
SET filename1=spritesheet_1.png
SET filename2=spritesheet_2.png
SET output=spritesheet.png

:: Copy to this folder.
copy %folder%%filename1%
copy %folder%%filename2%

:: Use ImageMagick to insert the 16 pico-8 colors as the first 16 tiles of the
:: image (in order to preserve the correct palette in the tile conversion.)
convert ..\tools\colormap_cube.png  %filename1% -append %filename1%

:: Use ImageMagick to convert the image to 16 colors.
convert %filename1% -quantize RGB -remap ..\tools\colormap_cube.png  %filename1%

:: Append 8x8 sprites.
convert -append %filename1% %filename2% %output%

:: Use bmp2tile (in the tools folder) to make tiles out of the appended image.
..\tools\bmp2tile.exe %~dp0%output% -savetiles %~dp0%output%_tiles.inc -fullpalette -spritepalette -noremovedupes -nomirror -tileoffset 128 -exit

:: Remove the comments and the first 32 lines (thus removing the tiles of the
:: color map).
for /f "skip=32 delims=*" %%a in (%output%_tiles.inc) do (
echo %%a >>newfile.txt
)
xcopy newfile.txt %output%_tiles.inc /y

:: Clean up.
del newfile.txt /f /q
del %filename1%
del %filename2%
del %output%
