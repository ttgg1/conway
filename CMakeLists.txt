CMAKE_MINIMUM_REQUIRED(VERSION 3.16)
project(conway)

find_package(SDL2 REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_check_modules(GLIB REQUIRED glib-2.0)

file (GLOB_RECURSE Foo_SOURCES CONFIGURE_DEPENDS "src/*.c")
file (GLOB_RECURSE Foo_HEADERS CONFIGURE_DEPENDS "src/*.h")

set (Foo_INCLUDE_DIRS "")
foreach (_headerFile ${Foo_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND Foo_INCLUDE_DIRS ${_dir})
endforeach()
list (REMOVE_DUPLICATES Foo_INCLUDE_DIRS)

add_executable(conway ${Foo_SOURCES})

target_include_directories(conway PRIVATE ${SDL2_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})
target_link_libraries(conway ${SDL2_LIBRARIES} ${GLIB_LDFLAGS})