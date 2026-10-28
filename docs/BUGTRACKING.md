# Justificación de elección de bugtracking

## 1. Justificación

En la clase de teoría decidimos elegir los Issues de GitHub como sistema de bugtracking, por la facilidad de que lo tenemos en el repositorio y está todo más centralizado. Nos interesó mucho la automatización de los Issues con  Jev y los workflows, por ese motivo nos pusimos el reto de hacer esa automatización.

## 2. Como lo hemos hecho

Hemos creado una carpeta llamada "scripts" donde se almacena el archivo **triage.py** donde tenemos el código para la automatización del triaje del bug. Luego en la .github/workflows/ tenemos el archivo .yml donde hacemos que en GitHub se ejecute el .py cuando se cree un Issue como bug y hace toda la automatización del triaje.