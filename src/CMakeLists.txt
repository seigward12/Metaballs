target_sources(${PROJECT_NAME} PRIVATE
    MainScreen.cpp
    State.hpp
    StateManager.cpp
    Particle.cpp
    QuadTree.hpp
)

set(ASSETS_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/assets/)
configure_file(assets.hpp.in asssets.hpp)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")