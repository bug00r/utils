#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "defs.h"
#include "byte_utils.h"

#ifdef debug
static void __test_bb_print_buffer(unsigned char* _buffer, size_t _size)
{
	unsigned char* buffer = _buffer;
	size_t size = _size;
	printf("buffer:[");

	for (size_t curIdx = 0; curIdx < size; curIdx++ )
	{
		printf("%c",buffer[curIdx]);
	}

	printf("]\n");
}
#endif

static void test_bb_init()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	unsigned char rawBuffer[20];
	size_t buffSize = 20;

	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);

	assert(buffer.buffer == &rawBuffer[0]);
	assert(buffer.size == buffSize);
	assert(buffer.offset == 0);
	assert(buffer.alloc == false);
	assert(buffer.mode == BYTE_BUFFER_TRUNCATE);

	byte_buffer_free(&buffer);

	assert(buffer.buffer == NULL);
	assert(buffer.size == 0);
	assert(buffer.offset == 0);

	byte_buffer_init_new(&buffer, BYTE_BUFFER_RING, buffSize);

	assert(buffer.buffer != NULL);
	assert(buffer.size == buffSize);
	assert(buffer.offset == 0);
	assert(buffer.alloc == true);
	assert(buffer.mode == BYTE_BUFFER_RING);

	byte_buffer_free(&buffer);

	assert(buffer.buffer == NULL);
	assert(buffer.size == 0);
	assert(buffer.offset == 0);

	DEBUG_LOG("<<<\n");
}

static void test_bb_fill_complete()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	unsigned char rawBuffer[20];
	size_t buffSize = 20;

	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);

	byte_buffer_fill_complete(&buffer, (unsigned char)'A');

	for (size_t curIdx = 0; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'A');
	}

	#ifdef debug
	printf("COMPLETE A:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	DEBUG_LOG("<<<\n");
}

static void test_bb_fill_to_end()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	
	unsigned char rawBuffer[20];
	size_t buffSize = 20;

	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);

	byte_buffer_fill_complete(&buffer, (unsigned char)'A');

	for (size_t curIdx = 0; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'A');
	}

	// Test a normal index 
	size_t fillStartIndex = 3;
	byte_buffer_fill_to_end(&buffer, fillStartIndex, (unsigned char)'B');

	for (size_t curIdx = 0; curIdx < fillStartIndex; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'A');
	}

	for (size_t curIdx = fillStartIndex; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'B');
	}

	#ifdef debug
	printf("A[0-2] B[3-]:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	//test last Index
	byte_buffer_fill_complete(&buffer, (unsigned char)'A');

	fillStartIndex = buffer.size - 1;
	byte_buffer_fill_to_end(&buffer, fillStartIndex, (unsigned char)'B');

	for (size_t curIdx = 0; curIdx < fillStartIndex; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'A');
	}

	assert(buffer.buffer[fillStartIndex] == 'B');

	#ifdef debug
	printf("A[0-N-1] B[N]:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	//Test Index Overflow
	fillStartIndex = buffer.size;
	byte_buffer_fill_to_end(&buffer, fillStartIndex, (unsigned char)'C');

	for (size_t curIdx = 0; curIdx < (fillStartIndex - 2); curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'A');
	}

	assert(buffer.buffer[fillStartIndex-1] == 'B');

	DEBUG_LOG("<<<\n");
}

static void test_bb_fill_range()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	unsigned char rawBuffer[20];
	size_t buffSize = 20;

	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);

	byte_buffer_fill_complete(&buffer, (unsigned char)'A');

	//Test fill range alinged left
	size_t startIndex = 0;
	size_t cnt = 5;
	byte_buffer_fill_range(&buffer, startIndex, cnt, (unsigned char)'B');

	size_t endIndex = startIndex + cnt;
	for (size_t curIdx = 0; curIdx < endIndex; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'B');
	}

	for (size_t curIdx = endIndex; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'A');
	}

	#ifdef debug
	printf("B[0-4]:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	//Test fill range aligned middle
	byte_buffer_fill_complete(&buffer, (unsigned char)'A');

	startIndex = 5;
	byte_buffer_fill_range(&buffer, startIndex, cnt, (unsigned char)'B');

	#ifdef debug
	printf("B[5-9]:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	endIndex = startIndex + cnt;
	for (size_t curIdx = 0; curIdx < startIndex; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'A');
	}

	for (size_t curIdx = startIndex; curIdx < endIndex; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'B');
	}

	for (size_t curIdx = endIndex; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'A');
	}

	//Test fill aligned right
	byte_buffer_fill_complete(&buffer, (unsigned char)'A');

	startIndex = buffer.size - 6; //14
	cnt = 6;
	byte_buffer_fill_range(&buffer, startIndex, cnt, (unsigned char)'B');

	endIndex = startIndex + cnt;
	for (size_t curIdx = 0; curIdx < startIndex; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'A');
	}

	for (size_t curIdx = startIndex; curIdx < endIndex; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'B');
	}

	#ifdef debug
	printf("B[14-19]:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	//Test fill range alinged left overflow
	startIndex = 0;
	cnt = 23;
	byte_buffer_fill_range(&buffer, startIndex, cnt, (unsigned char)'B');

	for (size_t curIdx = 0; curIdx <  buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'B');
	}

	#ifdef debug
	printf("B[0-N]:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	//Test fill range aligned middle overflow
	byte_buffer_fill_complete(&buffer, (unsigned char)'A');

	startIndex = 5;
	byte_buffer_fill_range(&buffer, startIndex, cnt, (unsigned char)'B');

	#ifdef debug
	printf("B[5-N]:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	endIndex = startIndex + cnt;
	for (size_t curIdx = 0; curIdx < startIndex; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'A');
	}

	for (size_t curIdx = startIndex; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'B');
	}

	//Test fill aligned right overflow
	byte_buffer_fill_complete(&buffer, (unsigned char)'A');

	startIndex = buffer.size + 6; //14
	cnt = 6;
	byte_buffer_fill_range(&buffer, startIndex, cnt, (unsigned char)'B');

	endIndex = startIndex + cnt;
	for (size_t curIdx = 0; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 'A');
	}

	#ifdef debug
	printf("A[0-N]:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	DEBUG_LOG("<<<\n");
}

static void test_bb_dummy()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	DEBUG_LOG("<<<\n");
}

int main(int argc, char **argv) {
	UNUSED(argc);
	UNUSED(argv);
	DEBUG_LOG(">> start byte utils test:\n");

	test_bb_init();

	test_bb_fill_complete();

	test_bb_fill_to_end();

	test_bb_fill_range();

	DEBUG_LOG("<< end byte utils test:\n");

	return 0;
}
