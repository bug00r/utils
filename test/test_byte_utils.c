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
		printf("%c",(buffer[curIdx] == 0 ? '0' : buffer[curIdx]));
	}

	printf("]\n");
}
#endif

static void __test_bb_equals(byte_buffer_t *_buffer, unsigned char* _testBuff)
{
	byte_buffer_t *buffer = _buffer;
	unsigned char* testBuff = _testBuff;
	for (size_t curByte = 0; curByte < buffer->size; curByte++)
	{	
		assert(buffer->buffer[curByte] == testBuff[curByte]);
	}
}

static void test_bb_init()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	unsigned char rawBuffer[20];
	size_t buffSize = 20;

	byte_buffer_t buffer;
	byte_buffer_t *buffPtr = &buffer;

	byte_buffer_init(buffPtr, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);

	assert(buffPtr->buffer == &rawBuffer[0]);
	assert(buffPtr->size == buffSize);
	assert(buffPtr->offset == 0);
	assert(buffPtr->alloc == false);
	assert(buffPtr->allocObj == false);
	assert(buffPtr->mode == BYTE_BUFFER_TRUNCATE);

	byte_buffer_free(&buffPtr);

	assert(buffPtr->buffer == NULL);
	assert(buffPtr->size == 0);
	assert(buffPtr->offset == 0);

	byte_buffer_init_new(buffPtr, BYTE_BUFFER_RING, buffSize);

	assert(buffPtr->buffer != NULL);
	assert(buffPtr->size == buffSize);
	assert(buffPtr->offset == 0);
	assert(buffPtr->alloc == true);
	assert(buffPtr->allocObj == false);
	assert(buffPtr->mode == BYTE_BUFFER_RING);

	byte_buffer_free(&buffPtr);

	assert(buffPtr->buffer == NULL);
	assert(buffPtr->size == 0);
	assert(buffPtr->offset == 0);

	byte_buffer_t *bbObj = byte_buffer_new(BYTE_BUFFER_TRUNCATE, buffSize);

	assert(bbObj->buffer != NULL);
	assert(bbObj->size == buffSize);
	assert(bbObj->offset == 0);
	assert(bbObj->alloc == true);
	assert(bbObj->allocObj == true);
	assert(bbObj->mode == BYTE_BUFFER_TRUNCATE);

	byte_buffer_free(&bbObj);

	assert(bbObj == NULL);

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

static void test_bb_clear()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	unsigned char rawBuffer[20];
	size_t buffSize = 20;

	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);

	byte_buffer_clear(&buffer);

	for (size_t curIdx = 0; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 0);
	}

	#ifdef debug
	printf("0[0-N]:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	DEBUG_LOG("<<<\n");
}

static void test_bb_state()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	unsigned char rawBuffer[20];
	size_t buffSize = 20;

	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);
	assert(byte_buffer_mode_get(&buffer) == BYTE_BUFFER_TRUNCATE);

	byte_buffer_mode_set(&buffer, BYTE_BUFFER_RING);
	assert(byte_buffer_mode_get(&buffer) == BYTE_BUFFER_RING);

	DEBUG_LOG("<<<\n");
}

