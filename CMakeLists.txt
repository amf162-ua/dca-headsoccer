# ==========================================
# CONFIGURACIÓN MÍNIMA DE CMAKE (CAPA 2)
# ==========================================
cmake_minimum_required(VERSION 3.16)
project(HeadSoccerClash VERSION 1.0.0 LANGUAGES CXX)
include(GNUInstallDirs)

# Usar el estándar C++17 de forma estricta
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Directorio de salida para los binarios compilados
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ==========================================
# DETECCIÓN DE CÓDIGO Y CABECERAS
# ==========================================
# Descubrimiento automático de archivos .cpp (equivalente a find en Make)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

if(EMSCRIPTEN)
    # ==========================================
    # RAYLIB PARA WEB (Emscripten)
    # ==========================================
    # La libraylib.a de vendor/lib es para Linux y no sirve en el navegador.
    # Se descarga y compila Raylib para la plataforma Web en la configuración.
    enable_language(C)
    include(FetchContent)
    set(PLATFORM Web CACHE STRING "Plataforma de Raylib" FORCE)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        6.2
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(raylib)
else()
    # Rutas de Raylib distribuida con el proyecto
    set(RAYLIB_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/vendor/include"
        CACHE PATH "Carpeta con las cabeceras de Raylib")

    # MSVC utiliza .lib; GCC y MinGW utilizan .a.
    # La biblioteca debe haberse compilado para el sistema y compilador elegidos.
    if(MSVC)
        set(RAYLIB_DEFAULT_LIBRARY "${PROJECT_SOURCE_DIR}/vendor/lib/raylib.lib")
    else()
        set(RAYLIB_DEFAULT_LIBRARY "${PROJECT_SOURCE_DIR}/vendor/lib/libraylib.a")
    endif()
    set(RAYLIB_LIBRARY "${RAYLIB_DEFAULT_LIBRARY}"
        CACHE FILEPATH "Biblioteca estatica de Raylib para la plataforma destino")

    if(NOT EXISTS "${RAYLIB_INCLUDE_DIR}/raylib.h")
        message(FATAL_ERROR "No se encuentra Raylib en ${RAYLIB_INCLUDE_DIR}")
    endif()

    if(NOT EXISTS "${RAYLIB_LIBRARY}")
        message(FATAL_ERROR
            "No se encuentra Raylib en ${RAYLIB_LIBRARY}. "
            "Indica su biblioteca con -DRAYLIB_LIBRARY=ruta/al/archivo")
    endif()
endif()

# ==========================================
# CREACIÓN DEL EJECUTABLE Y ENLAZADO
# ==========================================
add_executable(game ${SOURCES})

# Las opciones de GCC no son validas para el compilador de Visual Studio.
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

if(EMSCRIPTEN)
    # El target raylib ya aporta sus cabeceras y las opciones de enlazado web.
    target_link_libraries(game PRIVATE raylib)

    # Se genera game.html (+ game.js, game.wasm y game.data con los recursos).
    set_target_properties(game PROPERTIES SUFFIX ".html")
    target_link_options(game PRIVATE
        -sUSE_GLFW=3            # Ventana e input a través del port GLFW de Emscripten
        -sASYNCIFY              # Permite el bucle while de main.cpp sin bloquear el navegador
        -sALLOW_MEMORY_GROWTH=1 # La memoria crece si hace falta (fuentes, texturas)
        "--preload-file=${PROJECT_SOURCE_DIR}/assets@/assets" # Recursos en el sistema de ficheros virtual
        "--shell-file=${raylib_SOURCE_DIR}/src/minshell.html" # Página HTML mínima de Raylib
    )
elseif(WIN32)
    target_include_directories(game SYSTEM PRIVATE "${RAYLIB_INCLUDE_DIR}")

    # MinGW suele enlazar el runtime de GCC mediante DLL. Para que el juego se
    # pueda mover a otro PC sin copiar esas DLL, lo incluimos en el ejecutable.
    if(MINGW)
        target_link_options(game PRIVATE -static)
    endif()

    # Bibliotecas del sistema que necesita Raylib en Windows.
    # WIN32 tambien es verdadero al compilar para Windows de 64 bits.
    target_link_libraries(game PRIVATE
        "${RAYLIB_LIBRARY}"
        opengl32
        gdi32
        winmm
        user32
        shell32
    )
else()
    target_include_directories(game SYSTEM PRIVATE "${RAYLIB_INCLUDE_DIR}")

    # Enlazado de Raylib y librerías del sistema Linux (LDFLAGS de Make)
    target_link_libraries(game PRIVATE
        "${RAYLIB_LIBRARY}"
        X11
        m
        pthread
        dl
        rt
    )
endif()

install(TARGETS game RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(DIRECTORY assets/
    DESTINATION "${CMAKE_INSTALL_DATADIR}/head-soccer-clash/assets"
)
# El lanzador .desktop y el icono del menu son propios de la instalacion Linux.
if(UNIX AND NOT EMSCRIPTEN)
    install(FILES
        packaging/debian/usr/share/applications/head-soccer-clash.desktop
        DESTINATION "${CMAKE_INSTALL_DATADIR}/applications"
    )
    install(FILES
        packaging/debian/usr/share/icons/hicolor/scalable/apps/head-soccer-clash.svg
        DESTINATION "${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps"
    )
endif()
