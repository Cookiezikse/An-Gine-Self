@echo off
set glslc="C:\VulkanSDK\1.3.275.0\Bin\glslc.exe"
set shader_dir="%~dp0Shaders"

rem Compilation des fichiers .vert

for %%f in ("%shader_dir%\*.vert") do (
    echo Compiling: %%f
    "%glslc%" -c "%%f" -o "%%~dpnf.vert.spv"
)

for %%f in ("%shader_dir%\*.frag") do (
    echo Compiling: %%f
    "%glslc%" -c "%%f" -o "%%~dpnf.frag.spv"
)

echo Compilation des shaders terminée.
pause