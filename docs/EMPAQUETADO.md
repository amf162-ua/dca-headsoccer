# Crear el paquete Debian

Desde la raiz del repositorio, con `g++`, Make, Raylib y `dpkg-deb` disponibles:

```bash
make dist
```

El resultado queda en `dist/head-soccer-clash_1.0.0_amd64.deb`. El nombre se
obtiene del archivo `packaging/debian/DEBIAN/control`.

Si Raylib esta instalada en otra carpeta, indica donde esta su biblioteca:

```bash
make dist RAYLIB_LIB_DIR=/usr/local/lib
```

La regla compila en `build/deb-build` y utiliza `make install` con una carpeta
temporal, `build/deb-root`. No instala archivos en el sistema. Dentro del paquete,
el ejecutable queda en `/usr/bin/game` y los recursos en
`/usr/share/head-soccer-clash/assets`. Tambien incluye el icono y el lanzador.

La compilacion del paquete esta separada de la normal porque sus rutas de
instalacion son distintas. Se puede ejecutar con `make -j8 dist`; las reglas
`install` y `uninstall` siguen disponibles por separado.

Para revisar el paquete antes de instalarlo:

```bash
dpkg-deb --info dist/head-soccer-clash_1.0.0_amd64.deb
dpkg-deb --contents dist/head-soccer-clash_1.0.0_amd64.deb
```

El paquete declara las bibliotecas de ejecucion de X11, OpenGL, C y C++. No depende de
un paquete de Raylib porque se enlaza con su biblioteca estatica.

La opcion `--root-owner-group` permite crear el paquete sin usar `sudo`.
Referencia: [manual de dpkg-deb](https://manpages.debian.org/bookworm/dpkg/dpkg-deb.1.en.html).
