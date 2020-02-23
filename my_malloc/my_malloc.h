#include <stdio.h>
#include <stdlib.h>
#include "sys/time.h"
#include <unistd.h>

// data structure to operate memory
struct link_node{
  struct link_node* next;
  struct link_node* last;
  int isfree;
  unsigned long length;
  int curr_size;
};

// NODE_SIZE, used in main c program to represent the size(in bytes) of the struct
#define NODE_SIZE sizeof(struct link_node)

// used to set the properties of a new memory chunk
void set_new_malloc(struct link_node* node_ptr, size_t size);

// used to split the free memory into two parts
void split(struct link_node* node, size_t size);


// First Fit malloc/free
void * ff_malloc(size_t size);
void ff_free(void * ptr);
struct link_node* ff_search_free_node(size_t size);


// Best Fit malloc/free
void * bf_malloc(size_t size);
void bf_free(void * ptr);


// For memory fragmentation
unsigned long get_largest_free_data_segment_size(); //in bytes
unsigned long get_total_free_size(); //in bytes
unsigned long get_data_segment_size();
