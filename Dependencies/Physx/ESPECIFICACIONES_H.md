**ESCOGER BIBLIOTECAS:**
- Se puede modificar las bibliotecas que se quieran tener de PhysX en *physx/source/compiler/cmake/windows/CMakeLists.txt* en la funcion **INSTALL** abajo del archivo. 

**MODIFICAR DESTINO COMPILACION:**
- Se puede modificar en *physx/source/compiler/cmake/modules/NvidiaBuildOptions.cmake* donde se definen **PX_ROOT_LIB_DIR** y **PX_ROOT_EXE_DIR**. 

**PROBLEMAS DE IMPLEMENTACION (compile_physx.bat) :**
- Se compilan todas las librerias de PhysX5, sin embargo, estas se mueven a *Dependencies/(Debug o Release)* y luego se eliminan las librerias restantes en el código fuente de PhysX5. Esto es debido a que al compilar la solucion se compilan las librerias y luego se crea un copia (instalación). Donde te incluye los 'includes' necesarios.  