static void test_bb_append_byte()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);

	byte_buffer_clear(&buffer);

	unsigned char bytes[9] = "ABCDEFGHI";
	size_t byteSize = 9;

	for (size_t curByte = 0; curByte < byteSize; curByte++)
	{
		byte_buffer_append_byte(&buffer, bytes[curByte]);
	}
	
	for (size_t curIdx = 0; curIdx < byteSize; curIdx++)
	{
		assert(buffer.buffer[curIdx] == bytes[curIdx]);
	}

	for (size_t curIdx = byteSize; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 0);
	}

	//Test Overflow TRUNCATE. After append content should not change
	byte_buffer_clear(&buffer);
	unsigned char overFlowbytes[27] = "0123456789ABCDEFGHIJKLMNOPQ";
	byteSize = 27;

	for (size_t curByte = 0; curByte < byteSize; curByte++)
	{
		byte_buffer_append_byte(&buffer, overFlowbytes[curByte]);
	}

	for (size_t curIdx = byteSize; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == overFlowbytes[curIdx]);
	}

	#ifdef debug
	printf("OF TRUNC:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	//Test Overflow SKIP. After append content should not change
	byte_buffer_mode_set(&buffer, BYTE_BUFFER_SKIP);
	byte_buffer_clear(&buffer);
	
	for (size_t curByte = 0; curByte < byteSize; curByte++)
	{
		byte_buffer_append_byte(&buffer, overFlowbytes[curByte]);
	}

	for (size_t curIdx = byteSize; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == overFlowbytes[curIdx]);
	}

	#ifdef debug
	printf("OF SKIP:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	//Test Overflow RING. After reaching end of buffer it should begin at 0;
	unsigned char ringResultbytes[20] = "KLMNOPQ789ABCDEFGHIJ";
	byte_buffer_mode_set(&buffer, BYTE_BUFFER_RING);
	byte_buffer_clear(&buffer);
	
	for (size_t curByte = 0; curByte < byteSize; curByte++)
	{
		byte_buffer_append_byte(&buffer, overFlowbytes[curByte]);
	}

	for (size_t curIdx = byteSize; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == ringResultbytes[curIdx]);
	}

	#ifdef debug
	printf("OF RING:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	DEBUG_LOG("<<<\n");
}

static void test_bb_append_bytes_trunc()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);

	byte_buffer_clear(&buffer);

	unsigned char bytes1[10] = "0123456789";
	size_t cntBytes1 = 10;
	unsigned char bytes2[6] = "ABCDEF";
	size_t cntBytes2 = 6;
	unsigned char bytes3[10] = "GHIJKLMNOP";
	size_t cntBytes3 = 10;

	byte_buffer_append_bytes(&buffer, &bytes1[0], cntBytes1);

	for (size_t curByte = 0; curByte < cntBytes1; curByte++)
	{
		assert(buffer.buffer[curByte] == bytes1[curByte]);
	}

	for (size_t curIdx = cntBytes1; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 0);
	}

	byte_buffer_append_bytes(&buffer, &bytes2[0], cntBytes2);
	
	for (size_t curByte = 0; curByte < cntBytes1; curByte++)
	{
		assert(buffer.buffer[curByte] == bytes1[curByte]);
	}

	size_t endBytes2 =  cntBytes2 + cntBytes1;
	size_t curByteBytes = 0;
	for (size_t curByte = cntBytes1; curByte < endBytes2; curByte++, curByteBytes++)
	{
		assert(buffer.buffer[curByte] == bytes2[curByteBytes]);
	}

	for (size_t curIdx = endBytes2; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 0);
	}

	#ifdef debug
	printf("Bytes 0-9A-F:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	byte_buffer_append_bytes(&buffer, &bytes3[0], cntBytes3);

		for (size_t curByte = 0; curByte < cntBytes1; curByte++)
	{
		assert(buffer.buffer[curByte] == bytes1[curByte]);
	}

	endBytes2 =  cntBytes2 + cntBytes1;
	curByteBytes = 0;
	for (size_t curByte = cntBytes1; curByte < endBytes2; curByte++, curByteBytes++)
	{
		assert(buffer.buffer[curByte] == bytes2[curByteBytes]);
	}

	curByteBytes = 0;
	for (size_t curIdx = endBytes2; curIdx < buffer.size; curIdx++, curByteBytes++)
	{
		assert(buffer.buffer[curIdx] == bytes3[curByteBytes]);
	}

	#ifdef debug
	printf("Bytes 0-9A-J:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	byte_buffer_clear(&buffer);

	byte_buffer_fill_complete(&buffer, 'A');

	byte_buffer_append_bytes_fmt(&buffer, "[%.3f]", 47.222f);

	byte_buffer_append_bytes_fmt(&buffer, "[%s]", "FU");

	#ifdef debug
	printf("FMT APPEND: ");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	__test_bb_equals(&buffer, (unsigned char *)"[47.222][FU]AAAAAAAA");

	DEBUG_LOG("<<<\n");
}

static void test_bb_append_bytes_skip()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_SKIP, &rawBuffer[0], buffSize);

	byte_buffer_clear(&buffer);

	unsigned char bytes1[10] = "0123456789";
	size_t cntBytes1 = 10;
	unsigned char bytes2[6] = "ABCDEF";
	size_t cntBytes2 = 6;
	unsigned char bytes3[10] = "GHIJKLMNOP";
	size_t cntBytes3 = 10;

	byte_buffer_append_bytes(&buffer, &bytes1[0], cntBytes1);

	for (size_t curByte = 0; curByte < cntBytes1; curByte++)
	{
		assert(buffer.buffer[curByte] == bytes1[curByte]);
	}

	for (size_t curIdx = cntBytes1; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 0);
	}

	byte_buffer_append_bytes(&buffer, &bytes2[0], cntBytes2);
	
	for (size_t curByte = 0; curByte < cntBytes1; curByte++)
	{
		assert(buffer.buffer[curByte] == bytes1[curByte]);
	}

	size_t endBytes2 =  cntBytes2 + cntBytes1;
	size_t curByteBytes = 0;
	for (size_t curByte = cntBytes1; curByte < endBytes2; curByte++, curByteBytes++)
	{
		assert(buffer.buffer[curByte] == bytes2[curByteBytes]);
	}

	for (size_t curIdx = endBytes2; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 0);
	}

	#ifdef debug
	printf("Bytes 0-9A-F:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	byte_buffer_append_bytes(&buffer, &bytes3[0], cntBytes3);

		for (size_t curByte = 0; curByte < cntBytes1; curByte++)
	{
		assert(buffer.buffer[curByte] == bytes1[curByte]);
	}

	endBytes2 =  cntBytes2 + cntBytes1;
	curByteBytes = 0;
	for (size_t curByte = cntBytes1; curByte < endBytes2; curByte++, curByteBytes++)
	{
		assert(buffer.buffer[curByte] == bytes2[curByteBytes]);
	}

	for (size_t curIdx = endBytes2; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 0);
	}

	#ifdef debug
	printf("Bytes 0-9A-F:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif


	DEBUG_LOG("<<<\n");
}

