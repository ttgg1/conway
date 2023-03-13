CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
project(conway)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-Ofast")

find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)

#pkg_check_modules(GLIB REQUIRED glib-2.0)


FIND_PATH(MATH_INCLUDE_DIR math.h)

SET(MATH_NAMES ${MATH_NAMES} m)
FIND_LIBRARY(MATH_LIBRARY NAMES ${MATH_NAMES})

# handle the QUIETLY and REQUIRED arguments and set MATH_FOUND to TRUE if
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MATH DEFAULT_MSG MATH_LIBRARY MATH_INCLUDE_DIR)

IF (MATH_FOUND)
    SET(MATH_LIBRARIES ${MATH_LIBRARY} -lm)
ENDIF (MATH_FOUND)

file(GLOB_RECURSE Foo_SOURCES CONFIGURE_DEPENDS "src/*.c")
file(GLOB_RECURSE Foo_HEADERS CONFIGURE_DEPENDS "src/*.h")

set(Foo_INCLUDE_DIRS "")
foreach (_headerFile ${Foo_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list(APPEND Foo_INCLUDE_DIRS ${_dir})
endforeach ()
list(REMOVE_DUPLICATES Foo_INCLUDE_DIRS)

add_executable(conway ${Foo_SOURCES})

target_include_directories(conway PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(conway ${SDL2_LIBRARIES} ${MATH_LIBRARY})