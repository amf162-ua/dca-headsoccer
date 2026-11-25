# Optimización de la compilación

## Entorno y método de prueba

Las mediciones de este informe se realizaron el 14 de noviembre de 2026 sobre
el estado del proyecto en esa fecha.

Los resultados de `-jN` actualizados al 25 de noviembre, con los 14 archivos
fuente actuales, estan en [BENCHMARKS_PARALELO.md](./BENCHMARKS_PARALELO.md).
Los datos de ccache de este documento se conservan como referencia.

Entorno de las mediciones del 14 de noviembre:

| Elemento | Valor |
|---|---|
| Sistema | Debian, entorno virtualizado |
| Compilador | g++ 14.2.0 |
| ccache | 4.11.2 |
| Archivos fuente | 13 archivos `.cpp` |
| Biblioteca | Raylib estática de `vendor/lib/` |
| CPU reportada por el entorno | 1 procesador lógico |

Cada compilación empezó con `make clean` y se midió el tiempo de pared del
build completo, incluido el enlazado. Se hicieron tres ejecuciones para cada
valor de `-j`; el orden se intercaló para reducir el efecto de medir siempre
un mismo valor primero o último. Para ccache se hicieron tres ejecuciones con
la caché vacía y tres con la caché ya poblada. Se comparan las medianas, menos
sensibles a una ejecución especialmente lenta.

## 1. ¿Qué hace `make -jN` y cuánto reduce el tiempo?

`-jN` permite que Make ejecute hasta `N` tareas en paralelo. Así, mientras un
archivo se compila, Make puede compilar otros archivos que no dependan de él.
`-j1` es equivalente a compilar una tarea cada vez.

Resultados sin ccache:

| Opción | Tiempos de las 3 ejecuciones | Mediana | Cambio frente a `-j1` |
|---|---|---:|---:|
| `-j1` | 7,882 s; 7,368 s; 12,925 s | 7,882 s | — |
| `-j2` | 5,743 s; 5,235 s; 5,644 s | 5,644 s | 28 % menos tiempo |
| `-j4` | 4,466 s; 4,385 s; 4,265 s | 4,385 s | 44 % menos tiempo |
| `-j8` | 4,126 s; 3,765 s; 3,687 s | 3,765 s | 52 % menos tiempo |

En esta prueba, `-j8` fue el más rápido. El beneficio no crece necesariamente
en proporción al número de tareas: hay pasos que no se pueden paralelizar y
el sistema puede limitar CPU o memoria.

## 2. ¿Qué valor de `N` conviene usar?

Como punto de partida, se puede usar el número de procesadores lógicos que
informa el sistema:

```bash
make -j"$(nproc)"
```

También se puede probar un valor fijo, por ejemplo `-j4`. Conviene medir en la
máquina de destino: demasiadas tareas simultáneas pueden aumentar el uso de
memoria, provocar paginación o incluso hacer que la compilación falle por falta
de recursos.

En este entorno se reportó un procesador lógico, pero las ejecuciones con
`-j2`, `-j4` y `-j8` fueron progresivamente más rápidas. Esto muestra que el
resultado depende de la virtualización y de las condiciones de la máquina; no
se debe interpretar esta medición como una recomendación universal de usar
`-j8`.

Para repetir la comparación:

```bash
make clean && /usr/bin/time -p make USE_CCACHE=0 -j1
make clean && /usr/bin/time -p make USE_CCACHE=0 -j2
make clean && /usr/bin/time -p make USE_CCACHE=0 -j4
make clean && /usr/bin/time -p make USE_CCACHE=0 -j8
```

## 3. ¿Qué aporta ccache y qué ocurre en la primera y segunda compilación?

ccache guarda los resultados de compilaciones anteriores. Si se recompila un
archivo sin cambiar el contenido, las cabeceras ni las opciones del
compilador, puede recuperar el objeto de la caché en vez de volver a ejecutar
g++. Por eso `make clean` es importante en esta prueba: obliga a Make a
solicitar de nuevo los objetos y permite comprobar los aciertos de ccache.

Comparación con `-j4`:

| Caso | Tiempos de las 3 ejecuciones | Mediana |
|---|---|---:|
| Sin ccache | 4,466 s; 4,385 s; 4,265 s | 4,385 s |
| ccache, caché vacía | 5,646 s; 6,197 s; 7,313 s | 6,197 s |
| ccache, caché poblada | 0,512 s; 0,482 s; 0,533 s | 0,512 s |

La primera compilación con ccache tardó un 41 % más que la referencia sin
ccache: además de compilar, tuvo que calcular las claves y guardar los objetos.
Con la caché poblada tardó un 88 % menos que la referencia (unas 8,6 veces
menos tiempo). En este proyecto pequeño, la diferencia absoluta de la primera
compilación es limitada; la ventaja principal aparece al recompilar.

Comandos para repetir la prueba en una máquina con ccache instalado:

```bash
ccache -C
ccache -z
make clean
/usr/bin/time -p make -j4
ccache -s

make clean
/usr/bin/time -p make -j4
ccache -s
```

El `Makefile` activa ccache por defecto si está instalado. Para desactivarlo
explícitamente se puede usar `USE_CCACHE=0`.

## 4. Comprobación que ccache está funcionando

`ccache -s` muestra las llamadas cacheables, los aciertos (*hits*) y los fallos
(*misses*). En las tres ejecuciones frías se registraron 39 fallos cacheables
(13 por build) y 3 llamadas no cacheables (el enlazado, una por build). Tras
las tres ejecuciones calientes, las estadísticas acumuladas fueron:

```text
Cacheable calls:     78
  Hits:              39 (50.00%)
    Direct:          39 (100.00%)
    Preprocessed:     0
  Misses:            39 (50.00%)
Uncacheable calls:    6
```

El 50 % de aciertos corresponde a las 39 compilaciones de las ejecuciones
calientes; las otras 39 llamadas fueron los fallos de las ejecuciones frías.
Las seis llamadas no cacheables corresponden al enlazado, que no reutiliza
ccache. Las estadísticas son acumulativas desde el último `ccache -z`.

## Conclusión

`-jN` reduce el tiempo de una compilación completa al repartir tareas
independientes. ccache reduce sobre todo el tiempo de recompilaciones cuando
el resultado de los archivos fuente ya está guardado. Son optimizaciones
complementarias: se pueden combinar `make -jN` y ccache.