static void test_bb_append_bytes_ring()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_RING, &rawBuffer[0], buffSize);

	byte_buffer_clear(&buffer);

	unsigned char bytes1[10] = "0123456789";
	size_t cntBytes1 = 10;
	unsigned char bytes2[6] = "ABCDEF";
	size_t cntBytes2 = 6;
	unsigned char bytes3[10] = "GHIJKLMNOP";
	size_t cntBytes3 = 10;

	byte_buffer_append_bytes(&buffer, &bytes1[0], cntBytes1);

	for (size_t curByte = 0; curByte < cntBytes1; curByte++)
	{
		assert(buffer.buffer[curByte] == bytes1[curByte]);
	}

	for (size_t curIdx = cntBytes1; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 0);
	}

	byte_buffer_append_bytes(&buffer, &bytes2[0], cntBytes2);
	
	for (size_t curByte = 0; curByte < cntBytes1; curByte++)
	{
		assert(buffer.buffer[curByte] == bytes1[curByte]);
	}

	size_t endBytes2 =  cntBytes2 + cntBytes1;
	size_t curByteBytes = 0;
	for (size_t curByte = cntBytes1; curByte < endBytes2; curByte++, curByteBytes++)
	{
		assert(buffer.buffer[curByte] == bytes2[curByteBytes]);
	}

	for (size_t curIdx = endBytes2; curIdx < buffer.size; curIdx++)
	{
		assert(buffer.buffer[curIdx] == 0);
	}

	#ifdef debug
	printf("Bytes 0-9A-F:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	byte_buffer_append_bytes(&buffer, &bytes3[0], cntBytes3);

	__test_bb_equals(&buffer, (unsigned char *)"KLMNOP6789ABCDEFGHIJ");

	unsigned char ringResultBytes[20] = "KLMNOP6789ABCDEFGHIJ";

	#ifdef debug
	printf("RING bytes:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif


	DEBUG_LOG("<<<\n");
}

static void test_bb_replace_byte()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);

	byte_buffer_fill_complete(&buffer, 'A');

	byte_buffer_replace_byte(&buffer, 3, 'B');
	byte_buffer_append_byte(&buffer, 'C');
	byte_buffer_replace_byte(&buffer, 20, 'B');
	byte_buffer_replace_byte(&buffer, 9, 'B');
	byte_buffer_append_byte(&buffer, 'C');
	byte_buffer_replace_byte(&buffer, 19, 'B');
	
	__test_bb_equals(&buffer, (unsigned char *)"CCABAAAAABAAAAAAAAAB");	

	#ifdef debug
	printf("REPLACE byte:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif
	
	DEBUG_LOG("<<<\n");
}

