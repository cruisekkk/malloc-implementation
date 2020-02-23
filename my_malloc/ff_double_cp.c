# include "my_malloc.h"
# define INT_MAX 2147483647

int first_call = 1;
struct link_node* first_node = NULL;
struct link_node* last_node = NULL;
long total_free = 0;
long total_segment = 0;
long largest_free = 0;

unsigned long get_total_free_size(){ // total free
  /*
  struct link_node *temp = first_node;
  unsigned long total = 0;
  while(temp != NULL){
    if (temp->isfree){
      total += temp->length;
    }
    temp = temp -> next;
  }

  return total;
  */
  return total_free;
}

unsigned long get_data_segment_size(){
  /*
  struct link_node *temp = first_node;
  unsigned long total = 0;
  while(temp != NULL){
    total += temp->length;
    temp = temp -> next;
  }

  return total;
  */
  return total_segment;
}


unsigned long get_largest_free_data_segment_size(){
  struct link_node * temp= first_node;
  unsigned long curr_max = 0;
  while(temp != NULL){
    if(temp->isfree == 1 && temp->length > curr_max){ //can exchange later, save time
      curr_max = temp->length; 
    }
    temp = temp -> next;
  }
  return curr_max;
}


void set_new_malloc(struct link_node * node_ptr, size_t size){
  node_ptr->last = last_node; // set head
  last_node = node_ptr;
  node_ptr->next = NULL; // set tail
  node_ptr->isfree = 0;
  node_ptr->length = size;
  node_ptr->curr_size = size;
  total_segment += node_ptr->length;
}


// search the first enough node to free
struct link_node * ff_search_free_node(size_t size){
  struct link_node * temp= first_node;
  while(temp != NULL){
    if(temp->isfree == 1 && temp->length >= size){      
      temp->isfree = 0;
      temp->curr_size = size;
      total_free -= size;
      
      return temp; 
    }
    
    temp = temp -> next;
  }

  return NULL;
}


void * ff_malloc(size_t size){
  struct link_node* node_ptr;
  if (size <= 0){
    return NULL;
  }

  if (first_call){
    node_ptr = sbrk(0); 
    void * malloced = sbrk(size + NODE_SIZE);
    if (malloced == (void *) -1){
      return NULL;
    }
    
    first_call = 0;
    first_node = node_ptr;
    set_new_malloc(node_ptr, size);
  }
// when malloc is not in the first time
  else{  
     node_ptr = ff_search_free_node(size);
     // find usable space for node
     if (node_ptr == NULL){ // there is no usable space, need new malloc 
       node_ptr = sbrk(0); 
       void * malloced = sbrk(size + NODE_SIZE); 
       if (malloced == (void *) -1){
	 return NULL;
       }
       last_node->next = node_ptr; 
       set_new_malloc(node_ptr, size);
      
     }
  }    
  void * content = node_ptr + 1;
  return content;
}

void ff_free(void *ptr){
  struct link_node* p = (struct link_node*)ptr - 1;
  if (p->isfree == 1){
    return;
  }
  
  total_free += p->length; // dynamic change
  p->isfree = 1;
  p->curr_size = 0;

  if (p->last != NULL && p->last->isfree == 1){ // last node is a free node
    p = p->last;
    if (p->next->next != NULL && p->next->next->isfree == 1){ // situation 1: 3 adjacent
      //p = p->last;
      p->length = p->length + p->next->length + p->next->next->length + 2 * NODE_SIZE;
      p->next = p->next->next->next;

    }
    else{ //situation 2: 2 adjacent
      p->length = p->length + p->next->length + 1 * NODE_SIZE;
      p->next = p->next->next;
    }

    if(p->next != NULL){
      p->next->last = p;
    }
    return;
  }

  ///*
  else if (p->next != NULL && p->next->isfree == 1){ // situation 3: 2 adjacent)
    p->length = p->length + p->next->length + 1 * NODE_SIZE;
    p->next = p->next->next;
    if(p->next != NULL){
      p->next->last = p;
    }
  }


  //*/
  // */
}

// search the best free node to free
struct link_node * bf_search_free_node(size_t size){
  struct link_node * temp = first_node;
  struct link_node * best_node = NULL;
  int min = INT_MAX;
  while(temp != NULL){
    if(temp->isfree == 1 && temp->length >= size){
      if (temp->length == size){
	best_node = temp;
	break;
      }
      if (temp->length - size < min){
	min = temp->length;
	best_node = temp;
      }
    }
    temp = temp -> next;
  }
  if (best_node != NULL){
    best_node->isfree = 0;
    best_node->curr_size = size;
    total_free -= best_node->length;
  }
  
  return best_node;
}


