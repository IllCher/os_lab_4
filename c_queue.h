#ifndef D_QUEUE_NEW
#define D_QUEUE_NEW
#include <stdbool.h>
typedef struct {
    char *body;
    //char path[32];
    int size;
    int cap;
    int front;
}queue;
bool q_grow(queue *s);
void q_srink(queue *s);
queue *q_create();
void q_destroy(queue *q);
bool q_is_empty(queue *q);
bool push(queue *q, char val);
char pop(queue *q);
char q_front(queue *q);
int q_size(queue *q);
#endif