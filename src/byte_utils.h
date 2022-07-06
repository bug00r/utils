#ifndef BYTE_UTILS_H
#define BYTE_UTILS_H

#include <assert.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum
{
    BYTE_BUFFER_TRUNCATE,   //truncates buffer values to buffer size
    BYTE_BUFFER_SKIP,       //skip apppending or inserting if overflow would be happened
    BYTE_BUFFER_RING        //starts at beginning if overflow would be happened
} byte_buffer_mode_t;

typedef struct 
{
    bool alloc;
    byte_buffer_mode_t mode;
    size_t offset;
    size_t size;
    unsigned char* buffer;
} byte_buffer_t;

//Took outside buffer to work on it
void byte_buffer_init(byte_buffer_t* buffer, 
                      byte_buffer_mode_t mode, 
                      unsigned char* rawBuffer, 
                      size_t rawBuffSize);

//Handles internal memory allocation
void byte_buffer_init_new(byte_buffer_t* buffer, 
                          byte_buffer_mode_t mode, 
                          size_t rawBuffSize);

void byte_buffer_free(byte_buffer_t* buffer);

void byte_buffer_clear(byte_buffer_t* buffer);

//fills the complete buffer with fillbyte
void byte_buffer_fill_complete(byte_buffer_t* buffer, unsigned char fillByte);
//fills the buffer from given index to the end
void byte_buffer_fill_to_end(byte_buffer_t* buffer, size_t index, unsigned char fillByte);
//fills the buffer from start index by range with cnt bytes.
void byte_buffer_fill_range(byte_buffer_t* buffer, size_t startIndex, size_t cnt, unsigned char fillByte);

void byte_buffer_mode_set(byte_buffer_t* buffer, byte_buffer_mode_t mode);

byte_buffer_mode_t byte_buffer_mode_get(byte_buffer_t* buffer);

bool byte_buffer_is_alloc(byte_buffer_t* buffer);

//adding byte or bytes to the buffer
void byte_buffer_append_byte(byte_buffer_t* buffer, unsigned char byte);
void byte_buffer_append_bytes(byte_buffer_t* buffer, unsigned char* bytes, size_t cntBytes);

//replace set byte or bytes from given index
void byte_buffer_replace_byte(byte_buffer_t* buffer, size_t index, unsigned char byte);
void byte_buffer_replace_bytes(byte_buffer_t* buffer, size_t index, unsigned char* bytes, size_t cntBytes);

//insert set byte or bytes at given index. Moves other values based on mode.
void byte_buffer_insert_byte(byte_buffer_t* buffer, size_t index, unsigned char byte);
void byte_buffer_insert_bytes(byte_buffer_t* buffer, size_t index, unsigned char* bytes, size_t cntBytes);

//insert byte or bytes at index 0. Moves other values based on mode.
void byte_buffer_prepend_byte(byte_buffer_t* buffer, unsigned char byte);
void byte_buffer_prepend_bytes(byte_buffer_t* buffer, unsigned char* bytes, size_t cntBytes);

void byte_buffer_append_buffer(byte_buffer_t* dest, byte_buffer_t* src);
void byte_buffer_prepend_buffer(byte_buffer_t* dest, byte_buffer_t* src);
void byte_buffer_replace_buffer(byte_buffer_t* dest, byte_buffer_t* src, size_t index);
void byte_buffer_insert_buffer(byte_buffer_t* dest, byte_buffer_t* src, size_t index);

//merges two buffer into a new with the size and content of buffer A and B.
byte_buffer_t* byte_buffer_join_buffer(byte_buffer_t* bufferA, byte_buffer_t* bufferB);

#endif