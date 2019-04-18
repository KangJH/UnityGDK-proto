# This script is included by worker and library builds
# It is not meant to be built as a standalone library
project(CppWorkshopStartPoint)
cmake_minimum_required(VERSION 3.13)

set(PROJECT_DIR "${CMAKE_SOURCE_DIR}/../../")
set(SCHEMA_DIR "${PROJECT_DIR}/schema")
set(GENERATED_CODE_DIR "${CMAKE_SOURCE_DIR}/generated")
set(CPP_WORKER_SDK_DIR "${CMAKE_SOURCE_DIR}/dependencies")

if(MSVC)
  add_definitions(/W2 /bigobj)
else()
  # Strict warnings.
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj -std=c++11")
  add_definitions(-Wall -Wextra -Werror -pedantic)
endif()

# C++ API libraries
find_package(Threads)
find_library(LIB_CORE_SDK CoreSdk "${CPP_WORKER_SDK_DIR}/lib" DEFAULT_PATH)
find_library(LIB_WORKER_SDK WorkerSdk "${CPP_WORKER_SDK_DIR}/lib" NO_DEFAULT_PATH)
find_library(LIB_PROTOBUF NAMES protobuf libprotobuf PATHS "${CPP_WORKER_SDK_DIR}/lib" NO_DEFAULT_PATH)
find_library(LIB_RAKNET RakNetLibStatic "${CPP_WORKER_SDK_DIR}/lib" NO_DEFAULT_PATH)
find_library(LIB_SSL ssl "${CPP_WORKER_SDK_DIR}/lib" NO_DEFAULT_PATH)
find_library(LIB_ZLIB NAMES z zlibstatic PATHS "${CPP_WORKER_SDK_DIR}/lib" NO_DEFAULT_PATH)
find_library(LIB_GPR NAMES gpr PATHS "${CPP_WORKER_SDK_DIR}/lib" NO_DEFAULT_PATH)
find_library(LIB_GRPC NAMES grpc PATHS "${CPP_WORKER_SDK_DIR}/lib" NO_DEFAULT_PATH)
find_library(LIB_GRPCXX NAMES grpc++ PATHS "${CPP_WORKER_SDK_DIR}/lib" NO_DEFAULT_PATH)

# Worker SDK in library.
add_library(WorkerSdk INTERFACE)
target_include_directories(WorkerSdk SYSTEM INTERFACE "${CPP_WORKER_SDK_DIR}/include")
target_link_libraries(WorkerSdk INTERFACE
  ${LIB_WORKER_SDK} ${LIB_CORE_SDK} ${CMAKE_THREAD_LIBS_INIT} ${LIB_PROTOBUF}
  ${LIB_RAKNET} ${LIB_ZLIB} ${LIB_GRPCXX} ${LIB_GRPC} ${LIB_GPR} ${LIB_SSL}
  # Must be at end!
  ${CMAKE_DL_LIBS})

#################################
# Schema Project
#################################
file(GLOB_RECURSE SCHEMA_FILES
  "${SCHEMA_DIR}/*.schema"
  )
  
file(GLOB_RECURSE SCHEMA_SOURCE_FILES
  "${GENERATED_CODE_DIR}/*.cc"
  "${GENERATED_CODE_DIR}/*.h"
  )

add_library(Schema STATIC ${SCHEMA_FILES} ${SCHEMA_SOURCE_FILES})
target_include_directories(Schema SYSTEM PUBLIC "${GENERATED_CODE_DIR}")
target_link_libraries(Schema PRIVATE WorkerSdk)

#################################
#        AIWorker Project
#################################
file(GLOB_RECURSE SERVER_SOURCE_FILES
  "src/*.cc"
  "src/*.cpp"
  "src/*.h"
  )

add_executable(AIWorker . ${SERVER_SOURCE_FILES})
target_link_libraries(AIWorker WorkerSdk Schema)
target_include_directories(AIWorker SYSTEM PRIVATE 
 ${CMAKE_SOURCE_DIR}/src
 ${GENERATED_CODE_DIR} 
)
set_target_properties(AIWorker PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(AIWorker PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set_target_properties(AIWorker PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