static void test_bb_replace_bytes()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);

	unsigned char bytes1[10] = "0123456789";
	size_t cntBytes1 = 10;
	unsigned char bytes2[5] = "VWXYZ";
	size_t cntBytes2 = 5;
	
	byte_buffer_fill_complete(&buffer, 'A');

	byte_buffer_append_bytes(&buffer, &bytes1[0], cntBytes1);

	byte_buffer_replace_bytes(&buffer, 3, &bytes2[0], cntBytes2);

	byte_buffer_append_bytes(&buffer, "CCC", 3);

	#ifdef debug
	printf("REPLACE bytes:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	__test_bb_equals(&buffer, (unsigned char *)"012VWXYZ89CCCAAAAAAA");	

	byte_buffer_replace_bytes_fmt(&buffer, 3,"[%.3f]", 47.222f);

	byte_buffer_replace_bytes_fmt(&buffer, 15,"[%s]", "LOL");

	#ifdef debug
	printf("REPLACE bytes fmt:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	__test_bb_equals(&buffer, (unsigned char *)"012[47.222]CCAA[LOL]");	


	DEBUG_LOG("<<<\n");
}

static void test_bb_insert_byte()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);

	byte_buffer_fill_range(&buffer, 0, 5, 'A');
	byte_buffer_fill_range(&buffer, 5, 10, 'B');
	byte_buffer_fill_range(&buffer, 10, 15, 'C');
	byte_buffer_fill_range(&buffer, 15, 20, 'D');

	byte_buffer_insert_byte(&buffer, 0, 'E');
	byte_buffer_insert_byte(&buffer, 6, 'F');
	byte_buffer_insert_byte(&buffer, 19, 'G');
	byte_buffer_insert_byte(&buffer, 29, 'H');

	__test_bb_equals(&buffer, (unsigned char *)"EAAAAAFBBBBBCCCCCDDG");					 

	#ifdef debug
	printf("insert byte:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	DEBUG_LOG("<<<\n");
}

static void test_bb_insert_bytes()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);
	
	byte_buffer_fill_range(&buffer, 0, 5, 'A');
	byte_buffer_fill_range(&buffer, 5, 10, 'B');
	byte_buffer_fill_range(&buffer, 10, 15, 'C');
	byte_buffer_fill_range(&buffer, 15, 20, 'D');

	byte_buffer_insert_bytes(&buffer, 0, "EE", 2);
	byte_buffer_insert_bytes(&buffer, 6, "FFF", 3);
	byte_buffer_insert_bytes(&buffer, 18, "GG", 2);
	byte_buffer_insert_bytes(&buffer, 29, "HH", 2);

	__test_bb_equals(&buffer, (unsigned char *)"EEAAAAFFFABBBBBCCCGG");

	#ifdef debug
	printf("insert bytes:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	byte_buffer_insert_bytes_fmt(&buffer, 3, "[%.3f]", 47.222f);

	byte_buffer_insert_bytes_fmt(&buffer, 13, "[%s]", "TADA");

	#ifdef debug
	printf("insert bytes FMT:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	__test_bb_equals(&buffer, (unsigned char *)"EEA[47.222]AA[TADA]A");

	DEBUG_LOG("<<<\n");
}

static void test_bb_prepend_byte()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);
	
	byte_buffer_fill_range(&buffer, 0, 5, 'A');
	byte_buffer_fill_range(&buffer, 5, 10, 'B');
	byte_buffer_fill_range(&buffer, 10, 15, 'C');
	byte_buffer_fill_range(&buffer, 15, 20, 'D');

	byte_buffer_prepend_byte(&buffer, 'E');
	byte_buffer_prepend_byte(&buffer, 'F');
	byte_buffer_prepend_byte(&buffer, 'G');
	byte_buffer_prepend_byte(&buffer, 'H');

	__test_bb_equals(&buffer, (unsigned char *)"HGFEAAAAABBBBBCCCCCD");

	#ifdef debug
	printf("prepend byte:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	DEBUG_LOG("<<<\n");
}

