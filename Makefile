#============================== USER DEFINITIONS ==============================#
# Project name
PROJECT_NAME = $(shell basename $(CURDIR))

# Objects dir
OBJDIR = .objs

# Tools
## C Compiler
CC = gcc
# CC = clang
## C++ Compiler
CXX = g++
# CXX = clang++
## Linker
LD = g++

# Files excluded from compiling
# Info: You need to specify relative path or filename, examples:
# path/to/file.c 	-> exclude path/to/file.c file
# path 				-> exclude whole path dir in current folder
# file.c 			-> exclude all files with name file.c
EXCLUDE =

# Additional include dirs, outside of source folder
ADDITIONAL_INC_DIRS =


# Flags
## CXX flags
CXXFLAGS =
### Warnings
# Essential
CXXFLAGS +=-Wall -Wextra
# Useful because usually testing floating-point numbers for equality is bad
CXXFLAGS +=-Wfloat-equal
# Warn if an uninitialized identifier is evaluated in an #if directive
CXXFLAGS +=-Wundef
# Warn whenever a local variable shadows another local variable, parameter or
# global variable or whenever a built-in function is shadowed
CXXFLAGS +=-Wshadow
# Warn if anything depends upon the size of a function or of void
CXXFLAGS +=-Wpointer-arith
# Warn whenever a pointer is cast such that the required alignment of the target
# is increased. For example, warn if a char * is cast to an int * on machines
# where integers can only be accessed at two- or four-byte boundaries
CXXFLAGS +=-Wcast-align
# Warns about cases where the compiler optimizes based on the assumption that
# signed overflow does not occur
# (The value 5 may be too strict, see the manual page)
# CXXFLAGS +=-Wstrict-overflow=5
# Give string constants the type const char[length] so that copying the address
# of one into a non-const char * pointer will get a warning.
CXXFLAGS +=-Wwrite-strings
# Warn if any functions that return structures or unions are defined or called
# CXXFLAGS +=-Waggregate-return
# Warn whenever a pointer is cast to remove a type qualifier from the target type*
CXXFLAGS +=-Wcast-qual
# Warn whenever a switch statement does not have a default case*
CXXFLAGS +=-Wswitch-default
# Warn whenever a switch statement has an index of enumerated type and lacks a
# case for one or more of the named codes of that enumeration*.
CXXFLAGS +=-Wswitch-enum
# Warn for implicit conversions that may alter a value*.
CXXFLAGS +=-Wconversion
# Warn if the compiler detects that code will never be executed*.
CXXFLAGS +=-Wunreachable-code
# For asan (compiling with clang needed)
# CXXFLAGS +=-fsanitize=address -fno-omit-frame-pointer

## C flags
CFLAGS = ${CXXFLAGS}
# Warn if a function is declared or defined without specifying the argument types
CFLAGS +=-Wstrict-prototypes

### Optimization
# For debugging
# OPTFLAGS =-Og
# No optimization
# OPTFLAGS =-O0
# Adivised
OPTFLAGS =-O2
# Space optimization, level -02 mostly as well
# OPTFLAGS =-Os

# Debugging
# DEBFLAGS =-g
# DEBFLAGS =-ggdb
# DEBFLAGS =-ggdb1
# DEBFLAGS =-ggdb3

# Generate .d files
DEPFLAGS =-MMD -MP -MF"$(@:%.o=%.d)" -MT"$@"
DEPFLAGS_ECHO =-MMD -MP -MF"$(BLUE_COLOR)$(@:%.o=%.d)$(END_COLOR)" -MT"$(BLUE_COLOR)$@$(END_COLOR)"

# Linker flags
LDFLAGS =-lncurses
# For asan (compiling with clang needed)
# LDFLAGS +=-fsanitize=address
# google tests
# LDFLAGS += -lgtest -lgtest_main


