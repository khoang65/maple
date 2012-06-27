// Copyright 2011 The University of Michigan
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Authors - Jie Yu (jieyu@umich.edu)
//           Markus Kusano (mukusano@vt.edu)

/**
 * \file
 * \author Jie Yu (jieyu@umuch.edu)
 * \author Markus Kusano (mukusano@vt.edu)
 *
 * Source: https://github.com/jieyu/maple
 *
 * Example program provided with Maple (see source) written by Jie Yu. The
 * program was converted into C code and comments were added.
 */
#include <assert.h>
#include <stdlib.h>
#include <pthread.h>

/* Comment out to remove calls to printf() */
#define DEBUG

/* Comment out to remove calls to sleep(), used to force the bug */
#define FORCE

#ifdef DEBUG
#include <stdio.h>
#endif

#ifdef FORCE
#include <unistd.h>
#endif


/**
 * Type def for list node struct that is stored in the circular list
 */
typedef struct list_node_st list_node;

/** 
 * Circular linked list node data structure. 
 */
struct list_node_st {
  void *data;	/**< Data stored in the node, see data_type for the currently
		  used data */
  list_node *next;  /**< Next pointer for the list */
};


/**
 * Circular linked list data structure. Contains a lock to synchronize access
 */
typedef struct {
  list_node *head;  /**< Head pointer, points to the first item in the list */
  list_node *tail;  /**< Tail pointer, points to the last item in the list */
  int size;	    /**< current size of the list */
  pthread_mutex_t lock;	/**< Internal lock to synchronize access */
} circular_list;

/**
 * Data stored in list_node.data
 */
typedef struct {
  int num;  /**< Store an interger in the list */
} data_type;

/**
 * Lock to protect concurrent calls to malloc().
 */
pthread_mutex_t mem_lock;

/**
 * Allocates memory for a new list_node and returns a pointer to the newly
 * created node 
 *
 * \return Pointer to a newly allocated list_node
 */
list_node *new_list_node() {
  pthread_mutex_lock(&mem_lock);
  list_node *node = malloc(sizeof(list_node));
  pthread_mutex_unlock(&mem_lock);
  return node;
}

/**
 * Frees the passed list_node
 *
 * \param node Pointer to a node to be free'd
 */
void free_list_node(list_node *node) {
  pthread_mutex_lock(&mem_lock);
  free(node);
  pthread_mutex_unlock(&mem_lock);
}

/**
 * Pushes data to the back of list. 
 *
 * This function calls new_list_node() in order to properly store data in the
 * list_node data structure. Operations on the list are synchnoized using the
 * list's lock.
 *
 * \param list The list data will be stored in
 * \param data to be pushed to the back of the list
 */
void list_push_back(circular_list *list, void *data) {
  list_node *node = new_list_node();
  node->data = data;
  node->next = NULL;

  pthread_mutex_lock(&list->lock);
  if (list->tail) {
    list->tail->next = node;
    list->tail = node;
  } else {
    list->head = node;
    list->tail = node;
  }
  list->size++;
  pthread_mutex_unlock(&list->lock);

}

/**
 * Pop the front of the circular_list list
 *
 * \param list List to pop
 * \return Returns the data member of the node at the front of the list or NULL
 * if the list is empty
 */
void *list_pop_front(circular_list *list) {
  void *ret_val;
  list_node *node = NULL;
  pthread_mutex_lock(&list->lock);
  if (list->head) { /* Atleast one member in the list */
    ret_val = list->head->data;
    node = list->head;
    if (list->head == list->tail) { /* One member in the list */
      list->head = list->head->next;
      list->tail = list->head;
    } else {	/* More than one member in the list */
      list->head = list->head->next;
    }
    list->size--;
  } else {  /* No members in the list */
    ret_val = NULL;
  }
  pthread_mutex_unlock(&list->lock);
  if (node) {
    free_list_node(node);
  }
  return ret_val;
}

/**
 * Initialize the passed circular_list list.
 *
 * \param list circular_list to be initialized.
 */
void list_init(circular_list *list) {
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
  pthread_mutex_init(&list->lock, NULL);
}

/**
 * Pop the front of the list and store the result in a local variable.
 * Increment the popped data by 10 and then push the data onto the back of the
 * list. The operation is not protected by any locks, but list_pop_front() and
 * list_push_back() have locks around there own operations to the list.
 *
 * \param list List to perform the above operation on
 */
void process(circular_list * list) {
  data_type *data = (data_type *)list_pop_front(list);
  data->num += 10;

#ifdef FORCE
  static int threadNum = 0;
  if (threadNum == 0) {
    threadNum++;
    sleep(1);
  }

#endif

  list_push_back(list, data);;
}

/**
 * Thread function. Calls process() and then returns
 *
 * \param args circular_list to call process() on
 * \return Returns NULL upon completion
 */
void *t1_main(void *args) {
  circular_list *list = (circular_list *)args;

#ifdef DEBUG
  printf("t1 is rotating the list\n");
#endif

  process(list);

#ifdef DEBUG
  printf("t1 done\n");
#endif

  return NULL;
}

/**
 * Thread function. Calls process() and then returns
 *
 * \param args circular list to call process() on
 * \return Returns NULL upon completion
 */
void *t2_main(void *args) {
  circular_list *list = (circular_list *)args;

#ifdef DEBUG
  printf("t2 is rotating the list\n");
#endif

  process(list);

#ifdef DEBUG
  printf("t2 done\n");
#endif

  return NULL;
}

/**
 * Initializes a circular list and stores 10 nodes into it. The nodes's data
 * values range from 0 to 9 (head to tail).
 */
int main(int argc, char *argv[]) {
  pthread_mutex_init(&mem_lock, NULL);
  circular_list *work_list = malloc(sizeof(circular_list));
  list_init(work_list);

  int i;
  for (i = 0; i < 10; i++) {
    data_type *data = malloc(sizeof(data_type));
    data->num = i;
    list_push_back(work_list, data);
  }

  pthread_t *tids = malloc(sizeof(pthread_t) * 2);

  pthread_create(&tids[0], NULL, t1_main, work_list);
  pthread_create(&tids[1], NULL, t2_main, work_list);
  pthread_join(tids[0], NULL);
  pthread_join(tids[1], NULL);

  /* Print and verify results */
  list_node *iterator = work_list->head;
  int prev_num = -1;
  while (iterator != NULL) {
    data_type *data = (data_type *)iterator->data;

#ifdef DEBUG
    printf("%d ", data->num);
#endif

    if (prev_num != -1) {
      assert(data->num > prev_num);
    }
    prev_num = data->num;
    iterator = iterator->next;
  }

#ifdef DEBUG
  printf("\n");
#endif

  return 0;
}

