cmake_minimum_required(VERSION 3.14)


################################################################################
#   Options
################################################################################
option(ADDRESS_SANITIZER "Enable AddressSanitizer" OFF)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
        "Choose the type of build (Debug, Release, RelWithDebInfo, MinSizeRel)")
endif()


################################################################################
#   Project configuration
################################################################################
project(nmcurse
        VERSION 0.1.0
        DESCRIPTION "Curses interface for NetworkManager")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Support for linters
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Optimizations
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Let's nicely support folders in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


################################################################################
#   Dependencies
################################################################################
find_package(Threads REQUIRED)
find_package(Curses REQUIRED)


################################################################################
#   Flags
################################################################################
set(COMPILE_FLAGS
    # Essential
    -Wall -Wextra
    # Useful because usually testing floating-point numbers for equality is bad
    -Wfloat-equal
    # Warn if an uninitialized identifier is evaluated in an #if directive
    -Wundef
    # Warn whenever a local variable shadows another local variable, parameter or
    # global variable or whenever a built-in function is shadowed
    -Wshadow
    # Warn if anything depends upon the size of a function or of void
    -Wpointer-arith
    # Warn whenever a pointer is cast such that the required alignment of the target
    # is increased. For example, warn if a char * is cast to an int * on machines
    # where integers can only be accessed at two- or four-byte boundaries
    -Wcast-align
    # Warns about cases where the compiler optimizes based on the assumption that
    # signed overflow does not occur
    # (The value 5 may be too strict, see the manual page)
    -Wstrict-overflow=2
    # Give string constants the type const char[length] so that copying the address
    # of one into a non-const char * pointer will get a warning.
    -Wwrite-strings
    # Warn if any functions that return structures or unions are defined or called
    # add_compile_options(-Waggregate-return)
    # Warn whenever a pointer is cast to remove a type qualifier from the target type*
    -Wcast-qual
    # Warn whenever a switch statement does not have a default case*
    -Wswitch-default
    # Warn whenever a switch statement has an index of enumerated type and lacks a
    # case for one or more of the named codes of that enumeration*.
    -Wswitch-enum
    # Warn for implicit conversions that may alter a value*.
    -Wconversion
    # Warn if the compiler detects that code will never be executed*.
    -Wunreachable-code
)


################################################################################
#   Target
################################################################################
set(SOURCE_FILES
    main.cpp
    ui/ui.cpp
    ui/network.cpp
)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CURSES_INCLUDE_DIR}
)

target_compile_options(${PROJECT_NAME} PRIVATE ${COMPILE_FLAGS})
if (ADDRESS_SANITIZER)
    target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
endif(ADDRESS_SANITIZER)

target_link_libraries(${PROJECT_NAME} ${CURSES_LIBRARIES} Threads::Threads)

install(TARGETS ${PROJECT_NAME} DESTINATION /usr/local/bin)
