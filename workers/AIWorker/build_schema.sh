SCHEMA_DIRS="$(pwd)/../../schema"
BUILD_DIR="$(pwd)"
DOWNLOAD_DIR="$(pwd)/dependencies"
OUT_DIR=generated
mkdir -p "$OUT_DIR"
"${DOWNLOAD_DIR}"/schema_compiler --schema_path="${SCHEMA_DIRS}" --schema_path="${DOWNLOAD_DIR}" --cpp_out="$OUT_DIR" --load_all_schema_on_schema_path "${SCHEMA_DIRS}"/*/*.schema "${DOWNLOAD_DIR}"/improbable/*.schema


cmake -E make_directory cmake_build
pushd cmake_build
cmake ../ -G"Visual Studio 15 2017 Win64"
cmake --build . --target Schema