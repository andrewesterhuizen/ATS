#pragma once

#include <stdint.h>
#include <stdlib.h>

template <class T>
class Queue
{
public:
    Queue(uint8_t _capacity = 16)
    {
        capacity = _capacity;
        nItems = 0;
        front = 0;
        rear = 0;
        items = (T *)malloc(capacity * sizeof(T));
    }

    bool isEmpty()
    {
        return nItems == 0;
    }

    void enqueue(T item)
    {
        if (nItems == capacity)
            return;

        items[rear] = item;
        rear = (rear + 1) % capacity;
        nItems++;
    }

    T dequeue()
    {
        if (isEmpty())
        {
            return T{};
        }

        T item = items[front];
        front = (front + 1) % capacity;
        nItems--;
        return item;
    }

private:
    uint8_t front;
    uint8_t rear;
    uint8_t nItems;
    uint8_t capacity;
    T *items;
};