static void test_bb_prepend_bytes()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);
	
	byte_buffer_fill_range(&buffer, 0, 5, 'A');
	byte_buffer_fill_range(&buffer, 5, 10, 'B');
	byte_buffer_fill_range(&buffer, 10, 15, 'C');
	byte_buffer_fill_range(&buffer, 15, 20, 'D');

	byte_buffer_prepend_bytes(&buffer,  (unsigned char *)"EE", 2);
	byte_buffer_prepend_bytes(&buffer,  (unsigned char *)"FF", 2);
	byte_buffer_prepend_bytes(&buffer,  (unsigned char *)"GG", 2);
	byte_buffer_prepend_bytes(&buffer,  (unsigned char *)"HH", 2);

	__test_bb_equals(&buffer, (unsigned char *)"HHGGFFEEAAAAABBBBBCC");

	byte_buffer_prepend_bytes(&buffer,  (unsigned char *)"This is a override!!", 20);

	__test_bb_equals(&buffer, (unsigned char *)"This is a override!!");

	byte_buffer_prepend_bytes(&buffer,  (unsigned char *)"This is a override with overflow!!", 34);

	__test_bb_equals(&buffer, (unsigned char *)"This is a override w");

	#ifdef debug
	printf("prepend bytes:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	byte_buffer_prepend_bytes_fmt(&buffer, "[%.3f]", 47.222f);

	byte_buffer_prepend_bytes_fmt(&buffer, "[%s]", "IN");

	#ifdef debug
	printf("prepend bytes FMT:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	__test_bb_equals(&buffer, (unsigned char *)"[IN][47.222]This is ");

	DEBUG_LOG("<<<\n");
}

static void test_bb_append_buffer()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	//Buffer1
	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);
	
	byte_buffer_fill_complete(&buffer, 'A');
	
	//buffer2
	unsigned char rawBuffer2[10];
	size_t buffSize2 = 10;
	
	byte_buffer_t buffer2;

	byte_buffer_init(&buffer2, BYTE_BUFFER_TRUNCATE, &rawBuffer2[0], buffSize2);
	
	byte_buffer_fill_complete(&buffer2, 'B');

	//buffer3
	unsigned char rawBuffer3[5];
	size_t buffSize3 = 5;
	
	byte_buffer_t buffer3;

	byte_buffer_init(&buffer3, BYTE_BUFFER_TRUNCATE, &rawBuffer3[0], buffSize3);
	
	byte_buffer_fill_complete(&buffer3, 'C');


	byte_buffer_append_buffer(&buffer, &buffer2);
	byte_buffer_append_buffer(&buffer, &buffer3);

	__test_bb_equals(&buffer, (unsigned char *)"BBBBBBBBBBCCCCCAAAAA");

	#ifdef debug
	printf("append buff:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif

	DEBUG_LOG("<<<\n");
}

static void test_bb_prepend_buffer()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	//Buffer1
	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);
	
	byte_buffer_fill_complete(&buffer, 'A');
	
	//buffer2
	unsigned char rawBuffer2[10];
	size_t buffSize2 = 10;
	
	byte_buffer_t buffer2;

	byte_buffer_init(&buffer2, BYTE_BUFFER_TRUNCATE, &rawBuffer2[0], buffSize2);
	
	byte_buffer_fill_complete(&buffer2, 'B');

	//buffer3
	unsigned char rawBuffer3[5];
	size_t buffSize3 = 5;
	
	byte_buffer_t buffer3;

	byte_buffer_init(&buffer3, BYTE_BUFFER_TRUNCATE, &rawBuffer3[0], buffSize3);
	
	byte_buffer_fill_complete(&buffer3, 'C');


	byte_buffer_prepend_buffer(&buffer, &buffer2);
	byte_buffer_prepend_buffer(&buffer, &buffer3);

	__test_bb_equals(&buffer, (unsigned char *)"CCCCCBBBBBBBBBBAAAAA");

	#ifdef debug
	printf("prepend buff:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif
	
	DEBUG_LOG("<<<\n");
}

