cmake_minimum_required(VERSION 3.16)
project(Metaballs LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

### setting up interprocedural optimization not sure what it does
include(CheckIPOSupported)
check_ipo_supported(RESULT result)
if(result)
    message("setting interprocedural optimization")
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()
###


include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML) 


### bulding the executable and adding the source files
# To add all the source files autmatically (Not recommended by Cmake)
# file(GLOB_RECURSE SOURCES_CPP CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
# file(GLOB_RECURSE SOURCES_HPP CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
# add_executable(${PROJECT_NAME} ${SOURCES_CPP} ${SOURCES_HPP})
add_executable(${PROJECT_NAME} src/main.cpp)
add_subdirectory(src)
###

target_link_libraries(${PROJECT_NAME} PRIVATE 
    sfml-audio 
    sfml-graphics 
    sfml-window 
    sfml-network 
    sfml-system
)

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM)
endif()


# if(NOT CMAKE_BUILD_TYPE)
#   set(CMAKE_BUILD_TYPE Release)
# endif()
# file(COPY src/assets DESTINATION ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/src/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
        COMMENT "Copying assets into binary directory ${PROJECT_BINARY_DIR}"
        )

# add_dependencies(${PROJECT_NAME} copy_assets)

install(TARGETS ${PROJECT_NAME})
