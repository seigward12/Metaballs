cmake_minimum_required(VERSION 3.16)
project(Metaballs LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

### adding dependencies
    # SFML
include(FetchContent)
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x)
set(SFML_BUILD_AUDIO FALSE)
set(SFML_BUILD_NETWORK FALSE)
FetchContent_MakeAvailable(SFML) 

    # ImGui
# FetchContent_Declare(
#     imgui
#     GIT_REPOSITORY https://github.com/ocornut/imgui.git
#     GIT_TAG v1.90.5)
# FetchContent_MakeAvailable(imgui)

#     # ImGui-SFML
# FetchContent_Declare(
#   imgui-sfml
#   GIT_REPOSITORY https://github.com/SFML/imgui-sfml
#   GIT_TAG 2.6.x)
# set(IMGUI_DIR ${imgui_SOURCE_DIR})
# set(IMGUI_SFML_FIND_SFML OFF)
# FetchContent_MakeAvailable(imgui-sfml)

    # SFGUI
FetchContent_Declare(
    SFGUI
    GIT_REPOSITORY https://github.com/TankOs/SFGUI.git
    GIT_TAG master)
set(SFGUI_BUILD_EXAMPLES OFF)
set(SFGUI_BUILD_SHARED_LIBS OFF)
set(SFML_STATIC_LIBRARIES OFF)
FetchContent_MakeAvailable(SFGUI)
###

### bulding the executable and adding the source files
# To add all the source files autmatically (Not recommended by Cmake)
# file(GLOB_RECURSE SOURCES_CPP CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
# file(GLOB_RECURSE SOURCES_HPP CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp")
# add_executable(${PROJECT_NAME} ${SOURCES_CPP} ${SOURCES_HPP})
add_executable(${PROJECT_NAME} src/main.cpp)
add_subdirectory(src)
###

# target_link_libraries(${PROJECT_NAME} PRIVATE ImGui-SFML::ImGui-SFML)
target_link_libraries(${PROJECT_NAME} PRIVATE 
    # sfml-audio 
    sfml-graphics 
    sfml-window 
    # sfml-network 
    sfml-system
)
target_link_libraries(${PROJECT_NAME} PRIVATE SFGUI::SFGUI)

if(WIN32)
    if(SFML_BUILD_AUDIO)
        add_custom_command(
            TARGET ${PROJECT_NAME}
            COMMENT "Copy OpenAL DLL"
            PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:${PROJECT_NAME}>
            VERBATIM)
    endif()
endif()


add_custom_command(
        TARGET ${PROJECT_NAME}
        COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        ${PROJECT_SOURCE_DIR}/src/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
        COMMENT "Copying assets into binary directory ${PROJECT_BINARY_DIR}"
)
install(DIRECTORY src/assets TYPE BIN)


install(TARGETS ${PROJECT_NAME})

include(InstallRequiredSystemLibraries)
include(CPack)
