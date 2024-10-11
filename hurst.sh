#!/bin/bash
echo "Compiling"
g++ hurst.cpp -std=c++17 -lpthread -lcpprest -lcrypto -lssl -I/Library/Frameworks/Python.framework/Versions/3.11/include/python3.11 -L/Library/Frameworks/Python.framework/Versions/3.11/lib -lpython3.11 -I/Library/Frameworks/Python.framework/Versions/3.11/lib/python3.11/site-packages/numpy/core/include
echo "Compiled"
exit 0