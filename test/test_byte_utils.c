#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "defs.h"
#include "byte_utils.h"

int main(int argc, char **argv) {
	UNUSED(argc);
	UNUSED(argv);
	DEBUG_LOG(">> start byte utils test:\n");

	byte_buffer_t buffer;

	byte_buffer_free(&buffer);

	DEBUG_LOG("<< end byte utils test:\n");

	return 0;
}
