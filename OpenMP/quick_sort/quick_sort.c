#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

// Swap two elements
void swap(int* a, int* b) {
    int t = *a;
    *a = *b;
    *b = t;
}

// Partition function
int partition(int arr[], int low, int high) {
    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;
}

// Recursive QuickSort using OpenMP tasks
void quickSort(int arr[], int low, int high, int depth) {
    if (low < high) {
        int pi = partition(arr, low, high);

        #pragma omp task shared(arr) if (depth < 3)
        quickSort(arr, low, pi - 1, depth + 1);

        #pragma omp task shared(arr) if (depth < 3)
        quickSort(arr, pi + 1, high, depth + 1);
    }
}

// Wrapper to launch parallel sort
void quickSortParallel(int arr[], int size, int numThreads) {
    omp_set_num_threads(numThreads);
    #pragma omp parallel
    {
        #pragma omp single nowait
        quickSort(arr, 0, size - 1, 0);
    }
}

// Generate an array of random integers
int* generateRandomArray(int size, int seed) {
    srand(seed);
    int* arr = (int*)malloc(size * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; ++i)
        arr[i] = rand() % 1000000;

    return arr;
}

// Entry point
int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <array_size> <num_threads>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int arraySize = atoi(argv[1]);
    int numThreads = atoi(argv[2]);
    int seed = 42;

    int* arr = generateRandomArray(arraySize, seed);
    omp_set_num_threads(numThreads);

    double workload_time = 0.0;
    double total_start = omp_get_wtime();

    #pragma omp parallel
    {
        double work_start = omp_get_wtime();

        #pragma omp single nowait
        quickSort(arr, 0, arraySize - 1, 0);

        double work_end = omp_get_wtime();

        // Accumulate workload time from all threads
        #pragma omp atomic
        workload_time += (work_end - work_start);
    }

    double total_end = omp_get_wtime();
    double total_time = total_end - total_start;
    double overhead = total_time - workload_time;

    // Output to terminal (formatted table)
    printf("| %-11d | %-8d | %-13.6f | %-13.6f | %-13.6f |\n",
           arraySize, numThreads, total_time, workload_time, overhead);

    // Write to CSV
    FILE* fp = fopen("quick_sort_results.csv", "a");
    if (fp != NULL) {
        fprintf(fp, "%d,%d,%.6f,%.6f,%.6f\n", numThreads, arraySize, total_time, workload_time, overhead);
        fclose(fp);
    }

    free(arr);
    return 0;
}