#============================= COLOR DEFINITIONS ==============================#
WHITE_COLOR = \e[1;37m
LIGHT_GRAY_COLOR = \e[0;37m
DARK_GRAY_COLOR = \e[1;30m
BLACK_COLOR = \e[0;30m
YELLOW_COLOR = \e[1;33m
ORANGE_COLOR = \e[0;33m
LIGHT_RED_COLOR = \e[1;31m
RED_COLOR = \e[0;31m
LIGHT_PURPLE_COLOR = \e[1;35m
PURPLE_COLOR = \e[0;35m
LIGHT_BLUE_COLOR = \e[1;34m
BLUE_COLOR = \e[0;34m
LIGHT_CYAN_COLOR = \e[1;36m
CYAN_COLOR = \e[0;36m
LIGHT_GREEN_COLOR = \e[1;32m
GREEN_COLOR = \e[0;32m
END_COLOR = \e[0m


#=========================== FUNCTION DEFINITIONS =============================#
# Recursive search function
rwildcard = $(wildcard $1$2) $(foreach d,$(filter-out $(wildcard $1.*),$(wildcard $1*/)), $(call rwildcard,$d,$2))

# Remove multiple same instances from the list
uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

exclude_files = $(if $(strip $(1)), $(filter-out $(1), $(filter-out $(addprefix %, $(notdir $(1))), $(2))), $(2))
exclude_dirs = $(if $(strip $(1)), $(foreach dir,$(1),$(filter-out $(dir)%,$(2))), $(2))

find_files = $(strip $(call exclude_files, $(1), $(call exclude_dirs, $(1), $(call rwildcard,./,$(2)))))


#============================== FILE SCANNING =================================#
# Prepend excluded paths with ./ if not already present
EXCLUDE := $(foreach item,$(EXCLUDE),$(if $(filter ./%,$(item)),$(item),./$(item)))

# Find all h header files
C_INC_DIRS := $(call uniq,$(sort $(dir $(call find_files,$(EXCLUDE),*.h))))
C_INC_DIRS_ECHO := $(addprefix -I"$(BLUE_COLOR),$(addsuffix $(END_COLOR)",$(C_INC_DIRS)))

# Find all hpp header files
CXX_INC_DIRS := $(call uniq,$(sort $(dir $(call find_files,$(EXCLUDE),*.hpp))))
CXX_INC_DIRS_ECHO := $(addprefix -I"$(BLUE_COLOR),$(addsuffix $(END_COLOR)",$(CXX_INC_DIRS)))

# Additional header files
ADDITIONAL_INC_DIRS := $(call uniq,$(sort $(ADDITIONAL_INC_DIRS)))
ADDITIONAL_INC_DIRS_ECHO := $(addprefix -I"$(BLUE_COLOR),$(addsuffix $(END_COLOR)",$(ADDITIONAL_INC_DIRS)))

# Add -I prefix to all includes
INC_DIRS := $(addprefix -I",$(addsuffix ",$(ADDITIONAL_INC_DIRS) $(CXX_INC_DIRS) $(C_INC_DIRS)))
INC_DIRS_ECHO := $(ADDITIONAL_INC_DIRS_ECHO) $(CXX_INC_DIRS_ECHO) $(C_INC_DIRS_ECHO)

# Find all c source files
C_SRCS := $(call find_files,$(EXCLUDE),*.c)
C_OBJS := $(strip $(patsubst %c, %o, $(C_SRCS)))

# Find all cpp source files
CXX_SRCS := $(call find_files,$(EXCLUDE),*.cpp)
CXX_OBJS := $(strip $(patsubst %cpp, %o, $(CXX_SRCS)))

# Join all objs in one variable
OBJS := $(CXX_OBJS) $(C_OBJS)


#=============================== BUILDING =====================================#
# Rebuild target
rebuild: clean .WAIT all

# All Targets
all: pre-build .WAIT main-build post-build


#=============================== COMPILING ====================================#
# Main-build Target
main-build: $(PROJECT_NAME)

$(PROJECT_NAME) : $(addprefix $(OBJDIR),$(OBJS:.%=%))
	-@echo -e '$(PURPLE_COLOR)Building target:$(END_COLOR) $(BLUE_COLOR)$@$(END_COLOR)'
	-@echo -e '$(strip $(ORANGE_COLOR)$(LD)$(END_COLOR) $(LDFLAGS) $(GREEN_COLOR)$^$(END_COLOR) -o "$(BLUE_COLOR)$(PROJECT_NAME)$(END_COLOR)")'
	@$(LD) $^ $(LDFLAGS) -o "$(PROJECT_NAME)"
	-@echo -e '$(PURPLE_COLOR)Finished building target:$(END_COLOR) $(GREEN_COLOR)$@$(END_COLOR)'

$(OBJDIR)/%.o : %.cpp
	-@echo -e '$(PURPLE_COLOR)Building file:$(END_COLOR) $(BLUE_COLOR)$<$(END_COLOR)'
	-@echo -e '$(strip $(ORANGE_COLOR)$(CXX)$(END_COLOR) $(INC_DIRS_ECHO) $(CXXFLAGS) $(DEPFLAGS_ECHO) $(DEBFLAGS) $(OPTFLAGS) -o "$(BLUE_COLOR)$@$(END_COLOR)" -c "$(BLUE_COLOR)$<$(END_COLOR)")'
	@$(CXX) $(INC_DIRS) $(CXXFLAGS) $(DEPFLAGS) $(DEBFLAGS) $(OPTFLAGS) -o "$@" -c "$<"
	-@echo -e '$(PURPLE_COLOR)Finished building:$(END_COLOR) $(GREEN_COLOR)$<$(END_COLOR)'
	-@echo ' '

$(OBJDIR)/%.o : %.c
	-@echo -e '$(PURPLE_COLOR)Building file:$(END_COLOR) $(BLUE_COLOR)$<$(END_COLOR)'
	-@echo -e '$(strip $(ORANGE_COLOR)$(CC)$(END_COLOR) $(INC_DIRS_ECHO) $(CFLAGS) $(DEPFLAGS_ECHO) $(DEBFLAGS) $(OPTFLAGS) -o "$(BLUE_COLOR)$@$(END_COLOR)" -c "$(BLUE_COLOR)$<$(END_COLOR)")'
	@$(CC) $(INC_DIRS) $(CFLAGS) $(DEPFLAGS) $(DEBFLAGS) $(OPTFLAGS) -o "$@" -c "$<"
	-@echo -e '$(PURPLE_COLOR)Finished building:$(END_COLOR) $(GREEN_COLOR)$<$(END_COLOR)'
	-@echo ' '


#============================= DEPENDENCIES ===================================#
DEPS := $(OBJS:%.o=%.d)
DEPS := $(addprefix $(OBJDIR),$(DEPS:.%=%))
# Include them as makefiles
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(DEPS),)
-include $(DEPS)
endif
endif


#============================ OTHER TARGETS ===================================#
clean:
	-@echo -e '$(BLUE_COLOR)Cleaning project:$(END_COLOR)'
	-@echo -e '$(ORANGE_COLOR)$(RM)$(END_COLOR) -rf "$(BLUE_COLOR)$(PROJECT_NAME)$(END_COLOR)" $(LIGHT_BLUE_COLOR)$(OBJDIR)$(END_COLOR)'
	-@rm -rf "$(PROJECT_NAME)" $(OBJDIR)
	-@echo -e '$(GREEN_COLOR)Project cleaned.$(END_COLOR)'
	-@echo ''
	-@echo ''


# Map same folder structure to OBJDIR folder
ifeq ($(OS),Windows_NT)
pre-build:
	@echo -e '$(BLUE_COLOR)Start building:$(END_COLOR)'
	@echo off
	@setlocal enabledelayedexpansion
	@set "root=%CD%"
	@if not exist "$(OBJDIR)" mkdir $(OBJDIR)
	@for /d /r %%D in (.) do (
	    set "relpath=%%D"
	    set "relpath=!relpath:%root%=!"
	    if not "!relpath:~0,$(OBJDIR):~0,5%!"=="$(OBJDIR)" if not exist "$(OBJDIR)!relpath!" mkdir "$(OBJDIR)!relpath!" > nul 2>&1
	)
	@endlocal
else 	# Unix
pre-build:
	@echo -e '$(BLUE_COLOR)Start building:$(END_COLOR)'
	@mkdir -p $(OBJDIR)
	@find . -not \( -path "./.*" -prune \) -type d -print0 > .dirs
	@cd $(OBJDIR) && xargs -0 mkdir -p < ../.dirs
	@rm .dirs
endif


post-build:
	@echo -e '$(GREEN_COLOR)Build finished.$(END_COLOR)'
	@echo ''
	@echo ''


.PHONY: all clean pre-build
.SECONDARY: main-build post-build
.DEFAULT_GOAL := all