static void test_bb_replace_buffer()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	//Buffer1
	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);
	
	byte_buffer_fill_complete(&buffer, 'A');
	
	//buffer2
	unsigned char rawBuffer2[10];
	size_t buffSize2 = 10;
	
	byte_buffer_t buffer2;

	byte_buffer_init(&buffer2, BYTE_BUFFER_TRUNCATE, &rawBuffer2[0], buffSize2);
	
	byte_buffer_fill_complete(&buffer2, 'B');

	//buffer3
	unsigned char rawBuffer3[5];
	size_t buffSize3 = 5;
	
	byte_buffer_t buffer3;

	byte_buffer_init(&buffer3, BYTE_BUFFER_TRUNCATE, &rawBuffer3[0], buffSize3);
	
	byte_buffer_fill_complete(&buffer3, 'C');


	byte_buffer_replace_buffer(&buffer, &buffer2, 4);
	byte_buffer_replace_buffer(&buffer, &buffer3, 6);

	__test_bb_equals(&buffer, (unsigned char *)"AAAABBCCCCCBBBAAAAAA");

	#ifdef debug
	printf("replace buff:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif
	
	DEBUG_LOG("<<<\n");
}

static void test_bb_insert_buffer()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	//Buffer1
	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);
	
	byte_buffer_fill_complete(&buffer, 'A');
	
	//buffer2
	unsigned char rawBuffer2[10];
	size_t buffSize2 = 10;
	
	byte_buffer_t buffer2;

	byte_buffer_init(&buffer2, BYTE_BUFFER_TRUNCATE, &rawBuffer2[0], buffSize2);
	
	byte_buffer_fill_complete(&buffer2, 'B');

	//buffer3
	unsigned char rawBuffer3[5];
	size_t buffSize3 = 5;
	
	byte_buffer_t buffer3;

	byte_buffer_init(&buffer3, BYTE_BUFFER_TRUNCATE, &rawBuffer3[0], buffSize3);
	
	byte_buffer_fill_complete(&buffer3, 'C');

	byte_buffer_insert_buffer(&buffer, &buffer2, 0);
	byte_buffer_insert_buffer(&buffer, &buffer3, 3);

	__test_bb_equals(&buffer, (unsigned char *)"BBBCCCCCBBBBBBBAAAAA");

	#ifdef debug
	printf("insert buff:");
	__test_bb_print_buffer(&rawBuffer[0], buffSize);
	#endif
	
	DEBUG_LOG("<<<\n");
}

static void test_bb_join_buffer()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	//Buffer1
	unsigned char rawBuffer[20];
	size_t buffSize = 20;
	
	byte_buffer_t buffer;

	byte_buffer_init(&buffer, BYTE_BUFFER_TRUNCATE, &rawBuffer[0], buffSize);
	
	byte_buffer_fill_complete(&buffer, 'A');
	
	//buffer2
	unsigned char rawBuffer2[10];
	size_t buffSize2 = 10;
	
	byte_buffer_t buffer2;

	byte_buffer_init(&buffer2, BYTE_BUFFER_TRUNCATE, &rawBuffer2[0], buffSize2);
	
	byte_buffer_fill_complete(&buffer2, 'B');

	//joining buffer
	byte_buffer_t *joined = byte_buffer_join_buffer(&buffer, &buffer2, BYTE_BUFFER_RING);

	assert(joined != NULL);
	assert(joined->buffer != NULL);
	assert(joined->size == 30);
	assert(joined->offset == 30);
	assert(joined->alloc == true);
	assert(joined->allocObj == true);

	__test_bb_equals(joined, (unsigned char *)"AAAAAAAAAAAAAAAAAAAABBBBBBBBBB");

	#ifdef debug
	printf("join buff:");
	__test_bb_print_buffer(joined->buffer, joined->size);
	#endif

	byte_buffer_free(&joined);

	assert(joined == NULL);

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

	test_bb_clear();

	test_bb_state();

	test_bb_append_byte();

	test_bb_append_bytes_trunc();

	test_bb_append_bytes_skip();

	test_bb_append_bytes_ring();

	test_bb_replace_byte();

	test_bb_replace_bytes();

	test_bb_insert_byte();

	test_bb_insert_bytes();

	test_bb_prepend_byte();

	test_bb_prepend_bytes();

	test_bb_append_buffer();

	test_bb_prepend_buffer();

	test_bb_replace_buffer();

	test_bb_insert_buffer();

	test_bb_join_buffer();

	DEBUG_LOG("<< end byte utils test:\n");

	return 0;
}
