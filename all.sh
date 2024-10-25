#!/usr/bin/env bash
#
# Este script se encarga de invocar los tres programas que permiten la 
# conversion de un PNG a secuencia de pixeles, se procesan esos pixeles y
# posteriormente se convierte esa secuencia de pixeles a un archivo en formato
# PNG
#

# Autor: John Sanabria - john.sanabria@correounivalle.edu.co
# Fecha: 2024-08-22

# Ruta a la carpeta de imágenes
image_dir="./imagenes"

# Medir tiempo total de procesamiento de todas las imágenes
time {
  # Recorrer todos los archivos PNG en la carpeta "imagenes"
  for INPUT_PNG in "$image_dir"/*.png; do
    echo "Procesando imagen: $INPUT_PNG"

    # Definir un nombre temporal para el archivo binario
    TEMP_FILE="${INPUT_PNG%.*}.bin"

    # Convertir de PNG a BIN
    python3 fromPNG2Bin.py "$INPUT_PNG"
    
    # Ejecutar el programa principal con el archivo binario
    ./main "$TEMP_FILE"

    # Convertir de BIN a PNG
    python3 fromBin2PNG.py "${TEMP_FILE}.new"
    
    echo "Procesamiento completado para: $INPUT_PNG"
  done
}
