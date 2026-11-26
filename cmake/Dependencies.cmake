include(FetchContent)

# Desactivar ejemplos, juegos y pruebas internas de Raylib para compilar más rápido
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)

# Configurar la plataforma de Raylib según el destino
if(EMSCRIPTEN)
    set(PLATFORM "Web" CACHE STRING "" FORCE)
else()
    set(PLATFORM "Desktop" CACHE STRING "" FORCE)
endif()

# Declarar y descargar Raylib desde su repositorio oficial en GitHub
FetchContent_Declare(
    raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG        5.5
    GIT_SHALLOW    TRUE
    EXCLUDE_FROM_ALL
)

FetchContent_MakeAvailable(raylib)