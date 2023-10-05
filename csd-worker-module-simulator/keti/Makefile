#include ../rocksdb/make_config.mk

ifndef DISABLE_JEMALLOC
	ifdef JEMALLOC
		PLATFORM_CXXFLAGS += -DROCKSDB_JEMALLOC -DJEMALLOC_NO_DEMANGLE
	endif
	EXEC_LDFLAGS := $(JEMALLOC_LIB) $(EXEC_LDFLAGS) -lpthread
	PLATFORM_CXXFLAGS += $(JEMALLOC_INCLUDE)
endif

ifneq ($(USE_RTTI), 1)
	CXXFLAGS += -fno-rtti
endif

CFLAGS += -Wstrict-prototypes

.PHONY: clean librocksdb

all: main

clean:
	rm -rf ./server
	rm -rf ./test
	rm -rf ./sstopen

librocksdb:
	cd ../rocksdb && $(MAKE) static_lib

main: librocksdb main.cc
	$(CXX) $(CXXFLAGS) $@.cc input.cc return.cc merge_manager.cc filter.cc csd_table_manager.cc scan.cc  -o$@ ../rocksdb/librocksdb.a -I../rocksdb/include -L/usr/local/lib -lzstd -lsnappy -lz -llz4 -lbz2 -ldl -O2 -std=c++17 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)	

# main: librocksdb main.cc
# 	$(CXX) $(CXXFLAGS) $@.cc return.cc merge_manager.cc filter.cc csd_table_manager.cc scan.cc  -o$@ ../rocksdb/librocksdb.a -I../rocksdb/include ../../compress_libs/zstd/lib/libzstd.a ../../compress_libs/lz4/lib/liblz4.a ../../compress_libs/snappy/build/libsnappy.a ../../compress_libs/zlib/libz.a ../../compress_libs/bzip2/build/libbz2_static.a -ldl -O2 -std=c++17 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)

# main: librocksdb main.cc
# 	$(CXX) $(CXXFLAGS) $@.cc return.cc merge_manager.cc filter.cc csd_table_manager.cc scan.cc  -o$@ ../../csd_file_scan/rocksdb/librocksdb.a -I../rocksdb/include ../../compress_libs/zstd/lib/libzstd.a ../../compress_libs/lz4/lib/liblz4.a ../../compress_libs/snappy/build/libsnappy.a ../../compress_libs/zlib/libz.a ../../compress_libs/bzip2/build/libbz2_static.a -ldl -O2 -std=c++17 $(PLATFORM_LDFLAGS) $(PLATFORM_CXXFLAGS) $(EXEC_LDFLAGS)