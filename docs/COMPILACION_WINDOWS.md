# Compilar en Windows con CMake

Hace falta CMake, un compilador de C++17 y la biblioteca estatica de Raylib
para ese compilador. La `libraylib.a` de Linux no sirve para Windows.
Las cabeceras y la biblioteca deben ser de la misma version de Raylib.

Los ejemplos usan las cabeceras de `vendor/include`. Si las cabeceras de tu
Raylib estan en otra carpeta, añade `-DRAYLIB_INCLUDE_DIR=C:/raylib/include`
al primer comando.

## MinGW-w64

Abre una terminal con MinGW-w64 y `mingw32-make` en el PATH. Desde la raiz:

```bash
cmake -S . -B build-mingw -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DRAYLIB_LIBRARY=C:/raylib/lib/libraylib.a
cmake --build build-mingw --parallel 4
```

El ejecutable queda en `build-mingw/bin/game.exe`.
CMake enlaza estaticamente el runtime de MinGW, por lo que no hace falta copiar
`libgcc`, `libstdc++` ni `libwinpthread` junto al ejecutable.

## Visual Studio (MSVC)

Instala la carga de trabajo de desarrollo de escritorio con C++ y abre
Developer PowerShell de Visual Studio 2022. Usa la biblioteca `raylib.lib`
compilada para MSVC de 64 bits:

```powershell
cmake -S . -B build-msvc -G "Visual Studio 17 2022" -A x64 -DRAYLIB_LIBRARY=C:/raylib/lib/raylib.lib
cmake --build build-msvc --config Release --parallel 4
```

El ejecutable queda en `build-msvc/bin/Release/game.exe`.

La biblioteca debe coincidir con la arquitectura y el runtime de MSVC de la
compilacion. No mezcles una biblioteca de MinGW con MSVC ni al contrario.

## Recursos y opciones de compilacion

CMake copia `assets` junto al ejecutable. Conserva esa carpeta al mover el
juego. Si se proporciona una version dinamica de Raylib, su DLL debe estar en
el PATH o junto al `.exe`; los ejemplos anteriores usan la version estatica.

CMake detecta Windows con `WIN32` y enlaza las bibliotecas de Windows en lugar
de X11 y las bibliotecas de Linux. Para MSVC usa `/W4` y `/utf-8`; para MinGW,
las opciones de GCC. La configuracion de Emscripten se mantiene por separado.

Usa carpetas de compilacion distintas para cada compilador. Las carpetas
`build-mingw` y `build-msvc` se pueden borrar para empezar de nuevo.

Referencias: [Raylib en Windows](https://github.com/raysan5/raylib/wiki/Working-on-Windows),
[WIN32 en CMake](https://cmake.org/cmake/help/latest/variable/WIN32.html) y
[MSVC en CMake](https://cmake.org/cmake/help/latest/variable/MSVC.html).
