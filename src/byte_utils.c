#include "byte_utils.h"

static void __byte_buffer_append_bytes_trunc(ByteBuffer* _buffer, unsigned char* bytes, size_t cntBytes)
{
	ByteBuffer* buffer = _buffer;
	size_t curOffset = buffer->offset;
	size_t untilEndBytes = buffer->size - curOffset;

	//not space left and TRUNCMODE stopps here
	if ( untilEndBytes == 0 ) return;

	size_t cntCopyBytes = ( untilEndBytes < cntBytes ? untilEndBytes : cntBytes);

	memcpy( buffer->buffer + curOffset, bytes, cntCopyBytes );

	buffer->offset += cntCopyBytes;
}

static void __byte_buffer_append_bytes_skip(ByteBuffer* _buffer, unsigned char* bytes, size_t cntBytes)
{
	ByteBuffer* buffer = _buffer;
	if ( (buffer->offset + cntBytes) >= buffer->size ) return;

	__byte_buffer_append_bytes_trunc(buffer, bytes, cntBytes);
}

static void __byte_buffer_append_bytes_ring(ByteBuffer* _buffer, unsigned char* bytes, size_t cntBytes)
{
	ByteBuffer* buffer = _buffer;

	for ( size_t curByte = 0; curByte < cntBytes; curByte++ )
	{
		byte_buffer_append_byte(buffer, bytes[curByte]);
	}
}


ByteBuffer* byte_buffer_new(ByteBufferMode mode, size_t rawBuffSize)
{
	ByteBuffer* new_buf = malloc(sizeof(ByteBuffer));

	byte_buffer_init_new(new_buf, mode, rawBuffSize);

	new_buf->allocObj = true;

	return new_buf;
}

