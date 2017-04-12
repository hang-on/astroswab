:: Trimmer
for /f "skip=32 delims=*" %%a in (spritesheet.png_tiles.inc) do (
echo %%a >>newfile.txt
)
xcopy newfile.txt trimmed_sheet.inc /y
del newfile.txt /f /q
