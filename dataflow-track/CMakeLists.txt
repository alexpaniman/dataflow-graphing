cmake_minimum_required(VERSION 3.24)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(dataflow VERSION 1.0)

set(DEFAULT_BUILD_TYPE "Debug") # Source files are mostly for debugging anyway

# ==> Add option to force ANSI-colors (useful for building with Ninja)

option(FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." FALSE)

if (${FORCE_COLORED_OUTPUT})
  if     ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options (-fdiagnostics-color=always)
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options (-fcolor-diagnostics)
  endif ()
endif ()

# ==> Enable as much optimization as possible (note, some of these are dangerous)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -D NDEBUG -mavx2 -march=native -fomit-frame-pointer -mtune=native -Ofast -flto=auto")

# ==> Flags for debugging, enable sanitizers and make debugging experience as good as possible
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D _DEBUG -ggdb3 -std=c++2b -fcxx-exceptions -Wall -Wextra -Weffc++ -Wcast-align -Wcast-qual -Wchar-subscripts -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat=2 -Winline -Wnon-virtual-dtor -Woverloaded-virtual -Wpacked -Wpointer-arith -Wredundant-decls -Wsign-promo -Wstrict-overflow=2 -Wsuggest-override -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused -Wvariadic-macros -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -fcheck-new -fsized-deallocation -fstack-check -fstack-protector -fstrict-overflow -fno-omit-frame-pointer -fPIE -mavx2 -march=native -mtune=native -O0")

# ==> GCC supports some warnings that clang doesn't
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Waggressive-loop-optimizations -Wconditionally-supported -Wformat-signedness -Wlogical-op -Wopenmp-simd -Wstrict-null-sentinel -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsync-nand -Wuseless-cast -fconcepts-diagnostics-depth=3 -Wno-literal-suffix")
endif ()

add_subdirectory(lib/axp)
add_subdirectory(lib/graphviz)
add_subdirectory(lib/dataflow)

add_subdirectory(src)
