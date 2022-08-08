#include "byte_utils.h"

static void __byte_buffer_append_bytes_trunc(byte_buffer_t* _buffer, unsigned char* bytes, size_t cntBytes)
{
	byte_buffer_t* buffer = _buffer;
	size_t curOffset = buffer->offset;
	size_t untilEndBytes = buffer->size - curOffset;

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


byte_buffer_t* byte_buffer_new(byte_buffer_mode_t mode, size_t rawBuffSize)
{
	byte_buffer_t* new_buf = malloc(sizeof(byte_buffer_t));

	byte_buffer_init_new(new_buf, mode, rawBuffSize);

	new_buf->allocObj = true;

	return new_buf;
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
		buffer->allocObj = false;
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
		buffer->allocObj = false;
		buffer->mode = mode;
		buffer->offset = 0;
		buffer->size = rawBuffSize;
		buffer->buffer = malloc(rawBuffSize * sizeof(unsigned char));
	}
}


void byte_buffer_free(byte_buffer_t** _buffer)
{
	byte_buffer_t** buffer = _buffer;
	if (buffer && *buffer)
	{
		byte_buffer_t* toDelete = *buffer;
		if (toDelete->alloc)
		{
			free(toDelete->buffer);
		}

		toDelete->buffer = NULL;
		toDelete->size = 0;
		toDelete->offset = 0;

		if (toDelete->allocObj)
		{
			free(toDelete);
			*buffer = NULL;
		}
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

static void byte_buffer_append_bytes_fmt_va(byte_buffer_t* buffer, unsigned char* fmt, va_list argptr)
{
	int buffsize = vsnprintf(NULL, 0, fmt, argptr) + 1;
	char * bytebuffer = malloc(buffsize);
	vsnprintf(bytebuffer, buffsize, fmt, argptr);

	byte_buffer_append_bytes(buffer, (unsigned char*)bytebuffer, buffsize-1);

	free(bytebuffer);
}

void byte_buffer_append_bytes_fmt(byte_buffer_t* buffer, unsigned char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	byte_buffer_append_bytes_fmt_va(buffer, fmt, args);

	va_end(args);
}


//replace set byte or bytes from given index
void byte_buffer_replace_byte(byte_buffer_t* _buffer, size_t index, unsigned char byte)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer)
	{	
		size_t oldOffset = buffer->offset;
		buffer->offset = index;

		byte_buffer_append_byte(buffer, byte);
		
		buffer->offset = oldOffset;
	}
}

void byte_buffer_replace_bytes(byte_buffer_t* _buffer, size_t index, unsigned char* bytes, size_t cntBytes)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer)
	{	
		size_t oldOffset = buffer->offset;
		buffer->offset = index;

		byte_buffer_append_bytes(buffer, bytes, cntBytes);
		
		buffer->offset = oldOffset;
	}
}

void byte_buffer_replace_bytes_fmt(byte_buffer_t* _buffer, size_t index, unsigned char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	byte_buffer_t* buffer = _buffer;
	if (buffer)
	{	
		size_t oldOffset = buffer->offset;
		buffer->offset = index;

		byte_buffer_append_bytes_fmt_va(buffer, fmt, args);

		buffer->offset = oldOffset;
	}

	va_end(args);
}

//insert set byte or bytes at given index. Moves other values based on mode.
void byte_buffer_insert_byte(byte_buffer_t* _buffer, size_t index, unsigned char byte)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer && index < buffer->size)
	{	
		size_t oldOffset = buffer->offset;
		buffer->offset = index;

		size_t restByteCnt = buffer->size - buffer->offset;
		unsigned char *restBytes = malloc(restByteCnt * sizeof(unsigned char));
		
		memcpy(restBytes, buffer->buffer + buffer->offset, restByteCnt);

		byte_buffer_append_byte(buffer, byte);
		
		byte_buffer_append_bytes(buffer, restBytes, restByteCnt);

		free(restBytes);

		buffer->offset = oldOffset;
	}
}

