#!/bin/bash

cmake -B build .
cmake --build build

ln -sf build/output .
