#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define MAX_ITER 2048
#define NUM_RUNS 3

int mandelbrot(double real, double imag, int max_iter) {
    double z_real = 0, z_imag = 0;
    int iter = 0;
    while (z_real * z_real + z_imag * z_imag <= 4.0 && iter < max_iter) {
        double temp = z_real * z_real - z_imag * z_imag + real;
        z_imag = 2.0 * z_real * z_imag + imag;
        z_real = temp;
        iter++;
    }
    return iter;
}

double run_mandelbrot(int width, int height, int threads, int view, int save_image) {
    int *image = malloc(width * height * sizeof(int));
    double dx = 3.5 / width;
    double dy = 3.0 / height;
    double x0 = -2.5;
    double y0 = -1.5;

    if (view == 1) {
        x0 = -0.8;
        y0 = 0.156;
    }

    omp_set_num_threads(threads);

    double min_total_time = 1e30;
    double best_workload = 0.0;

    for (int r = 0; r < NUM_RUNS; r++) {
        double total_start = omp_get_wtime();
        double work_start = 1e30;
        double work_end = 0.0;

        #pragma omp parallel
        {
            double local_start = omp_get_wtime();

            #pragma omp for schedule(dynamic)
            for (int j = 0; j < height; j++) {
                for (int i = 0; i < width; i++) {
                    double real = x0 + i * dx;
                    double imag = y0 + j * dy;
                    image[j * width + i] = mandelbrot(real, imag, MAX_ITER);
                }
            }

            double local_end = omp_get_wtime();

            #pragma omp critical
            {
                if (local_start < work_start) work_start = local_start;
                if (local_end > work_end) work_end = local_end;
            }
        }

        double total_end = omp_get_wtime();


        double workload = work_end - work_start;
        double total = total_end - total_start;

        if (total < min_total_time) {
            min_total_time = total;
            best_workload = workload;
        }
    }

    if (save_image) {
        char filename[64];
        sprintf(filename, "mandelbrot_t%d_s%d_v%d.ppm", threads, width, view);
        FILE *f = fopen(filename, "w");
        fprintf(f, "P3\n%d %d\n255\n", width, height);
        for (int j = 0; j < height; j++) {
            for (int i = 0; i < width; i++) {
                int iter = image[j * width + i];
                int color = (int)(255.0 * iter / MAX_ITER);
                fprintf(f, "%d %d %d ", color, color, color);
            }
            fprintf(f, "\n");
        }
        fclose(f);
    }

    double total_ms = min_total_time * 1000;
    double workload_ms = best_workload * 1000;
    double overhead_ms = total_ms - workload_ms;

    // Terminal print (tabular row)
    printf("| %-8d | %-6d | %-4d | %-13.2f | %-16.2f | %-12.2f |\n",
           threads, width, view, total_ms, workload_ms, overhead_ms);

    // CSV-style output
    FILE *fp = fopen("mandelbrot_results.csv", "a");
    if (fp) {
        fprintf(fp, "%d,%d,%d,%.2f,%.2f,%.2f\n",
                threads, width, view, total_ms, workload_ms, overhead_ms);
        fclose(fp);
    }

    free(image);
    return total_ms;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <threads> <size> <view> [save_image 0|1]\n", argv[0]);
        return 1;
    }

    int threads = atoi(argv[1]);
    int size = atoi(argv[2]);
    int view = atoi(argv[3]);
    int save_image = (argc > 4) ? atoi(argv[4]) : 0;

    run_mandelbrot(size, size, threads, view, save_image);
    return 0;
}
