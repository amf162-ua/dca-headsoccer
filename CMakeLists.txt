cmake_minimum_required(VERSION 3.16)
project(HeadSoccerClash VERSION 1.0.0 LANGUAGES CXX C)
include(GNUInstallDirs)

# Usar el estándar C++17 de forma estricta
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Directorio de salida para los binarios compilados
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ==========================================
# DEPENDENCIAS EXTERNAS (Raylib desde GitHub)
# ==========================================
include(cmake/Dependencies.cmake)

# ==========================================
# DETECCIÓN DE CÓDIGO Y CABECERAS
# ==========================================
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# ==========================================
# CREACIÓN DEL EJECUTABLE Y ENLAZADO
# ==========================================
add_executable(game ${SOURCES})

if(MSVC)
    target_compile_options(game PRIVATE /W4 /utf-8)
else()
    target_compile_options(game PRIVATE -Wall -Wextra -Wno-unused-parameter)
endif()

target_include_directories(game PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/src/engine"
    "${PROJECT_SOURCE_DIR}/src/engine/debug"
    "${PROJECT_SOURCE_DIR}/src/engine/physics"
)

target_compile_definitions(game PRIVATE
    GAME_INSTALL_DATA_DIR="${CMAKE_INSTALL_FULL_DATADIR}/head-soccer-clash"
)

add_custom_command(TARGET game POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:game>/assets"
)

# Enlazado unificado: Raylib aporta sus cabeceras y librerías del sistema
target_link_libraries(game PRIVATE raylib)

if(EMSCRIPTEN)
    set_target_properties(game PROPERTIES SUFFIX ".html")
    target_link_options(game PRIVATE
        -sUSE_GLFW=3
        -sASYNCIFY
        -sALLOW_MEMORY_GROWTH=1
        "--preload-file=${PROJECT_SOURCE_DIR}/assets@/assets"
        "--shell-file=${raylib_SOURCE_DIR}/src/minshell.html"
    )
elseif(MINGW)
    target_link_options(game PRIVATE -static)
endif()

# ==========================================
# REGLAS DE INSTALACIÓN (para .deb y make install)
# ==========================================
install(TARGETS game RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(DIRECTORY assets/
    DESTINATION "${CMAKE_INSTALL_DATADIR}/head-soccer-clash/assets"
)

if(UNIX AND NOT EMSCRIPTEN)
    install(FILES
        packaging/head-soccer-clash.desktop
        DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
    )
    install(FILES
        packaging/head-soccer-clash.svg
        DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps"
    )
endif()