#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "c_queue.h"
#define MIN_CAP 32

queue *q_create(){
    queue *q=(queue*)malloc(sizeof(queue));
    q->body=(char *)malloc(32*sizeof(char));
    /*for (int i = 0; i < 31; ++i) {
        q->path[i]=path[i];
    }*/
    q->size=0;
    q->cap=32;
    q->front=0;
    return q;
}
bool q_is_empty(queue *q){
    if(q->size==0){
        return true;
    } else {
        return false;
    }
}
bool q_grow(queue *q){
    int new_cap=2*q->cap;
    char *new_body=(char *)realloc(q->body,new_cap* sizeof(char));
    if(new_body==NULL){
        return false;
    }
    for(int i = q->size-1 ;i >= q->front; i=i-1){
        new_body[i+(new_cap-q->cap)]=q->body[i];
    }
    q->body=new_body;
    if(q->front==0) {
        q->front = q->front;
    }else{
        q->front = q->front+(q->cap-new_cap);
    }
    q->cap=new_cap;
    return true;
}
void q_shrink(queue *q){
    if(q->size > q->cap/4){
        return;
    }
    int new_cap=q->cap/2;
    if(new_cap < MIN_CAP){
        new_cap=MIN_CAP;
    }
    if(q->front+q->size >= q->cap){
        for (int i = q->front; i < q->cap; ++i) {
            q->body[i-(q->cap-new_cap)]=q->body[i];
        }
        q->front=q->front-(q->cap-new_cap);
    } else{
        for (int i = q->front; i < q->size+q->front; ++i) {
            q->body[i-(q->cap-new_cap)]=q->body[i];
        }
    }
    q->body=(char *)realloc(q->body, sizeof(char)*new_cap);
    q->cap=new_cap;
    return;
}

char pop(queue *q){
    char val=q->body[q->front];
    if (q->front==q->cap-1){
        q->front=0;
    } else {
        q->front++;
    }
    q->size--;
    return val;
}
void q_destroy(queue *q){
    free(q->body);
    for (int i = 0; i < 31; ++i) {
        q->body[i]='\0';
    }
    q->size=0;
    q->cap=0;
    q->front=0;
}
bool push(queue *q, char val){
    if(q->size==q->cap){
        if(!q_grow(q)){
            return false;
        }
    }
    q->body[(q->size+q->front)%q->cap]=val;
    q->size++;
    return true;
}
char q_front(queue *q) {
    return q->body[q->front];
}
int q_size(queue *q) {
    return q->size;
}