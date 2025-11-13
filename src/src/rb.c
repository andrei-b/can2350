#include "rb.h"

static inline void __DMB(void)
{
  asm volatile ("dmb" ::: "memory");
}

void rb_init(RingBuffer* rb) {
    rb->head = 0;
    rb->tail = 0;
}



bool rb_push(RingBuffer *rb, uint16_t can_id, uint8_t dlc, uint8_t * data)
{
    size_t next = (rb->head + RECORD_SIZE) % BUFFER_SIZE;
    if (next == rb->tail)
        return false; // full

    *((uint16_t*)(&rb->buffer[rb->head])) = can_id;
    rb->buffer[rb->head + 2] = dlc;
    memcpy(&rb->buffer[rb->head + 3], data, dlc);

    __DMB();  // ensure data is visible before publishing head

    rb->head = next;
    return true;
}

bool rb_pop(RingBuffer *rb, uint16_t * can_id, uint8_t * dlc, uint8_t * data)
{
    if (rb->head == rb->tail)
        return false; // empty

    __DMB();  // ensure we see data written by producer before reading buffer

    can_id = ((uint16_t*)(&rb->buffer[rb->tail]));
    dlc = rb->buffer[rb->tail + 2];
    memcpy(data, &rb->buffer[rb->tail + 3], *dlc);
    rb->tail = (rb->tail + RECORD_SIZE) % BUFFER_SIZE;
    return true;
}