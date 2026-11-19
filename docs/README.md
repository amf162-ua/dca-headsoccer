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

Para consultar las mediciones de compilación paralela (`-jN`) y ccache, mira
[COMPILACION.md](./COMPILACION.md).

## Compilar para web (WebAssembly con Emscripten)

Hace falta tener instalados CMake y el [SDK de Emscripten](https://emscripten.org/docs/getting_started/downloads.html):

```bash
git clone https://github.com/emscripten-core/emsdk.git ~/emsdk
~/emsdk/emsdk install latest
~/emsdk/emsdk activate latest
source ~/emsdk/emsdk_env.sh   # en cada terminal nueva (o añádelo a ~/.bashrc)
```

Desde la raíz del repositorio:

```bash
emcmake cmake -S . -B build-web -DCMAKE_BUILD_TYPE=Release
cmake --build build-web
```

La primera configuración descarga y compila Raylib para la plataforma Web, porque la `libraylib.a` de `vendor/lib` solo sirve para Linux. El resultado queda en `build-web/bin/`:

| Fichero | Contenido |
|---|---|
| `game.html` | Página que carga el juego |
| `game.js` | Código de arranque generado por Emscripten |
| `game.wasm` | El juego compilado a WebAssembly |
| `game.data` | La carpeta `assets/` empaquetada (fuentes, sprites) |

El navegador no permite cargar el `.wasm` abriendo el `.html` directamente desde el disco, así que hay que servirlo con un servidor local:

```bash
cd build-web/bin
python3 -m http.server 8000
```

Y abrir <http://localhost:8000/game.html>. Para subirlo a Itch.io, renombra `game.html` a `index.html` y comprime los cuatro ficheros en un `.zip`.
