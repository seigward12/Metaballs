cmake_minimum_required(VERSION 3.16)
project(Metaballs LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
FetchContent_MakeAvailable(SFML)


file(GLOB_RECURSE SOURCES_CPP RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES_CPP})


target_link_libraries(${PROJECT_NAME} PRIVATE 
    sfml-audio 
    sfml-graphics 
    sfml-window 
    sfml-network 
    sfml-system
)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM)
endif()


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
file(COPY src/assets DESTINATION ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})

install(TARGETS ${PROJECT_NAME})
