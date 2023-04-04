/*
    File: deque.h
    This file provied an interface to a simple double ended queue abstraction.

*/
#ifndef _deque_h
#define _deque_h

struct DequeNode {
    void *dataptr;
    struct DequeNode *front;
    struct DequeNode *next;
};

typedef struct DequeNode* Deque;
#endif