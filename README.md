## Integrantes 
-Antonella Pincheira Poblete
-Benjamin Stuckrath Bustamante
## Comandos 
### Comando de compilación
g++ main.cpp -o xmlTree
### Comando de ejecución
./xmlTree.exe
## Disclaimer
Los archivos XML deben estar guardados en una carpeta llamada 'XMLs' junto al ejecutable. El programa generará el arbol a partir de todos los archivos que se encuentren en este
### Funciones relevantes
Tree t = Tree(): Crea un objeto de la clase Tree, que genera todos sus nodos a partir de los archivos XML en el directorio 'XMLs' junto al ejecutable
t.listar(), t.precursores() y t.borrar_ratings(float) realizan lo solicitado de acuerdo a la tarea de Canvas
