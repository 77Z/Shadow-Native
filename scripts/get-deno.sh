#!/bin/bash

# Bash script to download and extract latest version of Deno for use via Horde

wget https://github.com/denoland/deno/releases/latest/download/deno-x86_64-unknown-linux-gnu.zip
unzip deno-x86_64-unknown-linux-gnu.zip
rm deno-x86_64-unknown-linux-gnu.zip
chmod +x ./deno