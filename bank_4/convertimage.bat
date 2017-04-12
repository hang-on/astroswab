SET file="%1"

:: Copy to this folder.
copy %file% %~dp0

:: Use ImageMagick to convert the image to 16 colors.
convert scene_1.png -quantize RGB -remap colormap.png  converted_image.bmp
pause
