#pragma once
#include <stdio.h>
#include <stdlib.h>

int *generate_random_list(int size) {
  if (size <= 0) {
    return NULL;
  }

  int *arr = (int *)malloc(size * sizeof(int));
  if (!arr) {
    fprintf(stderr, "Error: Memory allocation failed.\n");
    return NULL;
  }

  for (int i = 0; i < size; i++) {
    arr[i] = i;
  }

  for (int i = size - 1; i > 0; i--) {
    int j = rand() % (i + 1);
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }

  return arr;
}
