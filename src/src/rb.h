#include <stdint.h>
#include <stddef.h> 
#include <stdbool.h>

#define RECORD_SIZE 11


#define BUFFER_SIZE 16*RECORD_SIZE

typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    volatile size_t head;  
    volatile size_t tail;  
} RingBuffer;

void rb_init(RingBuffer* rb);