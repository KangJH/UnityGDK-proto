#!/usr/bin/env bash

set -e -x

cd "$(dirname "$0")"

SCHEMA_DIRS="$(pwd)/../../schema"
BUILD_PLATFORMS=(macOS64 Windows64 Linux64)
DOWNLOAD_DIR="$(pwd)/dependencies"
BUILD_DIR="$(pwd)"
SDK_VERSION="13.4.0"
mkdir -p "${DOWNLOAD_DIR}"

function isLinux() {
  [[ "$(uname -s)" == "Linux" ]]
}

function isMacOS() {
  [[ "$(uname -s)" == "Darwin" ]]
}

function isWindows() {
  ! (isLinux || isMacOS)
}

# Return the target platform used by worker package names built for this OS.
function getPlatformName() {
  if isLinux; then
    echo "linux"
  elif isMacOS; then
    echo "macos"
  elif isWindows; then
    echo "win32"
  else
    echo "ERROR: Unknown platform." >&2
    exit 1
  fi
}

retrievePackage() {
  TYPE=$1
  PACKAGE=$2

  pushd "${DOWNLOAD_DIR}"
  if [ ! -f "${PACKAGE}.zip" ]; then
    spatial package retrieve --force "${TYPE}" "${PACKAGE}" "${SDK_VERSION}" "${PACKAGE}.zip"
    unzip -o "${PACKAGE}.zip"
  fi
  popd
}

# Get the tools:
# * Spatial Schema compiler
# * Standard Library Schemas
# * Core SDK for all platforms to enable building workers for MacOS, Windows or Linux
# * C++ SDK
retrievePackage "tools" "schema_compiler-x86_64-win32"
retrievePackage "schema" "standard_library"
retrievePackage "tools" "snapshot_converter-x86_64-win32" "snapshot_converter"
# retrievePackage "worker_sdk" "cpp"

if isWindows; then
  retrievePackage "worker_sdk" "cpp-static-x86_64-msvc_mdd-win32"
elif isMacOS; then
  retrievePackage "worker_sdk" "cpp-static-x86_64-clang_libcpp-macos"
else
  retrievePackage "worker_sdk" "cpp-static-x86_64-gcc_libstdcpp-linux"
fi

# Generated Schema files.
OUT_DIR=generated
mkdir -p "$OUT_DIR"
#mkdir -p "${BUILD_DIR}/SpatialOS/schema/bin"
"${DOWNLOAD_DIR}"/schema_compiler --schema_path="${SCHEMA_DIRS}" --schema_path="${DOWNLOAD_DIR}" --cpp_out="$OUT_DIR" --load_all_schema_on_schema_path "${SCHEMA_DIRS}"/*/*.schema "${DOWNLOAD_DIR}"/improbable/*.schema
#"${DOWNLOAD_DIR}/schema_compiler" --schema_path="${BUILD_DIR}/SpatialOS/schema" --schema_path="${DOWNLOAD_DIR}" --descriptor_set_out="${BUILD_DIR}/SpatialOS/schema/bin/schema.descriptor" --load_all_schema_on_schema_path "${DOWNLOAD_DIR}"/improbable/*.schema "${BUILD_DIR}"/SpatialOS/schema/*/*/*.schema

cmake -E make_directory cmake_build
pushd cmake_build
cmake ../ -G"Visual Studio 15 2017 Win64"
cmake --build . --target ChatWorker

echo "Build complete"
