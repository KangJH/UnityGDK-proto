#!/usr/bin/env bash

set -e

if [ "$#" -ne "3" ]; then
  echo "Usage: $(basename "$0") <binary_name> <output_dir>"
  exit 1
fi

if [ -z "$LINUX_MULTIARCH_ROOT" ]; then
  echo "You must set the environment variable LINUX_MULTIARCH_ROOT"
  exit 1
fi

WORKER_BINARY="$1"
OUTPUT_DIR="$2"

PLATFORM_TARGET="x86_64-unknown-linux-gnu"
CROSSTOOL_ROOT="${LINUX_MULTIARCH_ROOT}/${PLATFORM_TARGET}"
CLANG="${CROSSTOOL_ROOT}/bin/clang++.exe"
WORKER_SDK_DIR="$(pwd)/../dependencies"
GENERATED_CODE_DIR="$(pwd)/../generated"
SOURCE_PATH="$(pwd)/../src"
if [ ! -d "$CROSSTOOL_ROOT" ]; then
  echo "Could not find cross compile directory at: $CROSSTOOL_ROOT"
  exit 1
fi

SOURCE_FILES="$(find ${SOURCE_PATH} -type f -name "*.cc" -printf "%p ")
  $(find ${SOURCE_PATH} -type f -name "*.cpp" -printf "%p ")
  $(find "$GENERATED_CODE_DIR" -type f -name "*.cc" -printf "%p ")"
echo "Source Files: $SOURCE_FILES"
INCLUDE_PATHS="-I${SOURCE_PATH}  -I${GENERATED_CODE_DIR} -I${WORKER_SDK_DIR}/include"

LIB_PATHS="-L${WORKER_SDK_DIR}/lib"
# Order is important here
LIB_DEPENDENCIES="-lWorkerSdk -lCoreSdk -lpthread -lprotobuf -lRakNetLibStatic -lz -lgrpc++ -lgrpc -lgpr -lssl -ldl"

mkdir -p "$OUTPUT_DIR"

 # Library args need to appear after source args
"$CLANG" -v -target "$PLATFORM_TARGET" \
  "--sysroot=${CROSSTOOL_ROOT}" \
  "--gcc-toolchain=${CROSSTOOL_ROOT}" \
  --std=c++17 \
  $INCLUDE_PATHS $SOURCE_FILES $LIB_PATHS $LIB_DEPENDENCIES \
  -o "${OUTPUT_DIR}/${WORKER_BINARY}"

# Write out a runner script.
cat >"${OUTPUT_DIR}/RunWorker.sh" <<END
#!/bin/sh
WORKER_BINARY="$WORKER_BINARY"
TRUE_SCRIPT_NAME="\$(echo "\$0" | xargs readlink -f)"
WORKER_ROOT=\$(dirname "\$TRUE_SCRIPT_NAME")
chmod +x "\${WORKER_ROOT}/\${WORKER_BINARY}"
"\${WORKER_ROOT}/\${WORKER_BINARY}" \$@
END