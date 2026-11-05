# ==========================================
# CONFIGURACIÓN MÍNIMA DE CMAKE (CAPA 2)
# ==========================================
cmake_minimum_required(VERSION 3.16)
project(HeadSoccerClash VERSION 1.0.0 LANGUAGES CXX)

# Usar el estándar C++17 de forma estricta
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Opciones del compilador (Warnings equivalentes a CXXFLAGS de Make)
add_compile_options(-Wall -Wextra -Wno-unused-parameter)

# Directorio de salida para los binarios compilados
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ==========================================
# DETECCIÓN DE CÓDIGO Y CABECERAS
# ==========================================
# Descubrimiento automático de archivos .cpp (equivalente a find en Make)
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp")

# Incluir cabeceras locales y carpetas de src/
include_directories(
    src
    src/engine
    src/engine/debug
    src/engine/physics
    vendor/include
)

# Ruta para buscar librerías locales
link_directories(vendor/lib)

# ==========================================
# CREACIÓN DEL EJECUTABLE Y ENLAZADO
# ==========================================
add_executable(game ${SOURCES})

# Enlazado de Raylib y librerías del sistema Linux (LDFLAGS de Make)
target_link_libraries(game PRIVATE
    raylib
    X11
    m
    pthread
    dl
    rt
)