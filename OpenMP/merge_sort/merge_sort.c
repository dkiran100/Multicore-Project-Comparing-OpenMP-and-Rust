#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

// Generate an array of random integers
int* generateRandomArray(int size, int seed) {
    srand(seed);
    int* arr = (int*)malloc(size * sizeof(int));
    if (!arr) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; ++i) {
        arr[i] = rand() % 1000000;
    }

    return arr;
}

// Merge two sorted subarrays into one sorted segment
void merge(int arr[], int left, int mid, int right) {
    int leftSize = mid - left + 1;
    int rightSize = right - mid;

    int* L = (int*)malloc(leftSize * sizeof(int));
    int* R = (int*)malloc(rightSize * sizeof(int));
    if (!L || !R) {
        fprintf(stderr, "Error: Memory allocation failed during merge.\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < leftSize; ++i)
        L[i] = arr[left + i];
    for (int j = 0; j < rightSize; ++j)
        R[j] = arr[mid + 1 + j];

    int i = 0, j = 0, k = left;
    while (i < leftSize && j < rightSize) {
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    }

    while (i < leftSize) {
        arr[k++] = L[i++];
    }

    while (j < rightSize) {
        arr[k++] = R[j++];
    }

    free(L);
    free(R);
}

// Recursive merge sort with OpenMP task-based parallelism
void parallelMergeSort(int arr[], int left, int right, int depth) {
    if (left < right) {
        int mid = left + (right - left) / 2;

        #pragma omp task shared(arr) if(depth < 3)
        parallelMergeSort(arr, left, mid, depth + 1);

        #pragma omp task shared(arr) if(depth < 3)
        parallelMergeSort(arr, mid + 1, right, depth + 1);

        #pragma omp taskwait
        merge(arr, left, mid, right);
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <threads> <array_size>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int numThreads = atoi(argv[1]);
    int arraySize = atoi(argv[2]);
    int seed = 40;

    int* data = generateRandomArray(arraySize, seed);
    omp_set_num_threads(numThreads);

    double workload_time = 0.0;
    double total_start = omp_get_wtime();

    // Begin parallel region for sorting
    #pragma omp parallel
    {
        double work_start = omp_get_wtime();

        #pragma omp single nowait
        parallelMergeSort(data, 0, arraySize - 1, 0);

        double work_end = omp_get_wtime();

        // Accumulate workload time
        #pragma omp atomic
        workload_time += (work_end - work_start);
    }

    double total_end = omp_get_wtime();
    double total_time = total_end - total_start;
    double overhead = total_time - workload_time;

    // Output result in table format
    printf("| %-10d | %-8d | %-13.6f | %-13.6f | %-13.6f |\n",
           arraySize, numThreads, total_time, workload_time, overhead);

    // Append to CSV
    FILE* fp = fopen("merge_sort_results.csv", "a");
    if (fp != NULL) {
        fprintf(fp, "%d,%d,%.6f,%.6f,%.6f\n", arraySize, numThreads, total_time, workload_time, overhead);
        fclose(fp);
    }

    free(data);
    return EXIT_SUCCESS;
}


