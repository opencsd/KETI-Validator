cmake_minimum_required(VERSION 3.12)

include(./common.cmake)

add_executable(KETI_Validator  "QueryParser.cpp" "RestHandler.cpp" "SSDValidator.cpp" "CSDValidator.cpp" "HistogramTable.cpp" "StorageValidator.cpp" "main.cpp")

set(Casablanca_LIBRARIES "-lboost_system -lcrypto -lssl -lcpprest")
set(LINKING_LIBRARIES ${Casablanca_LIBRARIES})
set(CMAKE_C_COMPILER ${GCC})
set(CMAKE_CXX_COMPILER ${GPP})
target_compile_features(KETI_Validator PRIVATE cxx_std_17)


if (UNIX)
	# Linking POSIX threads
	find_package(Threads REQUIRED)
	set(LINKING_LIBRARIES ${LINKING_LIBRARIES} Threads::Threads)
endif (UNIX)

target_link_libraries(KETI_Validator
    mysqlcppconn
    ${LINKING_LIBRARIES})