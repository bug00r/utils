#ifndef BYTE_UTILS_H
#define BYTE_UTILS_H

#include <assert.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef enum
{
    BYTE_BUFFER_TRUNCATE,   //truncates buffer values to buffer size
    BYTE_BUFFER_SKIP,       //skip apppending or inserting if overflow would be happened
    BYTE_BUFFER_RING        //starts at beginning if overflow would be happened
} ByteBufferMode;

typedef struct 
{
    bool allocObj;              //true, if byte_buffer_new was called
    bool alloc;                 //true, if byte_buffer_new or byte_buffer_init_new were called
    ByteBufferMode mode;    //mode of buffer
    size_t offset;              //current intern offset
    size_t size;                //capacity of the buffer
    unsigned char* buffer;      //the rawBuffer Data
} ByteBuffer;

//Allocates a complete Buffer Object
ByteBuffer* byte_buffer_new(ByteBufferMode mode, size_t rawBuffSize);

//Took outside buffer to work on it
void byte_buffer_init(ByteBuffer* buffer, 
                      ByteBufferMode mode, 
                      unsigned char* rawBuffer, 
                      size_t rawBuffSize);

//Handles internal memory allocation
void byte_buffer_init_new(ByteBuffer* buffer, 
                          ByteBufferMode mode, 
                          size_t rawBuffSize);

void byte_buffer_free(ByteBuffer** buffer);

void byte_buffer_clear(ByteBuffer* buffer);

//fills the complete buffer with fillbyte
void byte_buffer_fill_complete(ByteBuffer* buffer, unsigned char fillByte);
//fills the buffer from given index to the end
void byte_buffer_fill_to_end(ByteBuffer* buffer, size_t index, unsigned char fillByte);
//fills the buffer from start index by range with cnt bytes.
void byte_buffer_fill_range(ByteBuffer* buffer, size_t startIndex, size_t cnt, unsigned char fillByte);

void byte_buffer_mode_set(ByteBuffer* buffer, ByteBufferMode mode);

ByteBufferMode byte_buffer_mode_get(ByteBuffer* buffer);

bool byte_buffer_is_alloc(ByteBuffer* buffer);

//adding byte or bytes to the buffer
void byte_buffer_append_byte(ByteBuffer* buffer, unsigned char byte);
void byte_buffer_append_bytes(ByteBuffer* buffer, unsigned char* bytes, size_t cntBytes);
void byte_buffer_append_bytes_fmt(ByteBuffer* buffer, unsigned char* fmt, ...);

//replace set byte or bytes from given index
void byte_buffer_replace_byte(ByteBuffer* buffer, size_t index, unsigned char byte);
void byte_buffer_replace_bytes(ByteBuffer* buffer, size_t index, unsigned char* bytes, size_t cntBytes);
void byte_buffer_replace_bytes_fmt(ByteBuffer* buffer, size_t index, unsigned char* fmt, ...);

//insert set byte or bytes at given index. Moves other values based on mode.
void byte_buffer_insert_byte(ByteBuffer* buffer, size_t index, unsigned char byte);
void byte_buffer_insert_bytes(ByteBuffer* buffer, size_t index, unsigned char* bytes, size_t cntBytes);
void byte_buffer_insert_bytes_fmt(ByteBuffer* buffer, size_t index, unsigned char* fmt, ...);

//insert byte or bytes at index 0. Moves other values based on mode.
void byte_buffer_prepend_byte(ByteBuffer* buffer, unsigned char byte);
void byte_buffer_prepend_bytes(ByteBuffer* buffer, unsigned char* bytes, size_t cntBytes);
void byte_buffer_prepend_bytes_fmt(ByteBuffer* buffer, unsigned char* fmt, ...);

void byte_buffer_append_buffer(ByteBuffer* dest, ByteBuffer* src);
void byte_buffer_prepend_buffer(ByteBuffer* dest, ByteBuffer* src);
void byte_buffer_replace_buffer(ByteBuffer* dest, ByteBuffer* src, size_t index);
void byte_buffer_insert_buffer(ByteBuffer* dest, ByteBuffer* src, size_t index);

/* Merges two buffer into a new with the size and content of buffer A and B. 
   The Result Buffer musst be free'd by caller in reason of dynamic memory allocation.
*/
ByteBuffer* byte_buffer_join_buffer(ByteBuffer* bufferA, ByteBuffer* bufferB, ByteBufferMode resultMode);

#endif