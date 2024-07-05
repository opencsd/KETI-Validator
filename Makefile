# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/workspace/keti/Validator-Instance

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/workspace/keti/Validator-Instance/cmake/build

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/local/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/local/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /root/workspace/keti/Validator-Instance/cmake/build/CMakeFiles /root/workspace/keti/Validator-Instance/cmake/build//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /root/workspace/keti/Validator-Instance/cmake/build/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named KETI_Validator

# Build rule for target.
KETI_Validator: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 KETI_Validator
.PHONY : KETI_Validator

# fast build rule for target.
KETI_Validator/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/build
.PHONY : KETI_Validator/fast

CSDValidator.o: CSDValidator.cpp.o
.PHONY : CSDValidator.o

# target to build an object file
CSDValidator.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/CSDValidator.cpp.o
.PHONY : CSDValidator.cpp.o

CSDValidator.i: CSDValidator.cpp.i
.PHONY : CSDValidator.i

# target to preprocess a source file
CSDValidator.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/CSDValidator.cpp.i
.PHONY : CSDValidator.cpp.i

CSDValidator.s: CSDValidator.cpp.s
.PHONY : CSDValidator.s

# target to generate assembly for a file
CSDValidator.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/CSDValidator.cpp.s
.PHONY : CSDValidator.cpp.s

HistogramTable.o: HistogramTable.cpp.o
.PHONY : HistogramTable.o

# target to build an object file
HistogramTable.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/HistogramTable.cpp.o
.PHONY : HistogramTable.cpp.o

HistogramTable.i: HistogramTable.cpp.i
.PHONY : HistogramTable.i

# target to preprocess a source file
HistogramTable.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/HistogramTable.cpp.i
.PHONY : HistogramTable.cpp.i

HistogramTable.s: HistogramTable.cpp.s
.PHONY : HistogramTable.s

# target to generate assembly for a file
HistogramTable.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/HistogramTable.cpp.s
.PHONY : HistogramTable.cpp.s

QEMUHandler.o: QEMUHandler.cpp.o
.PHONY : QEMUHandler.o

# target to build an object file
QEMUHandler.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/QEMUHandler.cpp.o
.PHONY : QEMUHandler.cpp.o

QEMUHandler.i: QEMUHandler.cpp.i
.PHONY : QEMUHandler.i

# target to preprocess a source file
QEMUHandler.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/QEMUHandler.cpp.i
.PHONY : QEMUHandler.cpp.i

QEMUHandler.s: QEMUHandler.cpp.s
.PHONY : QEMUHandler.s

# target to generate assembly for a file
QEMUHandler.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/QEMUHandler.cpp.s
.PHONY : QEMUHandler.cpp.s

QueryParser.o: QueryParser.cpp.o
.PHONY : QueryParser.o

# target to build an object file
QueryParser.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/QueryParser.cpp.o
.PHONY : QueryParser.cpp.o

QueryParser.i: QueryParser.cpp.i
.PHONY : QueryParser.i

# target to preprocess a source file
QueryParser.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/QueryParser.cpp.i
.PHONY : QueryParser.cpp.i

QueryParser.s: QueryParser.cpp.s
.PHONY : QueryParser.s

# target to generate assembly for a file
QueryParser.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/QueryParser.cpp.s
.PHONY : QueryParser.cpp.s

RestHandler.o: RestHandler.cpp.o
.PHONY : RestHandler.o

# target to build an object file
RestHandler.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/RestHandler.cpp.o
.PHONY : RestHandler.cpp.o

RestHandler.i: RestHandler.cpp.i
.PHONY : RestHandler.i

# target to preprocess a source file
RestHandler.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/RestHandler.cpp.i
.PHONY : RestHandler.cpp.i

RestHandler.s: RestHandler.cpp.s
.PHONY : RestHandler.s

# target to generate assembly for a file
RestHandler.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/RestHandler.cpp.s
.PHONY : RestHandler.cpp.s

SSDValidator.o: SSDValidator.cpp.o
.PHONY : SSDValidator.o

# target to build an object file
SSDValidator.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/SSDValidator.cpp.o
.PHONY : SSDValidator.cpp.o

SSDValidator.i: SSDValidator.cpp.i
.PHONY : SSDValidator.i

# target to preprocess a source file
SSDValidator.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/SSDValidator.cpp.i
.PHONY : SSDValidator.cpp.i

SSDValidator.s: SSDValidator.cpp.s
.PHONY : SSDValidator.s

# target to generate assembly for a file
SSDValidator.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/SSDValidator.cpp.s
.PHONY : SSDValidator.cpp.s

StorageValidator.o: StorageValidator.cpp.o
.PHONY : StorageValidator.o

# target to build an object file
StorageValidator.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/StorageValidator.cpp.o
.PHONY : StorageValidator.cpp.o

StorageValidator.i: StorageValidator.cpp.i
.PHONY : StorageValidator.i

# target to preprocess a source file
StorageValidator.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/StorageValidator.cpp.i
.PHONY : StorageValidator.cpp.i

StorageValidator.s: StorageValidator.cpp.s
.PHONY : StorageValidator.s

# target to generate assembly for a file
StorageValidator.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/StorageValidator.cpp.s
.PHONY : StorageValidator.cpp.s

main.o: main.cpp.o
.PHONY : main.o

# target to build an object file
main.cpp.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/main.cpp.o
.PHONY : main.cpp.o

main.i: main.cpp.i
.PHONY : main.i

# target to preprocess a source file
main.cpp.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/main.cpp.i
.PHONY : main.cpp.i

main.s: main.cpp.s
.PHONY : main.s

# target to generate assembly for a file
main.cpp.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/KETI_Validator.dir/build.make CMakeFiles/KETI_Validator.dir/main.cpp.s
.PHONY : main.cpp.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... KETI_Validator"
	@echo "... CSDValidator.o"
	@echo "... CSDValidator.i"
	@echo "... CSDValidator.s"
	@echo "... HistogramTable.o"
	@echo "... HistogramTable.i"
	@echo "... HistogramTable.s"
	@echo "... QEMUHandler.o"
	@echo "... QEMUHandler.i"
	@echo "... QEMUHandler.s"
	@echo "... QueryParser.o"
	@echo "... QueryParser.i"
	@echo "... QueryParser.s"
	@echo "... RestHandler.o"
	@echo "... RestHandler.i"
	@echo "... RestHandler.s"
	@echo "... SSDValidator.o"
	@echo "... SSDValidator.i"
	@echo "... SSDValidator.s"
	@echo "... StorageValidator.o"
	@echo "... StorageValidator.i"
	@echo "... StorageValidator.s"
	@echo "... main.o"
	@echo "... main.i"
	@echo "... main.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

