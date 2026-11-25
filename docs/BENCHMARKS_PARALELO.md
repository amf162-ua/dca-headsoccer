# Compilacion paralela con Make

Mediciones del 25 de noviembre de 2026 sobre el codigo con los sprites de los
jugadores ya integrados. Los datos de ccache del otro
informe no se han modificado.

## Entorno y metodo

| Elemento | Valor |
|---|---|
| Sistema | Ubuntu sobre WSL2, x86_64 |
| Compilador | g++ 13.3.0 |
| Procesadores logicos (`nproc`) | 8 |
| Archivos `.cpp` | 14 |
| Raylib | Biblioteca estatica de `/usr/local/lib` |
| ccache | Desactivada con `USE_CCACHE=0` |

Se hicieron tres pasadas de `-j1`, `-j2`, `-j4` y `-j8`, intercalando los valores.
Antes de cada compilacion se ejecuto `make clean`. El tiempo medido incluye
Make, las compilaciones y el enlazado, pero no la limpieza ni las comprobaciones
posteriores. Se usaron las opciones normales del Makefile, sin añadir `-O`.

## Tiempos

| Opcion | Pasada 1 | Pasada 2 | Pasada 3 | Mediana | Reduccion frente a `-j1` |
|---|---:|---:|---:|---:|---:|
| `-j1` | 3,397 s | 3,196 s | 3,219 s | 3,219 s | — |
| `-j2` | 2,117 s | 2,270 s | 2,273 s | 2,270 s | 29,5 % |
| `-j4` | 1,769 s | 1,828 s | 1,834 s | 1,828 s | 43,2 % |
| `-j8` | 1,655 s | 1,587 s | 1,542 s | 1,587 s | 50,7 % |

En esta maquina, `-j8` ha sido la opcion mas rapida. La mejora no es proporcional
al numero de tareas porque tambien hay trabajo que se hace una sola vez, como
el enlazado. En otros equipos los tiempos pueden cambiar.

Los tiempos sin redondear y el hash de cada ejecutable estan en
[benchmarks-paralelo.csv](./benchmarks-paralelo.csv).

## Comprobacion de condiciones de carrera

Las doce compilaciones terminaron correctamente. Despues de cada una se comprobo:

- Que cada fuente tuviera su archivo `.o` y su archivo `.d`.
- Que `make -q bin/game` no encontrara trabajo pendiente.
- Que el ejecutable tuviera el mismo SHA-256 que en la primera compilacion.

Los doce hashes fueron identicos:

```text
e8ba4aebb14d8bd04060323c55944c43e5d8aa4de3a014764e77f771e226b0f1
```

No se observaron fallos por condiciones de carrera en estas ejecuciones. La
regla de enlazado depende de todos los objetos y cada compilacion crea su
carpeta con `mkdir -p`, que permite que varias tareas creen el mismo directorio.
Tambien se probo `make -j8 all dist`: la compilacion normal y la del paquete
usan carpetas distintas y terminaron correctamente.

## Repetir la prueba

Desde la raiz del repositorio:

```bash
python3 scripts/benchmark_parallel.py --raylib-lib-dir /usr/local/lib
```

Si la biblioteca esta en `vendor/lib`, se puede omitir esa opcion. El script
guarda un CSV, los datos del entorno y los doce logs en
`/tmp/headsoccer-benchmarks`. Se puede elegir otra carpeta con `--output`.
No debe ejecutarse a la vez que otro Make en la misma carpeta, ya que utiliza
`make clean` antes de cada medida.
