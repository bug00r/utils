#include "byte_utils.h"

//Took outside buffer to work on it
void byte_buffer_init(byte_buffer_t* _buffer, 
                      byte_buffer_mode_t mode, 
                      unsigned char* rawBuffer, 
                      size_t rawBuffSize)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer)
	{
		buffer->alloc = false;
		buffer->mode = mode;
		buffer->offset = 0;
		buffer->size = rawBuffSize;
		buffer->buffer = rawBuffer;
	}
}


//Handles internal memory allocation
void byte_buffer_init_new(byte_buffer_t* _buffer, 
                          byte_buffer_mode_t mode, 
                          size_t rawBuffSize)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer)
	{
		buffer->alloc = true;
		buffer->mode = mode;
		buffer->offset = 0;
		buffer->size = rawBuffSize;
		buffer->buffer = malloc(rawBuffSize * sizeof(unsigned char));
	}
}


void byte_buffer_free(byte_buffer_t* _buffer)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer)
	{
		if (buffer->alloc)
		{
			free(buffer->buffer);
		}
		buffer->buffer = NULL;
		buffer->size = 0;
		buffer->offset = 0;
	}
}

//fills the complete buffer with fillbyte
void byte_buffer_fill_complete(byte_buffer_t* _buffer, unsigned char fillByte)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer)
	{
		memset(buffer->buffer, fillByte, buffer->size);
	}
}

//fills the buffer from given index to the end
void byte_buffer_fill_to_end(byte_buffer_t* _buffer, size_t index, unsigned char fillByte)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer && index < buffer->size)
	{
		memset(buffer->buffer + index, fillByte, buffer->size - index);
	}
}

//fills the buffer from start index by range with cnt bytes.
void byte_buffer_fill_range(byte_buffer_t* _buffer, size_t startIndex, size_t cnt, unsigned char fillByte)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer && startIndex < buffer->size)
	{
		size_t alignedCnt = cnt;
		alignedCnt = ( alignedCnt + startIndex < buffer->size ? cnt : (buffer->size - startIndex));
		memset(buffer->buffer + startIndex, fillByte, alignedCnt);
	}
}


void byte_buffer_clear(byte_buffer_t* buffer)
{
	assert(("byte_buffer_clear: Not Implement YET!!!", false));
}

void byte_buffer_mode_set(byte_buffer_t* buffer, byte_buffer_mode_t mode)
{
	assert(("byte_buffer_mode_set: Not Implement YET!!!", false));
}


byte_buffer_mode_t byte_buffer_mode_get(byte_buffer_t* buffer)
{
	assert(("byte_buffer_mode_get: Not Implement YET!!!", false));
}


bool byte_buffer_is_alloc(byte_buffer_t* buffer)
{
	return buffer->alloc;
}


//adding byte or bytes to the buffer
void byte_buffer_append_byte(byte_buffer_t* buffer, unsigned char byte)
{
	assert(("byte_buffer_append_byte: Not Implement YET!!!", false));
}

void byte_buffer_append_bytes(byte_buffer_t* buffer, unsigned char* bytes, size_t cntBytes)
{
	assert(("byte_buffer_append_bytes: Not Implement YET!!!", false));
}


//replace set byte or bytes from given index
void byte_buffer_replace_byte(byte_buffer_t* buffer, size_t index, unsigned char byte)
{
	assert(("byte_buffer_replace_byte: Not Implement YET!!!", false));
}

void byte_buffer_replace_bytes(byte_buffer_t* buffer, size_t index, unsigned char* bytes, size_t cntBytes)
{
	assert(("byte_buffer_replace_bytes: Not Implement YET!!!", false));
}


//insert set byte or bytes at given index. Moves other values based on mode.
void byte_buffer_insert_byte(byte_buffer_t* buffer, size_t index, unsigned char byte)
{
	assert(("byte_buffer_insert_byte: Not Implement YET!!!", false));
}

void byte_buffer_insert_bytes(byte_buffer_t* buffer, size_t index, unsigned char* bytes, size_t cntBytes)
{
	assert(("byte_buffer_insert_bytes: Not Implement YET!!!", false));
}


//insert byte or bytes at index 0. Moves other values based on mode.
void byte_buffer_prepend_byte(byte_buffer_t* buffer, unsigned char byte)
{
	assert(("byte_buffer_prepend_byte: Not Implement YET!!!", false));
}

void byte_buffer_prepend_bytes(byte_buffer_t* buffer, unsigned char* bytes, size_t cntBytes)
{
	assert(("byte_buffer_prepend_bytes: Not Implement YET!!!", false));
}


void byte_buffer_append_buffer(byte_buffer_t* dest, byte_buffer_t* src)
{
	assert(("byte_buffer_append_buffer: Not Implement YET!!!", false));
}

void byte_buffer_prepend_buffer(byte_buffer_t* dest, byte_buffer_t* src)
{
	assert(("byte_buffer_prepend_buffer: Not Implement YET!!!", false));
}

void byte_buffer_replace_buffer(byte_buffer_t* dest, byte_buffer_t* src, size_t index)
{
	assert(("byte_buffer_replace_buffer: Not Implement YET!!!", false));
}

void byte_buffer_insert_buffer(byte_buffer_t* dest, byte_buffer_t* src, size_t index)
{
	assert(("byte_buffer_insert_buffer: Not Implement YET!!!", false));
}


//merges two buffer into a new with the size and content of buffer A and B.
byte_buffer_t* byte_buffer_join_buffer(byte_buffer_t* bufferA, byte_buffer_t* bufferB)
{
	assert(("byte_buffer_join_buffer: Not Implement YET!!!", false));
	return NULL;
}

