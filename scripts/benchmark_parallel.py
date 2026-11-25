"""Mide compilaciones limpias con varios valores de make -jN."""

import argparse
import csv
import hashlib
import os
from pathlib import Path
import platform
import statistics
import subprocess
import time


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repetitions", type=int, default=3)
    parser.add_argument("--raylib-lib-dir", default="vendor/lib")
    parser.add_argument("--output", type=Path, default=Path("/tmp/headsoccer-benchmarks"))
    args = parser.parse_args()
    if args.repetitions < 1:
        parser.error("El numero de repeticiones debe ser mayor que cero")

    root = Path(__file__).resolve().parent.parent
    output = args.output.resolve()
    # make clean borra build y bin; los resultados deben guardarse fuera.
    for directory in [root / "build", root / "bin"]:
        if output == directory or directory in output.parents:
            parser.error("Guarda los resultados fuera de build y bin")
    output.mkdir(parents=True, exist_ok=True)

    sources = sorted((root / "src").rglob("*.cpp"))
    options = ["USE_CCACHE=0", "RAYLIB_LIB_DIR=" + args.raylib_lib_dir]
    rows = []
    reference_hash = None

    # Intercalamos los valores para no medir siempre primero el mismo grupo.
    for repetition in range(1, args.repetitions + 1):
        for jobs in [1, 2, 4, 8]:
            subprocess.run(["make", "clean"], cwd=root, check=True, stdout=subprocess.DEVNULL)
            log_path = output / ("j" + str(jobs) + "-" + str(repetition) + ".log")
            with log_path.open("w") as log:
                start = time.perf_counter()
                subprocess.run(["make", *options, "-j" + str(jobs)], cwd=root,
                               stdout=log, stderr=subprocess.STDOUT, check=True)
                elapsed = time.perf_counter() - start

            # Cada fuente debe tener su objeto y su fichero de dependencias.
            for source in sources:
                relative = source.relative_to(root / "src")
                for suffix in [".o", ".d"]:
                    path = root / "build/obj" / relative.with_suffix(suffix)
                    if not path.is_file():
                        raise RuntimeError("Falta un resultado de compilacion: " + str(path))

            # Make debe considerar terminado el trabajo, sin tareas pendientes.
            subprocess.run(["make", *options, "-q", "bin/game"], cwd=root, check=True)
            digest = hashlib.sha256((root / "bin/game").read_bytes()).hexdigest()
            if reference_hash is None:
                reference_hash = digest
            if digest != reference_hash:
                raise RuntimeError("El ejecutable cambia entre compilaciones; revisa los logs")

            rows.append([repetition, jobs, round(elapsed, 6), len(sources), digest])
            print("Pasada " + str(repetition) + ", -j" + str(jobs) + ": "
                  + format(elapsed, ".3f") + " s; objetos, dependencias y hash correctos", flush=True)

    with (output / "results.csv").open("w", newline="") as result_file:
        writer = csv.writer(result_file, lineterminator="\n")
        writer.writerow(["repetition", "jobs", "seconds", "sources", "sha256"])
        writer.writerows(rows)

    compiler = subprocess.check_output(["g++", "--version"], text=True).splitlines()[0]
    processors = subprocess.check_output(["nproc"], text=True).strip()
    metadata = [platform.platform(), compiler, "nproc: " + processors,
                "CPU logicas reportadas por Python: " + str(os.cpu_count()),
                "Fuentes: " + str(len(sources)), "ccache: desactivada"]
    (output / "environment.txt").write_text("\n".join(metadata) + "\n")
    for jobs in [1, 2, 4, 8]:
        times = [row[2] for row in rows if row[1] == jobs]
        print("Mediana -j" + str(jobs) + ": " + format(statistics.median(times), ".3f") + " s")
    print("Resultados y logs: " + str(output))


if __name__ == "__main__":
    main()
