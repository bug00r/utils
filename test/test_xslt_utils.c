#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "defs.h"
#include "xml_source.h"
#include "xml_utils.h"
#include "xslt_utils.h"

EXTERN_BLOB(zip_resource, 7z);

int 
main() 
{

	DEBUG_LOG(">> Start xslt utils tests:\n");
	
	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	xml_source_t* input = xml_source_from_resname(ar, "breeds");

	assert(input != NULL);

	DEBUG_LOG_ARGS(">>> file type => %s\n", input->data.resfile->type);

	xml_ctx_t *input_ctx = xml_ctx_new(input);

	xml_source_t* stylesheet = xml_source_from_resname_full(ar, "xslt/","test_breed", "xsl");

	assert(stylesheet != NULL);

	DEBUG_LOG_ARGS(">>> file type => %s\n", stylesheet->data.resfile->type);

	xml_ctx_t *sheet_ctx = xml_ctx_new(stylesheet);

	xslt_ctx_t xslt_ctx;
	xslt_ctx_init(&xslt_ctx);

	xslt_ctx.xml = input_ctx;
	xslt_ctx.stylesheet = sheet_ctx;

	do_xslt(&xslt_ctx);
	xslt_print_err(&xslt_ctx);
	xslt_ctx_cleanup(&xslt_ctx);


	free_xml_ctx(&sheet_ctx);
	free_xml_ctx(&input_ctx);

	assert(sheet_ctx == NULL && input_ctx == NULL);

	xml_source_free(&input);
	xml_source_free(&stylesheet);

	archive_resource_free(&ar);

	DEBUG_LOG("<< end xslt utils tests:\n");

	return 0;
}