void byte_buffer_insert_bytes(byte_buffer_t* _buffer, size_t index, unsigned char* bytes, size_t cntBytes)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer && index < buffer->size)
	{	
		size_t oldOffset = buffer->offset;
		buffer->offset = index;

		size_t restByteCnt = buffer->size - buffer->offset;
		unsigned char *restBytes = malloc(restByteCnt * sizeof(unsigned char));
		
		memcpy(restBytes, buffer->buffer + buffer->offset, restByteCnt);

		byte_buffer_append_bytes(buffer, bytes, cntBytes);
		byte_buffer_append_bytes(buffer, restBytes, restByteCnt);

		free(restBytes);

		buffer->offset = oldOffset;
	}
}

static void byte_buffer_insert_bytes_fmt_va(byte_buffer_t* _buffer, size_t index, unsigned char* fmt, va_list argptr)
{
	
	byte_buffer_t* buffer = _buffer;
	
	if (buffer && index < buffer->size)
	{	
		size_t oldOffset = buffer->offset;
		buffer->offset = index;

		size_t restByteCnt = buffer->size - buffer->offset;
		unsigned char *restBytes = malloc(restByteCnt * sizeof(unsigned char));
		
		memcpy(restBytes, buffer->buffer + buffer->offset, restByteCnt);

		byte_buffer_append_bytes_fmt_va(buffer, fmt, argptr);
		byte_buffer_append_bytes(buffer, restBytes, restByteCnt);

		free(restBytes);

		buffer->offset = oldOffset;
	}
}

void byte_buffer_insert_bytes_fmt(byte_buffer_t* _buffer, size_t index, unsigned char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	byte_buffer_t* buffer = _buffer;
	
	byte_buffer_insert_bytes_fmt_va(buffer, index, fmt, args);

	va_end(args);
}

//insert byte or bytes at index 0. Moves other values based on mode.
void byte_buffer_prepend_byte(byte_buffer_t* _buffer, unsigned char byte)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer)
	{	
		byte_buffer_insert_byte(buffer, 0, byte);
	}
}

void byte_buffer_prepend_bytes(byte_buffer_t* _buffer, unsigned char* bytes, size_t cntBytes)
{
	byte_buffer_t* buffer = _buffer;
	if (buffer)
	{	
		byte_buffer_insert_bytes(buffer, 0, bytes, cntBytes);
	}
}

void byte_buffer_prepend_bytes_fmt(byte_buffer_t* _buffer, unsigned char* fmt, ...)
{

	va_list args;
	va_start(args, fmt);

	byte_buffer_t* buffer = _buffer;
	
	byte_buffer_insert_bytes_fmt_va(buffer, 0, fmt, args);

	va_end(args);

}

void byte_buffer_append_buffer(byte_buffer_t* _dest, byte_buffer_t* _src)
{
	byte_buffer_t* dest = _dest;
	byte_buffer_t* src = _src;
	if (dest && dest)
	{	
		byte_buffer_append_bytes(dest, src->buffer, src->size);
	}
}

void byte_buffer_prepend_buffer(byte_buffer_t* _dest, byte_buffer_t* _src)
{
	byte_buffer_t* dest = _dest;
	byte_buffer_t* src = _src;
	if (dest && dest)
	{	
		byte_buffer_prepend_bytes(dest, src->buffer, src->size);
	}
}

void byte_buffer_replace_buffer(byte_buffer_t* _dest, byte_buffer_t* _src, size_t index)
{
	byte_buffer_t* dest = _dest;
	byte_buffer_t* src = _src;
	if (dest && dest)
	{	
		byte_buffer_replace_bytes(dest, index, src->buffer, src->size);
	}
}

void byte_buffer_insert_buffer(byte_buffer_t* _dest, byte_buffer_t* _src, size_t index)
{
	byte_buffer_t* dest = _dest;
	byte_buffer_t* src = _src;
	if (dest && dest)
	{	
		byte_buffer_insert_bytes(dest, index, src->buffer, src->size);
	}
}

byte_buffer_t* byte_buffer_join_buffer(byte_buffer_t* _bufferA, byte_buffer_t* _bufferB, byte_buffer_mode_t resultMode)
{
	byte_buffer_t* bufferA = _bufferA;
	byte_buffer_t* bufferB = _bufferB;
	byte_buffer_t* result = NULL;

	if (bufferA && bufferB)
	{	
		result = byte_buffer_new( resultMode, bufferA->size + bufferB->size );
		byte_buffer_append_buffer(result, bufferA);
		byte_buffer_append_buffer(result, bufferB);
	}

	return result;
}

