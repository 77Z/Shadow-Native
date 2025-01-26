@echo off


curl -LO https://github.com/denoland/deno/releases/latest/download/deno-x86_64-pc-windows-msvc.zip

tar -xf deno-x86_64-pc-windows-msvc.zip

del deno-x86_64-pc-windows-msvc.zip

echo "Deno has been downloaded and unzipped successfully."