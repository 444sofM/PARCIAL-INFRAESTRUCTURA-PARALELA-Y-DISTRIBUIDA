#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

// Funciones placeholder para la carga y guardado de imágenes
void cargarImagen(int *imagen, int width, int height);
void guardarImagen(int *imagen, int width, int height);

// Función para aplicar un filtro Sobel
void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height);

// Función para calcular la suma de los píxeles (como una estadística)
int calcularSumaPixeles(int *imagen, int width, int height);

char *filename;

int main(int argc, char* argv[]) {
    int width = 1024, height = 1024;
    int *imagen = (int *)malloc(width * height * sizeof(int));
    int *imagenProcesada = (int *)malloc(width * height * sizeof(int));

    if (argc != 2) {
        fprintf(stderr, "Dar un nombre de archivo de entrada\n");
        exit(1);
    }

    filename = argv[1];

    double start, end;

    // Medir tiempo de carga de imagen (no paralelizable)
    start = omp_get_wtime();
    cargarImagen(imagen, width, height);
    end = omp_get_wtime();
    printf("Tiempo de carga de imagen: %f segundos\n", end - start);

    // Medir tiempo de aplicación del filtro (paralelizable)
    start = omp_get_wtime();
    aplicarFiltro(imagen, imagenProcesada, width, height);
    end = omp_get_wtime();
    printf("Tiempo de aplicación del filtro: %f segundos\n", end - start);

    // Medir tiempo de cálculo de suma de píxeles (paralelizable)
    start = omp_get_wtime();
    int sumaPixeles = calcularSumaPixeles(imagenProcesada, width, height);
    end = omp_get_wtime();
    printf("Tiempo de cálculo de suma de píxeles: %f segundos\n", end - start);

    printf("Suma de píxeles: %d\n", sumaPixeles);

    // Guardar la imagen (no paralelizable)
    start = omp_get_wtime();
    guardarImagen(imagenProcesada, width, height);
    end = omp_get_wtime();
    printf("Tiempo de guardado de imagen: %f segundos\n", end - start);

    free(imagen);
    free(imagenProcesada);
    return 0;
}

// Carga una imagen desde un archivo binario
void cargarImagen(int *imagen, int width, int height) {
    FILE *archivo = fopen(filename, "rb");
    if (archivo == NULL) {
        perror("Error al abrir el archivo para cargar la imagen");
        return;
    }

    size_t elementosLeidos = fread(imagen, sizeof(int), width * height, archivo);
    if (elementosLeidos != width * height) {
        perror("Error al leer la imagen desde el archivo");
    }

    fclose(archivo);
}

// Guarda una imagen en un archivo binario
void guardarImagen(int *imagen, int width, int height) {
    char *output_filename;

    output_filename = (char*)malloc(sizeof(char) * (strlen(filename) + 4));
    sprintf(output_filename, "%s.new", filename);
    FILE *archivo = fopen(output_filename, "wb");
    if (archivo == NULL) {
        perror("Error al abrir el archivo para guardar la imagen");
        return;
    }

    size_t elementosEscritos = fwrite(imagen, sizeof(int), width * height, archivo);
    if (elementosEscritos != width * height) {
        perror("Error al escribir la imagen en el archivo");
    }

    fclose(archivo);
}

// Función para aplicar el filtro Sobel (paralelizable con OpenMP)
void aplicarFiltro(int *imagen, int *imagenProcesada, int width, int height) {
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    #pragma omp parallel for collapse(2)
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sumX = 0;
            int sumY = 0;

            // Aplicar máscaras de Sobel (Gx y Gy)
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sumX += imagen[(y + ky) * width + (x + kx)] * Gx[ky + 1][kx + 1];
                    sumY += imagen[(y + ky) * width + (x + kx)] * Gy[ky + 1][kx + 1];
                }
            }

            // Calcular magnitud del gradiente
            int magnitude = abs(sumX) + abs(sumY);
            imagenProcesada[y * width + x] = (magnitude > 255) ? 255 : magnitude; // Normalizar a 8 bits
        }
    }
}

// Función para calcular la suma de los píxeles (paralelizable con OpenMP)
int calcularSumaPixeles(int *imagen, int width, int height) {
    int suma = 0;

    #pragma omp parallel for reduction(+:suma)
    for (int i = 0; i < width * height; i++) {
        suma += imagen[i];
    }
    return suma;
}

