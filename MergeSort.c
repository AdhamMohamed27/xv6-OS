#include <stdio.h>
#include <stdlib.h>

// Function to convert string to integer (handles negative numbers)
int my_atoi(char *s) {
    int num = 0, sign = 1;
    if (*s == '-') {
        sign = -1;
        s++;
    }
    while (*s >= '0' && *s <= '9') {
        num = num * 10 + (*s - '0');
        s++;
    }
    return sign * num;
}

// Merge function
void merge(int arr[], int left, int mid, int right) {
    int i, j, k;
    int l = mid - left + 1;
    int r = right - mid;
    int leftbranch[100], rightbranch[100];

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
    if (argc < 2) {
        printf("Usage: %s <list of numbers>\n", argv[0]);
        exit(1);
    }

    int n = argc - 1;
    int arr[100]; // Static size for xv6 compatibility

    // Convert command-line arguments to integers
    for (int i = 0; i < n; i++) {
        arr[i] = my_atoi(argv[i + 1]);
    }

    // Sorting arr using Merge Sort
    mergeSort(arr, 0, n - 1);

    // Print sorted array
    printf("Sorted array: ");
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    exit(0);
}
