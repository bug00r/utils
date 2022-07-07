#include "byte_utils.h"

static void __byte_buffer_append_bytes_trunc(byte_buffer_t* _buffer, unsigned char* bytes, size_t cntBytes)
{
	byte_buffer_t* buffer = _buffer;
	size_t curOffset = buffer->offset;
	int untilEndBytes = buffer->size - curOffset;

	//not space left and TRUNCMODE stopps here
	if ( untilEndBytes == 0 ) return;

	size_t cntCopyBytes = ( untilEndBytes < cntBytes ? untilEndBytes : cntBytes);

	memcpy( buffer->buffer + curOffset, bytes, cntCopyBytes );

	buffer->offset += cntCopyBytes;
}

static void __byte_buffer_append_bytes_skip(byte_buffer_t* _buffer, unsigned char* bytes, size_t cntBytes)
{
	byte_buffer_t* buffer = _buffer;
	if ( (buffer->offset + cntBytes) >= buffer->size ) return;

	__byte_buffer_append_bytes_trunc(buffer, bytes, cntBytes);
}

static void __byte_buffer_append_bytes_ring(byte_buffer_t* _buffer, unsigned char* bytes, size_t cntBytes)
{
	byte_buffer_t* buffer = _buffer;

	for ( size_t curByte = 0; curByte < cntBytes; curByte++ )
	{
		byte_buffer_append_byte(buffer, bytes[curByte]);
	}
}


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


void byte_buffer_clear(byte_buffer_t* _buffer)
{
	byte_buffer_t* buffer = _buffer;
	byte_buffer_fill_complete(buffer, 0);
	buffer->offset = 0;
}

void byte_buffer_mode_set(byte_buffer_t* _buffer, byte_buffer_mode_t mode)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer)
	{
		buffer->mode = mode;
	}
}


byte_buffer_mode_t byte_buffer_mode_get(byte_buffer_t* buffer)
{
	return buffer->mode;
}


bool byte_buffer_is_alloc(byte_buffer_t* buffer)
{
	return buffer->alloc;
}


//adding byte or bytes to the buffer
void byte_buffer_append_byte(byte_buffer_t* _buffer, unsigned char byte)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer)
	{
		size_t usedOffset = buffer->offset;
		bool isOverflow = (usedOffset >= buffer->size);

		if (isOverflow)
		{
			switch(buffer->mode)
			{
				case BYTE_BUFFER_TRUNCATE:
				case BYTE_BUFFER_SKIP:
										return;
				case BYTE_BUFFER_RING:  usedOffset = 0;
										buffer->offset = usedOffset + 1;
										break;
				default: return;
			}
		}
		else {
			buffer->offset++;
		}
			
		buffer->buffer[usedOffset] = byte;
	}
}

void byte_buffer_append_bytes(byte_buffer_t* _buffer, unsigned char* bytes, size_t cntBytes)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer)
	{	
		switch(buffer->mode)
		{
			case BYTE_BUFFER_TRUNCATE: 
				__byte_buffer_append_bytes_trunc(buffer, bytes, cntBytes);
				break;
			case BYTE_BUFFER_SKIP: 
				__byte_buffer_append_bytes_skip(buffer, bytes, cntBytes);
				break;
			case BYTE_BUFFER_RING: 
				__byte_buffer_append_bytes_ring(buffer, bytes, cntBytes);
				break;
			default: 
				__byte_buffer_append_bytes_trunc(buffer, bytes, cntBytes);
				break;
		}

	}
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

