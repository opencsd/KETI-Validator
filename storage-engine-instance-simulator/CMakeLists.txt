# Copyright 2021 the gRPC authors.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# cmake build file for C++ keyvaluestore example.
# Assumes protobuf and gRPC have been installed using cmake.
# See cmake_externalproject/CMakeLists.txt for all-in-one cmake build
# that automatically builds all the dependencies before building keyvaluestore.

cmake_minimum_required(VERSION 3.5.1)

project(Simulator C CXX)

include(./cmake/common.cmake)
# Proto file
get_filename_component(ss_proto "./proto/storage_engine_instance.proto" ABSOLUTE)
get_filename_component(ss_proto_path "${ss_proto}" PATH)

# Generated sources
set(ss_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/storage_engine_instance.pb.cc")
set(ss_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/storage_engine_instance.pb.h")
set(ss_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/storage_engine_instance.grpc.pb.cc")
set(ss_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/storage_engine_instance.grpc.pb.h")
add_custom_command(
      OUTPUT "${ss_proto_srcs}" "${ss_proto_hdrs}" "${ss_grpc_srcs}" "${ss_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${ss_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${ss_proto}"
      DEPENDS "${ss_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# Include common include directories
include_directories("./include")

# kvs_grpc_proto
add_library(ss_grpc_proto
  ${ss_grpc_srcs}
  ${ss_grpc_hdrs}
  ${ss_proto_srcs}
  ${ss_proto_hdrs})
target_link_libraries(ss_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF}
  )
  target_link_libraries(ss_grpc_proto
  grpc++_reflection
  grpc++
  protobuf)

if (UNIX)
	# Linking POSIX threads
	find_package(Threads REQUIRED)
	set(LINKING_LIBRARIES ${LINKING_LIBRARIES} Threads::Threads)
endif (UNIX)

# interface_container
add_executable(simulator_container "main.cc" "BufferManager.cc" "CSDStatusManager.cc" "MergeQueryManager.cc" "TableManager.cc" "SnippetScheduler.cc")
target_link_libraries(simulator_container
  ss_grpc_proto
  ${LINKING_LIBRARIES}
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})
