cmake_minimum_required(VERSION 3.10)

# set(CMAKE_C_COMPILER clang)
# set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)

# set(CMAKE_C_COMPILER gcc-7)
# set(CMAKE_CXX_COMPILER g++-7)


set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(units)

file(WRITE "${PROJECT_BINARY_DIR}/.gitignore" "*")

include(ExternalProject)

ExternalProject_Add(gcem-external
	GIT_REPOSITORY "https://github.com/kthohr/gcem.git"
	GIT_TAG "b1406bc7509dbee72ad311a66717a6ae331107b0"
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
)

ExternalProject_Add(ctti-external
	GIT_REPOSITORY "https://github.com/Manu343726/ctti.git"
	GIT_TAG "d7e9828b82ce7a6321465fbd84f9bccb772c7f43"
	CONFIGURE_COMMAND ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ""
)

file(GLOB_RECURSE gcem_sources "${PROJECT_BINARY_DIR}/gcem-external-prefix/src/gcem-external/include/*.hpp")
add_library(gcem INTERFACE)
target_include_directories(gcem INTERFACE "${PROJECT_BINARY_DIR}/gcem-external-prefix/src/gcem-external/include")
target_sources(gcem INTERFACE ${gcem_sources})
add_dependencies(gcem gcem-external)

file(GLOB_RECURSE ctti_sources "${PROJECT_BINARY_DIR}/ctti-external-prefix/src/ctti-external/include/*.hpp")
add_library(ctti INTERFACE)
target_include_directories(ctti INTERFACE "${PROJECT_BINARY_DIR}/ctti-external-prefix/src/ctti-external/include")
target_sources(ctti INTERFACE ${ctti_sources})
add_dependencies(ctti ctti-external)

file(GLOB_RECURSE sources "src/*.c" "src/*.cpp" "src/*.h" "src/*.hpp")
add_executable(${PROJECT_NAME} ${sources})
target_include_directories(${PROJECT_NAME} PRIVATE "src")
target_link_libraries(${PROJECT_NAME} PRIVATE gcem ctti)

add_custom_target(run DEPENDS ${PROJECT_NAME} COMMAND $<TARGET_FILE:${PROJECT_NAME}>)