//Took outside buffer to work on it
void byte_buffer_init(ByteBuffer* _buffer, 
                      ByteBufferMode mode, 
                      unsigned char* rawBuffer, 
                      size_t rawBuffSize)
{
	ByteBuffer* buffer = _buffer;
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
void byte_buffer_init_new(ByteBuffer* _buffer, 
                          ByteBufferMode mode, 
                          size_t rawBuffSize)
{
	ByteBuffer* buffer = _buffer;
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


void byte_buffer_free(ByteBuffer** _buffer)
{
	ByteBuffer** buffer = _buffer;
	if (buffer && *buffer)
	{
		ByteBuffer* toDelete = *buffer;
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
void byte_buffer_fill_complete(ByteBuffer* _buffer, unsigned char fillByte)
{
	ByteBuffer* buffer = _buffer;
	if (buffer)
	{
		memset(buffer->buffer, fillByte, buffer->size);
	}
}

//fills the buffer from given index to the end
void byte_buffer_fill_to_end(ByteBuffer* _buffer, size_t index, unsigned char fillByte)
{
	ByteBuffer* buffer = _buffer;
	if (buffer && index < buffer->size)
	{
		memset(buffer->buffer + index, fillByte, buffer->size - index);
	}
}

//fills the buffer from start index by range with cnt bytes.
void byte_buffer_fill_range(ByteBuffer* _buffer, size_t startIndex, size_t cnt, unsigned char fillByte)
{
	ByteBuffer* buffer = _buffer;
	if (buffer && startIndex < buffer->size)
	{
		size_t alignedCnt = cnt;
		alignedCnt = ( alignedCnt + startIndex < buffer->size ? cnt : (buffer->size - startIndex));
		memset(buffer->buffer + startIndex, fillByte, alignedCnt);
	}
}


void byte_buffer_clear(ByteBuffer* _buffer)
{
	ByteBuffer* buffer = _buffer;
	byte_buffer_fill_complete(buffer, 0);
	buffer->offset = 0;
}

void byte_buffer_mode_set(ByteBuffer* _buffer, ByteBufferMode mode)
{
	ByteBuffer* buffer = _buffer;
	if (buffer)
	{
		buffer->mode = mode;
	}
}


ByteBufferMode byte_buffer_mode_get(ByteBuffer* buffer)
{
	return buffer->mode;
}


bool byte_buffer_is_alloc(ByteBuffer* buffer)
{
	return buffer->alloc;
}


//adding byte or bytes to the buffer
void byte_buffer_append_byte(ByteBuffer* _buffer, unsigned char byte)
{
	ByteBuffer* buffer = _buffer;
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

void byte_buffer_append_bytes(ByteBuffer* _buffer, unsigned char* bytes, size_t cntBytes)
{
	ByteBuffer* buffer = _buffer;
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

static void byte_buffer_append_bytes_fmt_va(ByteBuffer* buffer, unsigned char* fmt, va_list argptr)
{
	int buffsize = vsnprintf(NULL, 0, fmt, argptr) + 1;
	char * bytebuffer = malloc(buffsize);
	vsnprintf(bytebuffer, buffsize, fmt, argptr);

	byte_buffer_append_bytes(buffer, (unsigned char*)bytebuffer, buffsize-1);

	free(bytebuffer);
}

void byte_buffer_append_bytes_fmt(ByteBuffer* buffer, unsigned char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	byte_buffer_append_bytes_fmt_va(buffer, fmt, args);

	va_end(args);
}


//replace set byte or bytes from given index
void byte_buffer_replace_byte(ByteBuffer* _buffer, size_t index, unsigned char byte)
{
	ByteBuffer* buffer = _buffer;
	if (buffer)
	{	
		size_t oldOffset = buffer->offset;
		buffer->offset = index;

		byte_buffer_append_byte(buffer, byte);
		
		buffer->offset = oldOffset;
	}
}

void byte_buffer_replace_bytes(ByteBuffer* _buffer, size_t index, unsigned char* bytes, size_t cntBytes)
{
	ByteBuffer* buffer = _buffer;
	if (buffer)
	{	
		size_t oldOffset = buffer->offset;
		buffer->offset = index;

		byte_buffer_append_bytes(buffer, bytes, cntBytes);
		
		buffer->offset = oldOffset;
	}
}

void byte_buffer_replace_bytes_fmt(ByteBuffer* _buffer, size_t index, unsigned char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	ByteBuffer* buffer = _buffer;
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
void byte_buffer_insert_byte(ByteBuffer* _buffer, size_t index, unsigned char byte)
{
	ByteBuffer* buffer = _buffer;
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

void byte_buffer_insert_bytes(ByteBuffer* _buffer, size_t index, unsigned char* bytes, size_t cntBytes)
{
	ByteBuffer* buffer = _buffer;
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

static void byte_buffer_insert_bytes_fmt_va(ByteBuffer* _buffer, size_t index, unsigned char* fmt, va_list argptr)
{
	
	ByteBuffer* buffer = _buffer;
	
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

void byte_buffer_insert_bytes_fmt(ByteBuffer* _buffer, size_t index, unsigned char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	ByteBuffer* buffer = _buffer;
	
	byte_buffer_insert_bytes_fmt_va(buffer, index, fmt, args);

	va_end(args);
}

//insert byte or bytes at index 0. Moves other values based on mode.
void byte_buffer_prepend_byte(ByteBuffer* _buffer, unsigned char byte)
{
	ByteBuffer* buffer = _buffer;
	if (buffer)
	{	
		byte_buffer_insert_byte(buffer, 0, byte);
	}
}

void byte_buffer_prepend_bytes(ByteBuffer* _buffer, unsigned char* bytes, size_t cntBytes)
{
	ByteBuffer* buffer = _buffer;
	if (buffer)
	{	
		byte_buffer_insert_bytes(buffer, 0, bytes, cntBytes);
	}
}

void byte_buffer_prepend_bytes_fmt(ByteBuffer* _buffer, unsigned char* fmt, ...)
{

	va_list args;
	va_start(args, fmt);

	ByteBuffer* buffer = _buffer;
	
	byte_buffer_insert_bytes_fmt_va(buffer, 0, fmt, args);

	va_end(args);

}

void byte_buffer_append_buffer(ByteBuffer* _dest, ByteBuffer* _src)
{
	ByteBuffer* dest = _dest;
	ByteBuffer* src = _src;
	if (dest && dest)
	{	
		byte_buffer_append_bytes(dest, src->buffer, src->size);
	}
}

void byte_buffer_prepend_buffer(ByteBuffer* _dest, ByteBuffer* _src)
{
	ByteBuffer* dest = _dest;
	ByteBuffer* src = _src;
	if (dest && dest)
	{	
		byte_buffer_prepend_bytes(dest, src->buffer, src->size);
	}
}

void byte_buffer_replace_buffer(ByteBuffer* _dest, ByteBuffer* _src, size_t index)
{
	ByteBuffer* dest = _dest;
	ByteBuffer* src = _src;
	if (dest && dest)
	{	
		byte_buffer_replace_bytes(dest, index, src->buffer, src->size);
	}
}

void byte_buffer_insert_buffer(ByteBuffer* _dest, ByteBuffer* _src, size_t index)
{
	ByteBuffer* dest = _dest;
	ByteBuffer* src = _src;
	if (dest && dest)
	{	
		byte_buffer_insert_bytes(dest, index, src->buffer, src->size);
	}
}

ByteBuffer* byte_buffer_join_buffer(ByteBuffer* _bufferA, ByteBuffer* _bufferB, ByteBufferMode resultMode)
{
	ByteBuffer* bufferA = _bufferA;
	ByteBuffer* bufferB = _bufferB;
	ByteBuffer* result = NULL;

	if (bufferA && bufferB)
	{	
		result = byte_buffer_new( resultMode, bufferA->size + bufferB->size );
		byte_buffer_append_buffer(result, bufferA);
		byte_buffer_append_buffer(result, bufferB);
	}

	return result;
}

