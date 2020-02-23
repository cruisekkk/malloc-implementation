#include <stdio.h>
#include <stdlib.h>
#include "sys/time.h"
#include <unistd.h>
// data structure

struct link_node{
  struct link_node* next;
  int isfree;
  int length;
  int curr_size;
};

#define NODE_SIZE sizeof(struct link_node)

// First Fit malloc/free
void * ff_malloc(size_t size);
void ff_free(void * ptr);

// Best Fit malloc/free
void * bf_malloc(size_t size);
void bf_free(void * ptr);

// For memory fragmentation
unsigned long get_largest_free_data_segment_size(); //in bytes
unsigned long get_total_free_size(); //in bytes
