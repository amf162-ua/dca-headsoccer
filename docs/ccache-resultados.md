# Resultados de ccache

Datos para las preguntas 3 y 4 de `COMPILACION.md` (Hito 2, Entregable 1).

## Entorno de medición

| Elemento | Valor |
|---|---|
| Sistema | Ubuntu en WSL 2 (repositorio clonado en `~`, fuera de OneDrive) |
| Compilador | g++ 15.2.0 |
| ccache | 4.12.3 |
| Trabajos en paralelo | `-j4` en todas las pruebas |
| Ficheros compilados | 12 `.cpp` (`src/` y `src/engine/...`) |

> **Nota:** el enlazado final falla con `cannot find -lraylib` porque `vendor/lib/libraylib.a` no está en el repositorio. No afecta a la medición: ccache solo actúa en la compilación (`.cpp` → `.o`), y los 12 objetos se generan correctamente en todos los casos.

## Procedimiento

```bash
# 1. Referencia sin ccache
make clean; time make USE_CCACHE=0 -j4

# 2. Vaciar la caché y poner las estadísticas a cero
ccache -C; ccache -z

# 3. Primera ejecución con ccache (caché vacía)
make clean; time make -j4; ccache -s

# 4. Segunda ejecución con ccache (caché llena)
make clean; time make -j4; ccache -s
```

`make clean` es necesario antes de cada ejecución: sin él, `make` vería los `.o` actualizados y no llamaría al compilador, así que ccache no llegaría a ejecutarse.

## Pregunta 3: tiempo de compilación con y sin ccache

| Ejecución | `real` | `user` | `sys` | Diferencia con la referencia |
|---|---|---|---|---|
| Sin ccache | 1,348 s | 2,702 s | 0,684 s | — |
| ccache, caché vacía | 1,823 s | 3,765 s | 0,960 s | **+35 %** (más lento) |
| ccache, caché llena | **0,422 s** | 0,042 s | 0,112 s | **−69 %** (≈3,2× más rápido) |

**Conclusiones:**

- **Con la caché vacía, ccache es más lento.** Además de compilar, calcula el hash de cada fichero y guarda el resultado en la caché. Es un coste que solo se paga la primera vez.
- **Con la caché llena, la compilación prácticamente desaparece.** El tiempo de CPU (`user`) baja de 2,7 s a 0,04 s, unas 64 veces menos. Los 0,42 s restantes son casi todo `make`, el arranque de procesos y el enlazado.
- **En proyectos grandes la mejora sería mucho mayor.** Como el proyecto es pequeño (12 ficheros), la diferencia absoluta es de solo ~1 s.

## Pregunta 4: estadísticas de ccache

### Tras la primera ejecución (caché vacía)

```
Cacheable calls:     12 /  13 (92.31%)
  Hits:               0 /  12 ( 0.00%)
    Direct:           0
    Preprocessed:     0
  Misses:            12 /  12 (100.0%)
Uncacheable calls:    1 /  13 ( 7.69%)
Local storage:
  Cache size (GiB): 0.0 / 5.0 ( 0.00%)
```

### Tras la segunda ejecución (acumulado de las dos)

```
Cacheable calls:     24 /  26 (92.31%)
  Hits:              12 /  24 (50.00%)
    Direct:          12 /  12 (100.0%)
    Preprocessed:     0 /  12 ( 0.00%)
  Misses:            12 /  24 (50.00%)
Uncacheable calls:    2 /  26 ( 7.69%)
Local storage:
  Cache size (GiB): 0.0 / 5.0 ( 0.00%)
```

### Interpretación

- **Las estadísticas son acumuladas** desde el `ccache -z`. Las 24 llamadas cacheables son 12 + 12, y el 50 % de hits significa que fallaron todas las de la primera ejecución y acertaron todas las de la segunda.
- **Llamadas cacheables (12 por ejecución):** las 12 compilaciones `g++ -c`, una por cada `.cpp`.
- **Llamadas no cacheables (1 por ejecución):** el **enlazado**. ccache no guarda enlazados, así que esa llamada pasa directamente a `g++`.
- **Primera ejecución: 12 misses.** La caché estaba vacía, así que se compiló todo y se guardó cada resultado.
- **Segunda ejecución: 12 hits, todos en modo Direct.** ccache tiene dos formas de buscar en la caché:
  - *Direct mode*: hash del `.cpp`, de las cabeceras incluidas y de las opciones del compilador, sin ejecutar el preprocesador. Es el más rápido y fue el que acertó en los 12 casos.
  - *Preprocessed mode*: ejecuta el preprocesador y calcula el hash de su salida. Solo se usa si el modo directo no encuentra nada; aquí no hizo falta.
- **Tamaño de caché:** los 12 objetos ocupan tan poco que se redondean a 0.0 GiB. El límite por defecto es 5 GiB.

## Idea clave

`make` evita recompilar lo que **no ha cambiado**, mirando las fechas de los ficheros. ccache evita recompilar lo que **ya se compiló alguna vez**, mirando el contenido. ccache aporta cuando `make` decide recompilar pero el resultado ya existe: después de un `make clean`, al cambiar de rama y volver, en otra copia del repositorio o en la integración continua.

## Limitaciones

- Cada caso se midió una sola vez. Para mayor rigor, repetir cada medición 3 veces y usar la media.
- Los tiempos en WSL pueden variar algo respecto a un Linux nativo, como los PCs de la facultad.
