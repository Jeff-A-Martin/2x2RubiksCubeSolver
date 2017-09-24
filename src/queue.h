/** File: queue.h 
  * @author Jeff Martin
  * This file defines a queue struct; each entry can have a pointer to anything.
  *   The head points to the last occupied cell.
  *   The tail points to the next free cell
  * The file also includes all the function prototypes for queue functions.
  */

struct queue{
  int* base; //pointer to the base of the queue
  int* head; //pointer to the last occupied cell
  int* tail; //pointer to the next free cell. 
  int max_cells; //Maximum number of entries in the queue
  int cells_used; //The number of cells that are currently full.
};

typedef struct queue Queue;

//Function prototypes.

Queue* createQueue(int max_cells);

void deleteQueue(Queue *queue);

int enqueue(Queue *queue, int element);

int dequeue(Queue *queue);

int peek(Queue *queue);
