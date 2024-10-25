#!/usr/bin/env bash
#
# Este script se encarga de medir los tiempos de ejecución del programa en paralelo.
#

# Ruta a la carpeta de imágenes
image_dir="./imagenes"

# Detectar el número de núcleos físicos de la máquina
NUM_CORES=$(lscpu | grep '^CPU(s):' | awk '{print $2}')

# Configuración de hilos para la prueba
thread_configs=($NUM_CORES $((NUM_CORES * 2)))

# Función para correr las pruebas
run_tests() {
  local num_threads=$1
  local label=$2
  local tiempos=()

  echo "Ejecutando pruebas con $num_threads hilos ($label)"

  # Ejecutar el programa 5 veces
  for i in {1..5}; do
    echo "Prueba $i con $num_threads hilos..."
    start_time=$(date +%s.%N)
    
    # Recorrer todos los archivos PNG en la carpeta "imagenes"
    for INPUT_PNG in "$image_dir"/*.png; do
      echo "Procesando imagen: $INPUT_PNG con $num_threads hilos"
      
      # Definir un nombre temporal para el archivo binario
      TEMP_FILE="${INPUT_PNG%.*}.bin"
      
      # Convertir de PNG a BIN
      python3 fromPNG2Bin.py "$INPUT_PNG"
      
      # Ejecutar el programa principal con el archivo binario y la cantidad de hilos deseada
      ./main "$TEMP_FILE" $num_threads

      # Convertir de BIN a PNG
      python3 fromBin2PNG.py "${TEMP_FILE}.new"
      
      echo "Procesamiento completado para: $INPUT_PNG"
    done

    # Tomar el tiempo de finalización
    end_time=$(date +%s.%N)
    
    # Calcular el tiempo de ejecución y almacenarlo
    elapsed_time=$(echo "$end_time - $start_time" | bc)
    tiempos+=($elapsed_time)
    echo "Tiempo de ejecución para prueba $i: $elapsed_time segundos"
  done

  # Ordenar los tiempos y eliminar el más alto y el más bajo
  sorted_times=($(printf '%s\n' "${tiempos[@]}" | sort -n))
  unset sorted_times[0]  # Eliminar el menor tiempo
  unset sorted_times[${#sorted_times[@]}-1]  # Eliminar el mayor tiempo

  # Calcular el promedio de los tiempos restantes
  total_time=0
  for t in "${sorted_times[@]}"; do
    total_time=$(echo "$total_time + $t" | bc)
  done
  average_time=$(echo "$total_time / ${#sorted_times[@]}" | bc -l)

  # Imprimir el promedio de los tiempos
  echo "Tiempo promedio para $label ($num_threads hilos): $average_time segundos"
}

# Correr las pruebas para ambas configuraciones de hilos
for num_threads in "${thread_configs[@]}"; do
  if [ $num_threads -eq $NUM_CORES ]; then
    run_tests $num_threads "Número de hilos igual al número de núcleos"
  else
    run_tests $num_threads "Número de hilos igual al doble del número de núcleos"
  fi
donet 
