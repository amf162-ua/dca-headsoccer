# Compilar y ejecutar el juego

Estas instrucciones son para Linux. Hace falta tener `g++` con soporte para C++17 y Raylib instalados.

Abre una terminal en la raíz del repositorio, donde están las carpetas `src`, `vendor` y `docs`, y compila con:

```bash
g++ -std=c++17 -Isrc -Ivendor/include src/*.cpp src/physics/*.cpp src/debug/*.cpp -L/usr/local/lib -lraylib -lX11 -lm -lpthread -ldl -lrt -o game
```

Para ejecutar el juego:

```bash
./game
```

Pulsa **Enter** para empezar una partida y **Esc** para salir.

Si modificas el código, vuelve a compilar antes de ejecutar para ver los cambios.
