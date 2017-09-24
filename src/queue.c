/** File: queue.c
  * @author Jeff Martin
  * This file holds the queue functions.
  */

#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

/** Create a queue by allocating a Queue structure, initializing it,
 *  and allocating memory to hold the Queue entries.
 *  @param max_cells Maximum entries in the queue
 *  @return Pointer to newly-allocated Queue structure, NULL if error.
 */
Queue* createQueue(int max_cells) {
  Queue *new_queue; // Holds pointer to the newly-allocated queue structure.
  new_queue = (Queue*) malloc(sizeof(Queue)); 

  if (new_queue == NULL) return NULL; // Error--unable to allocate.

  // Fill in the struct
  new_queue->max_cells = max_cells;
  new_queue->cells_used= 0; //empty to begin

  // Now allocate space for the queue entries.
  new_queue->base = (int*) calloc(sizeof(int), max_cells);
  if (new_queue->base == NULL) {
    free(new_queue); // Unable to allocate queue entries, so free struct.
    return NULL;
  }
  new_queue->head = new_queue->base; // Start at base
  new_queue->tail = new_queue->base; // Start at base

  return new_queue;
}

/** Deletes a queue, including the structure and the memory
 *  for holding the Queue, but not the entries themselves.
 *  @param queue Pointer to Queue structure.
 */
void deleteQueue(Queue *queue) {
  free(queue->base); // Free memory block with stack entries.
  free(queue); // Then free the struct.
}

/** enqueues an integer onto a queue.
 *  @param queue Pointer to queue you want to enqueue onto.
 *  @param element Number to be enqueued.
 *  @return 0 if successful, -1 if not.
 */
int enqueue(Queue *queue, int element) {
  // Check if queue is already full 
  if ((queue->cells_used) >= (queue->max_cells)) {
    queue->cells_used = queue->max_cells; // Fix
    return -1;  // queue overflow.
  }

  // enqueue onto queue.
  *(queue->tail) = element;  // Store the number on the queue
  (queue->tail)++; //increment pointer
  if (queue->tail == (queue->base + queue->max_cells)){
    //determines if the head needs to loop back to the beginning
    queue->tail = queue->base; //wraps it back around.
  }

  (queue->cells_used)++; //increment count of elements.
  return 0;  // Success
}

/** Dequeues head of queue, and returns it.
 *  @param queue Pointer to Queue you want to dequeue from.
 *  @return head of the queue, -1 if queue is empty.
 */
int dequeue(Queue *queue) {
  // Check if queue is empty
  if ((queue->cells_used) <= 0) {
    queue->cells_used = 0; // Fix
    return -1;  // queue empty
  }

  // dequeue from queue
  int* head = queue->head; // Point to head
  (queue->head)++; //increment pointer
  if (queue->head == (queue->base + queue->max_cells)){
    //determines if the tail needs to loop back to the beginning
    queue->head = queue->base; //wraps it back around.
  }

  (queue->cells_used)--;//decrement count of entries
  return (*head); // Get the entry
  
}

/** Peek at the top of the queue, without dequeue 
 *  @param queue Pointer to the Queue you want to peek into
 *  @return Head of the Queue, -1 if queue is empty.
 */
int peek(Queue *queue) {
  // Check if Queue is empty
  if ((queue->cells_used) <= 0) {
    queue->cells_used = 0; // Fix
    return -1;  // Queue empty
  }

  //Return head of queue, without dequeue
  //Remember, 'next' points to next free cell

  int* head = queue->head; // Point to tail

  return (*head); // Get the entry
}
