#!/bin/bash

mkdir -p headers
cd headers
wget https://electronjs.org/headers/v29.2.0/node-v29.2.0-headers.tar.gz
tar -xf node-v29.2.0-headers.tar.gz
rm node-v29.2.0-headers.tar.gz
wget https://raw.githubusercontent.com/nodejs/node/main/deps/v8/include/v8config.h

echo ""
echo DONE.