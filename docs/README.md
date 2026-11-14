# Compilar y ejecutar el juego

Estas instrucciones son para Linux. Hace falta tener `g++` con soporte para C++17 y Raylib instalados.

Abre una terminal en la raíz del repositorio. CMake configura las cabeceras y la biblioteca de Raylib incluidas en `vendor/`:

```bash
cmake -S . -B build
cmake --build build
```

El ejecutable queda en `build/bin/game`. CMake también instala el juego, la fuente, el icono y el lanzador de escritorio:

```bash
cmake --install build
```

Para una compilación directa con Make:

```bash
make
./bin/game
```

Pulsa **Enter** para empezar una partida y cierra la ventana para salir.

Si modificas el código, vuelve a compilar antes de ejecutar para ver los cambios.
