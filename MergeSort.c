#include "types.h"
#include "user.h"
#include "stat.h"

// Merge function


void merge(int arr[], int left, int mid, int right) {
    int i, j, k;
    int l = mid - left + 1;
    int r = right - mid;
    int leftbranch[50], rightbranch[50];  // Reduce static array sizes for safety

    for (i = 0; i < l; i++)
        leftbranch[i] = arr[left + i];

    for (j = 0; j < r; j++)
        rightbranch[j] = arr[mid + 1 + j];

    i = 0;  
    j = 0;  
    k = left;  

    while (i < l && j < r) {
        if (leftbranch[i] <= rightbranch[j]) {
            arr[k] = leftbranch[i];
            i++;
        } else {
            arr[k] = rightbranch[j];
            j++;
        }
        k++;
    }

    while (i < l) {
        arr[k] = leftbranch[i];
        i++;
        k++;
    }

    while (j < r) {
        arr[k] = rightbranch[j];
        j++;
        k++;
    }
}

// Merge Sort function
void mergeSort(int arr[], int left, int right) {
    if (left < right) {
        int mid = left + (right - left) / 2;
        mergeSort(arr, left, mid);
        mergeSort(arr, mid + 1, right);
        merge(arr, left, mid, right);
    } 
}

int main(int argc, char *argv[]) {
    printf(1, "WE got the arguments\n");
    if (argc < 2) {
        printf(1, "Usage: %s <list of numbers>\n", argv[0]);
        exit();  // Fix: No arguments allowed in exit()
    }

    int n = argc - 1;
    int arr[100]; // Keep static size for xv6 compatibility

    // Convert command-line arguments to integers
    for (int i = 0; i < n; i++) {
        arr[i] = atoi(argv[i + 1]);  // Use built-in atoi()
    }

    // Sorting arr using Merge Sort
    mergeSort(arr, 0, n - 1);

    // Print sorted array
    printf(1, "Sorted array: ");
    for (int i = 0; i < n; i++) {
        printf(1, "%d ", arr[i]);
    }
    printf(1, "\n");

    exit();  // Fix: No arguments allowed in exit()
}
