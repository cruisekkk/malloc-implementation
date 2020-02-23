#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "my_malloc.h"

#define NUM_ITERS    10000
#define NUM_ITEMS    10000
#define ALLOC_SIZE   128

#ifdef FF
#define MALLOC(sz) ff_malloc(sz)
#define FREE(p)    ff_free(p)
#endif
#ifdef BF
#define MALLOC(sz) bf_malloc(sz)
#define FREE(p)    bf_free(p)
#endif


double calc_time(struct timespec start, struct timespec end) {
  double start_sec = (double)start.tv_sec*1000000000.0 + (double)start.tv_nsec;
  double end_sec = (double)end.tv_sec*1000000000.0 + (double)end.tv_nsec;

  if (end_sec < start_sec) {
    return 0;
  } else {
    return end_sec - start_sec;
  }
};


int main(int argc, char *argv[])
{
  int i, j;
  int *array[NUM_ITEMS];
  int *spacing_array[NUM_ITEMS];
  unsigned long total_size_of_free_memory;
  unsigned long largest_allocatable_memory_block;
  struct timespec start_time, end_time;

  if (NUM_ITEMS < 10000) {
    printf("Error: NUM_ITEMS must be >= 1000\n");
    return -1;
  } //if

  for (i=0; i < NUM_ITEMS; i++) {
    array[i] = (int *)MALLOC(ALLOC_SIZE);
    spacing_array[i] = (int *)MALLOC(ALLOC_SIZE);
  } //for i

  for (i=0; i < NUM_ITEMS; i++) {
    FREE(array[i]);
  } //for i

  //Start Time
  clock_gettime(CLOCK_MONOTONIC, &start_time);

  for (i=0; i < NUM_ITERS; i++) {
    for (j=0; j < 1000; j++) {
      array[j] = (int *)MALLOC(ALLOC_SIZE);
    } //for j
    
    //printf(" loop start\n");
    
    for (j=1000; j < NUM_ITEMS; j++) {
      array[j] = (int *)MALLOC(ALLOC_SIZE);
      FREE(array[j-1000]);

      if ((i==NUM_ITERS/2) && (j==NUM_ITEMS/2)) {
	//Record fragmentation halfway through (try to repsresent steady state)
	total_size_of_free_memory = get_total_free_size();
	largest_allocatable_memory_block = get_largest_free_data_segment_size();
	
      } //if
    } //for j

    for (j=NUM_ITEMS-1000; j < NUM_ITEMS; j++) {
      FREE(array[j]);
    } //for j

    //printf(" loop end\n");
  } //for i

  //Stop Time
  clock_gettime(CLOCK_MONOTONIC, &end_time);

  double elapsed_ns = calc_time(start_time, end_time);
  printf("Execution Time = %f seconds\n", elapsed_ns / 1e9);
  printf("Fragmentation  = %f\n", 1 - ((float)largest_allocatable_memory_block/(float)total_size_of_free_memory));

  for (i=0; i < NUM_ITEMS; i++) {
    FREE(spacing_array[i]);
  }
  
  return 0;
}