void * bf_malloc(size_t size){
  struct link_node* node_ptr;
  if (size <= 0){
    return NULL;
  }

  if (first_call){
    node_ptr = sbrk(0); 
    void * malloced = sbrk(size + NODE_SIZE);
    if (malloced == (void *) -1){
      return NULL;
    }
    
    first_call = 0;
    first_node = node_ptr;
    set_new_malloc(node_ptr, size);
  }
// when malloc is not in the first time
  else{  
     node_ptr = bf_search_free_node(size);
     // find usable space for node
     if (node_ptr == NULL){ // there is no usable space, need new malloc 
       node_ptr = sbrk(0); 
       void * malloced = sbrk(size + NODE_SIZE); 
       if (malloced == (void *) -1){
	 return NULL;
       }
       last_node->next = node_ptr;
       set_new_malloc(node_ptr, size);
     }
  }    
  void * content = node_ptr + 1;
  return content;
}

void bf_free(void *ptr){
  struct link_node* p = (struct link_node*)ptr - 1;
  total_free += p->length; // dynamic change
  p->isfree = 1;
  p->curr_size = 0;
}


int main(void){
//  int* test1 = ff_malloc(2 * sizeof(int));
//  int* test2 = ff_malloc(5 * sizeof(int));
//  ff_free(test1);
//  int* test3 = ff_malloc(200 * sizeof(int));
//  int* test4 = ff_malloc(sizeof(int));
//  return EXIT_SUCCESS;

const unsigned NUM_ITEMS = 10;
  int i;
  int size;
  int sum = 0;
  int expected_sum = 0;
  int *array[NUM_ITEMS];

  size = 4;
  expected_sum += size * size;
  array[0] = (int *)ff_malloc(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[0][i] = size;
  } //for i                                                                    
  for (i=0; i < size; i++) {
    sum += array[0][i];
  } //for i

  size = 16;
  expected_sum += size * size;
  array[1] = (int *)ff_malloc(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[1][i] = size;
  } //for i                                                                    
  for (i=0; i < size; i++) {
    sum += array[1][i];
  } //for i                                                                    
  //  /*
  
  size = 8;
  expected_sum += size * size;
  array[2] = (int *)ff_malloc(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[2][i] = size;
  } //for i                                                                    
  for (i=0; i < size; i++) {
    sum += array[2][i];
  } //for i    

  size = 32;
  expected_sum += size * size;
  array[3] = (int *)ff_malloc(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[3][i] = size;
  } //for i                                                                    
  for (i=0; i < size; i++) {
    sum += array[3][i];
  } //for i                                                                    

  unsigned long a = get_largest_free_data_segment_size();
  printf("1: %lu\n", a);
  ff_free(array[0]);
  ff_free(array[2]);
  a = get_largest_free_data_segment_size();
  printf("2: %lu\n", a);
  // /*                                                                        
  size = 7;
  expected_sum += size * size;
  array[4] = (int *)ff_malloc(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[4][i] = size;
  } //for i                                                                    
  for (i=0; i < size; i++) {
    sum += array[4][i];
  }

  size = 256;
  expected_sum += size * size;
  array[5] = (int *)ff_malloc(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[5][i] = size;
  } //for i                                                                    
  for (i=0; i < size; i++) {
    sum += array[5][i];
  } //for i                                                                    
  
  ff_free(array[5]);
  ff_free(array[1]);
  ff_free(array[3]);

// /*                                                                        
  size = 23;
  expected_sum += size * size;
  array[6] = (int *)ff_malloc(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[6][i] = size;
  } //for i                                                                    
  for (i=0; i < size; i++) {
    sum += array[6][i];
  } //for i                                                                    

  size = 4;
  expected_sum += size * size;
  array[7] = (int *)ff_malloc(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[7][i] = size;
  } //for i                                                                    
  for (i=0; i < size; i++) {
    sum += array[7][i];
  } //for i                                                                    

  ff_free(array[4]);

  size = 10;
  expected_sum += size * size;
  array[8] = (int *)ff_malloc(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[8][i] = size;
  } //for i                                                                    
  for (i=0; i < size; i++) {
    sum += array[8][i];
  } //for i                                                                    

  size = 32;
  expected_sum += size * size;
  array[9] = (int *)ff_malloc(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[9][i] = size;
  } //for i                                                                    
  for (i=0; i < size; i++) {
    sum += array[9][i];
  } //for i    

  ff_free(array[6]);
  ff_free(array[7]);
  ff_free(array[8]);
  ff_free(array[9]);
  
  if (sum == expected_sum) {
    printf("Calculated expected value of %d\n", sum);
    printf("Test passed\n");
  } else {
    printf("Expected sum=%d but calculated %d\n", expected_sum, sum);
    printf("Test failed\n");
  } //else 
  return EXIT_SUCCESS;
}
// */
