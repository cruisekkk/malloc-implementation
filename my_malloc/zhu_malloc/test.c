# include "my_malloc.h"


int first_call = 1;
struct link_node* first_node = NULL;
struct link_node* last_node = NULL;


unsigned long get_largest_free_data_segment_size(){
  struct link_node * temp= first_node;
  int curr_max = 0;
  while(temp != NULL){
    if(temp->length >= curr_max && temp->isfree == 1){ //can exchange later, save time
      curr_max = temp->length; 
    }
    temp = temp -> next;
  }

  return curr_max;
}

// search the first enough node to free
struct link_node * ff_search_free_node(size_t size){
  struct link_node * temp= first_node;
  while(temp != NULL){
    if(temp->length >= size && temp->isfree == 1){
      
      temp->isfree = 0;
      temp->curr_size = size;
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
    // don't know if we need NODE_SIZE

    if (malloced == (void *) -1){
      return NULL;
    }
 
    first_call = 0;
    first_node = node_ptr;
    last_node = node_ptr;
    node_ptr->next = NULL;
    node_ptr->isfree = 0;
    node_ptr->length = size;
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
       node_ptr->next = NULL;
       node_ptr = last_node;
       
       node_ptr->length = size;
       node_ptr->curr_size = size;
       node_ptr->isfree = 0;
     }
  }    
  void * content = node_ptr + 1;
  return content;
}

void ff_free(void *ptr){
  struct link_node* p = ptr - 1;
  p->isfree = 1;
  p->curr_size = 0;
}


int main(void){
  int* test1 = ff_malloc(2 * sizeof(int));
  int* test2 = ff_malloc(5 * sizeof(int));
  ff_free(test1);
  int* test3 = ff_malloc(200 * sizeof(int));
  int* test4 = ff_malloc(sizeof(int));
  return EXIT_SUCCESS;
}
