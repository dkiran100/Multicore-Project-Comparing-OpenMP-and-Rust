#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define INF 1000000000

void floyd_warshall(int n, int *dist) {
    #pragma omp parallel
    {
        for (int k = 0; k < n; ++k) {
            #pragma omp for schedule(static)
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    int dik = dist[i * n + k];
                    int dkj = dist[k * n + j];
                    if (dik != INF && dkj != INF) {
                        int new_dist = dik + dkj;
                        if (new_dist < dist[i * n + j]) {
                            dist[i * n + j] = new_dist;
                        }
                    }
                }
            }
        }
    }
}

void generate_random_graph(int n, int *dist, float density) {
    srand(time(NULL));

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            dist[i * n + j] = (i == j) ? 0 : INF;

    int max_edges = n * (n - 1);
    int edge_count = (int)(density * max_edges);

    for (int k = 0; k < edge_count; ++k) {
        int u = rand() % n;
        int v = rand() % n;
        if (u != v) {
            int w = rand() % 100 + 1;
            if (w < dist[u * n + v]) {
                dist[u * n + v] = w;
            }
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <nodes> <threads> <density (0-1)>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    int num_threads = atoi(argv[2]);
    float density = atof(argv[3]);

    if (n <= 0 || num_threads <= 0 || density < 0 || density > 1) {
        fprintf(stderr, "Invalid parameters.\n");
        return EXIT_FAILURE;
    }

    omp_set_num_threads(num_threads);

    int *dist = (int *)malloc(n * n * sizeof(int));
    if (!dist) {
        fprintf(stderr, "Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    generate_random_graph(n, dist, density);

    double workload_time = 0.0;
    double total_start = omp_get_wtime();

    double work_start = omp_get_wtime();
    floyd_warshall(n, dist);
    double work_end = omp_get_wtime();

    double total_end = omp_get_wtime();
    workload_time = work_end - work_start;
    double total_time = total_end - total_start;
    double overhead = total_time - workload_time;


    // Print tabular row for the current configuration
    printf("| %-6d | %-7d | %-7.2f | %-12.6f | %-14.6f | %-10.6f |\n",
           n, num_threads, density, total_time, workload_time, overhead);

    // Save to CSV
    FILE *fp = fopen("floyd_results.csv", "a");
    if (fp) {
        fprintf(fp, "%d,%d,%.2f,%.6f,%.6f,%.6f\n", n, num_threads, density, total_time, workload_time, overhead);
        fclose(fp);
    }

    free(dist);
    return EXIT_SUCCESS;
}


