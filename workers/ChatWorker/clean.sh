#!/usr/bin/env bash

set -e -x

cd "$(dirname "$0")"

BUILD_DIR="$(pwd)"
DOWNLOAD_DIR="$(pwd)/build"

# Remove Generated files from Schema
rm -rf generated

# Clean CMake files
rm -rf cmake_build
rm -rf CMakeFiles
rm -f CMakeCache.txt
rm -rf dependencies
rm -rf bin

