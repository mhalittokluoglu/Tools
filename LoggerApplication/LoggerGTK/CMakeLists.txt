cmake_minimum_required(VERSION 3.16)

project (LoggerApp)
add_executable(${PROJECT_NAME} src/LoggerApplication.cpp
src/Handler/LoggerHandler.hpp
src/Handler/LoggerHandler.cpp
src/LogListener/LogListener.hpp
src/LogListener/LogListener.cpp
src/LogMessage/LogMessage.hpp
src/LogMessage/LogMessage.cpp
src/LogMessage/Filter/LogMessageFilter.hpp
src/LogMessage/Filter/LogMessageFilter.cpp
)
target_include_directories(${PROJECT_NAME} PUBLIC src)

set(CMAKE_EXE_LINKER_FLAGS "-Wl,-export-dynamic")
include(FindPkgConfig)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB2 glib-2.0)
pkg_check_modules(GTK3 gtk+-3.0)

target_link_libraries(${PROJECT_NAME} ${GLIB2_LIBRARIES} ${GTK3_LIBRARIES})
target_include_directories(${PROJECT_NAME} PUBLIC ${GLIB2_INCLUDE_DIRS} ${GTK3_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME} PUBLIC ${GLIB2_CFLAGS_OTHER} ${GTK3_CFLAGS_OTHER})

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)