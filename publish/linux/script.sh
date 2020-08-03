#!/bin/bash
git clone https://github.com/decode-it/cook 
cd cook
rake prepare b0:build b1:build
cp build/b1/cook.exe /var/publish/
