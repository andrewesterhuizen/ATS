#pragma once

#include <stdint.h>
#include <stdlib.h>

template <typename T, uint8_t capacity = 16>
class Queue
{
public:
    Queue()
    {
        n_items = 0;
        front = 0;
        rear = 0;
        items = (T *)malloc(capacity * sizeof(T));
    }

    bool is_empty()
    {
        return n_items == 0;
    }

    void enqueue(T item)
    {
        if (n_items == capacity)
            return;

        items[rear] = item;
        rear = (rear + 1) % capacity;
        n_items++;
    }

    T dequeue()
    {
        if (is_empty())
        {
            return T{};
        }

        T item = items[front];
        front = (front + 1) % capacity;
        n_items--;
        return item;
    }

private:
    uint8_t front;
    uint8_t rear;
    uint8_t n_items;
    T *items;
};
