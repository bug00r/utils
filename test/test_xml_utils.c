#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "defs.h"
#include "xml_source.h"

#include "xml_utils.h"

EXTERN_BLOB(zip_resource, 7z);

#ifndef DEBUG_LOG_ARGS
	#if debug != 0
		#define DEBUG_LOG_ARGS(fmt, ...) printf((fmt), __VA_ARGS__)
	#else
		#define DEBUG_LOG_ARGS(fmt, ...)
	#endif
#endif

#ifndef DEBUG_LOG
	#if debug != 0
		#define DEBUG_LOG(msg) printf((msg))
	#else
		#define DEBUG_LOG(msg)
	#endif
#endif

static void test_xml_ctx_empty() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	xml_ctx_t *nCtx = xml_ctx_new_empty();

	assert(nCtx->src == NULL);
	assert(nCtx->doc != NULL);
	
	#if debug > 0
		int writtenbytes = xmlSaveFileEnc("-", nCtx->doc,"UTF-8");
		assert(writtenbytes == 39);
	#endif

	free_xml_ctx_src(&nCtx);

	assert(nCtx == NULL);

	nCtx = xml_ctx_new_empty_root_name("heros");

	assert(nCtx->src == NULL);
	assert(nCtx->doc != NULL);
	
	#if debug > 0
		writtenbytes = xmlSaveFileEnc("-", nCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
		assert(writtenbytes == 48);
	#endif

	xmlNodePtr nroot = xmlDocGetRootElement(nCtx->doc);

	xml_ctx_t * nodectx = xml_ctx_new_node(nroot);

	xmlNodePtr noderoot = xmlDocGetRootElement(nodectx->doc);

	assert(nroot != noderoot);
	assert(strcmp((const char *)noderoot->name, (const char *)nroot->name) == 0);

	#if debug > 0
		writtenbytes = xmlSaveFileEnc("-", nCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> from node copy BYTES %i\n", writtenbytes);
		assert(writtenbytes == 48);
	#endif

	free_xml_ctx_src(&nCtx);

	free_xml_ctx_src(&nodectx);

	assert(nCtx == NULL);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_file() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	xml_ctx_t *nCtx = xml_ctx_new_file("data\\xml\\basehero.xml");

	assert(nCtx->src == NULL);
	assert(nCtx->doc != NULL);

	#if debug > 1
		int writtenbytes = xmlSaveFileEnc("-", nCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
	#endif

	const char *file = "build\\cc\\dumpbasehero.xml";

	xml_ctx_save_file(nCtx, file);

	free_xml_ctx_src(&nCtx);

	assert(nCtx == NULL);


	nCtx = xml_ctx_new_file(file);

	assert(nCtx->src == NULL);
	assert(nCtx->doc != NULL);

	#if debug > 1
		DEBUG_LOG("WRITTEN FILE:\n");
		writtenbytes = xmlSaveFileEnc("-", nCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
	#endif

	free_xml_ctx_src(&nCtx);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_extra_src() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	xml_source_t* result = xml_source_from_resname(ar, "talents");

	assert(result != NULL);

	DEBUG_LOG_ARGS(">>> file type => %s\n", result->data.resfile->type);

	xml_ctx_t *nCtx = xml_ctx_new(result);
	
	assert(nCtx->doc != NULL);
	assert(nCtx->src != NULL && nCtx->src == result);
	assert(nCtx->state.state_no == XML_CTX_SUCCESS);
	assert(nCtx->state.reason == XML_CTX_READ_AND_PARSE);

	free_xml_ctx(&nCtx);

	assert(nCtx == NULL && result != NULL);

	xml_source_free(&result);

	assert(result == NULL);


	result = xml_source_from_resname(ar, "notfound");
	
	nCtx = xml_ctx_new(result);

	assert(nCtx != NULL);
	assert(nCtx->src == NULL && nCtx->src == result);
	assert(nCtx->doc == NULL);
	assert(nCtx->state.state_no == XML_CTX_ERROR);
	assert(nCtx->state.reason == XML_CTX_READ_AND_PARSE);

	free_xml_ctx(&nCtx);
	xml_source_free(&result);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_incl_src() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);

	xml_source_t* result = xml_source_from_resname(ar, "talents");

	xml_ctx_t *nCtx = xml_ctx_new(result);
	
	assert(nCtx->doc != NULL);
	assert(nCtx->src != NULL && nCtx->src == result);
	assert(nCtx->state.state_no == XML_CTX_SUCCESS);
	assert(nCtx->state.reason == XML_CTX_READ_AND_PARSE);

	free_xml_ctx_src(&nCtx);

	assert(nCtx == NULL); //Be careful with result at this point, i was freed and become a dangling pointer

	result = xml_source_from_resname(ar, "notfound");
	
	nCtx = xml_ctx_new(result);

	assert(nCtx != NULL);
	assert(nCtx->src == NULL && nCtx->src == result);
	assert(nCtx->doc == NULL);
	assert(nCtx->state.state_no == XML_CTX_ERROR);
	assert(nCtx->state.reason == XML_CTX_READ_AND_PARSE);

	free_xml_ctx_src(&nCtx);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void __debug_xpath_obj_ptr(xmlXPathObjectPtr xpathObj) {
	if ( xpathObj != NULL ) {
		xmlNodeSetPtr nodes = xpathObj->nodesetval;
		int size = (nodes) ? nodes->nodeNr : 0;
		xmlNodePtr cur;
		for(int i = 0; i < size; ++i) {
			cur = nodes->nodeTab[i];
			
			//single property example
			//xmlChar *attr = xmlGetProp(cur, "name");
			//DEBUG_LOG_ARGS("= node \"%s\": type %d : content: %s\n", cur->name, cur->type, attr);
			//xmlFree(attr);
			if(cur == NULL) continue;
			
			xmlAttr * attr = cur->properties;
			
			DEBUG_LOG_ARGS("= node \"%s\": type %d - ", cur->name, cur->type);
			
			while(attr != NULL) {
				xmlChar *sattr = xmlGetProp(cur, attr->name);
				if (sattr != NULL) {
					DEBUG_LOG_ARGS("%s / %s ", attr->name, sattr);
					xmlFree(sattr);
				}
				attr = attr->next;
			}
			DEBUG_LOG("\n");
		}
	}
}

static void test_xml_ctx_xpath() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	xml_source_t* result = xml_source_from_resname(ar, "talents");
	xml_ctx_t *nCtx = xml_ctx_new(result);

	const char *xpath = "//group[@name = 'Kampf']/*[regexmatch(@name,'Dolche')]";

	xmlXPathObjectPtr xpathObj = xml_ctx_xpath(nCtx, xpath);

	__debug_xpath_obj_ptr(xpathObj);

	xmlXPathObjectPtr xpathObj2 = xml_ctx_xpath(nCtx, xpath);

	assert(xpathObj->nodesetval->nodeTab[0] == xpathObj2->nodesetval->nodeTab[0]);
	DEBUG_LOG_ARGS(">>> %p => %p\n", xpathObj->nodesetval->nodeTab[0] , xpathObj2->nodesetval->nodeTab[0]);

	xmlXPathFreeObject(xpathObj);
	xmlXPathFreeObject(xpathObj2);
	free_xml_ctx_src(&nCtx);
	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_xpath_format() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	xml_source_t* result = xml_source_from_resname(ar, "talents");
	xml_ctx_t *nCtx = xml_ctx_new(result);

	xmlXPathObjectPtr xpathObj = xml_ctx_xpath_format(nCtx, "//group[@name = '%s']/*[regexmatch(@name,'%s')]", "Kampf", "Dolche");

	assert(xml_xpath_has_result(xpathObj));

	__debug_xpath_obj_ptr(xpathObj);

	xmlXPathFreeObject(xpathObj);

	assert(xml_ctx_exist(nCtx, "//group[@name = 'Kampf']/*[regexmatch(@name,'Dolche')]"));
	assert(!xml_ctx_exist(nCtx, "//group[@name = 'Natur']/*[regexmatch(@name,'Dolche')]"));

	assert(xml_ctx_exist_format(nCtx, "//group[@name = '%s']/*[regexmatch(@name,'%s')]", "Kampf", "Dolche"));
	assert(!xml_ctx_exist_format(nCtx, "//group[@name = '%s']/*[regexmatch(@name,'%s')]", "Natur", "Dolche"));

	free_xml_ctx_src(&nCtx);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_add_node_xpath() {
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	xml_source_t* result = xml_source_from_resname(ar, "basehero");
	xml_ctx_t *nCtx = xml_ctx_new(result);

	xml_ctx_t *hCtx = xml_ctx_new_empty_root_name("heros");

	xml_ctx_nodes_add_xpath(nCtx, "/hero", hCtx, "/heros");

	#if debug > 1
		int writtenbytes = xmlSaveFileEnc("-", hCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
	#endif

	free_xml_ctx_src(&nCtx);

	result = xml_source_from_resname(ar, "breeds");
	nCtx = xml_ctx_new(result);

	xml_ctx_nodes_add_xpath(nCtx, "/breeds//breed[@name = 'Die Tulamiden']", hCtx, "/heros/hero/breedcontainer");

	#if debug > 1
		writtenbytes = xmlSaveFileEnc("-", hCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
	#endif

	xml_ctx_nodes_add_xpath(nCtx, "/breeds//breed[@name = 'Die Tulamiden']//culture", hCtx, "/heros/*//group");

	#if debug > 1
		writtenbytes = xmlSaveFileEnc("-", hCtx->doc,"UTF-8");
		DEBUG_LOG_ARGS(">>> BYTES %i\n", writtenbytes);
	#endif

	xml_ctx_set_attr_str_xpath(hCtx, (unsigned char *)"New Name Baradon", "//hero/@description");
	xml_ctx_set_attr_str_xpath(hCtx, (unsigned char *)"150", "//hero/config/base-gp/@value");

	xml_ctx_set_attr_str_xpath_format(hCtx, (unsigned char *)"10", "//hero/attributes//attribute[regexmatch(@name,'%s')]/@value", "[KkSs]*.*");

	xml_ctx_set_content_xpath(hCtx, (unsigned char *)"Ups falscher Text", "//hero/story/text()");
	xml_ctx_set_content_xpath(hCtx, (unsigned char *)"Das ist die wahre Story von Baradon", "//hero/story/text()");
	xml_ctx_set_content_xpath(hCtx, (unsigned char *)"Ganz neuer Look wa!!!", "//hero/look/text()");

	xml_ctx_set_content_xpath_format(hCtx, (unsigned char *)"ganz neuer Titel", "//hero/%s/text()", "title");

	#if debug > 1
		xmlSaveFileEnc("-", hCtx->doc,"UTF-8");
	#endif

	free_xml_ctx_src(&nCtx);
	free_xml_ctx_src(&hCtx);

	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void __search_and_dum_range_assert(xml_ctx_t *ctx, const char *xpath, bool assert_val) {
	
	xmlXPathObjectPtr colres = xml_ctx_xpath(ctx, xpath);

	assert(xml_xpath_has_result(colres) == assert_val);

	#if debug > 1
		const int maxNodes = colres->nodesetval->nodeNr;
		xmlNodePtr *nodes = colres->nodesetval->nodeTab;
		printf("%s found: %i \n", xpath, maxNodes);
		for(int curNode = 0; curNode < maxNodes; ++curNode) {
			xmlChar * val = xmlGetProp(nodes[curNode],(xmlChar*)"value");
			xmlChar * name = xmlGetProp(nodes[curNode],(xmlChar*)"name");
			printf("- %s %s\n", name, val);
			xmlFree(val);
			xmlFree(name);
		}
	#endif

	xmlXPathFreeObject(colres);
}

static void test_xml_ctx_xpath_in_range() 
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);
	
	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	xml_source_t* result = xml_source_from_resname(ar, "breeds");
	xml_ctx_t *nCtx = xml_ctx_new(result);

	__search_and_dum_range_assert(nCtx, "/breeds/*//color[in_range(@value,'12')]", true);

	__search_and_dum_range_assert(nCtx, "/breeds/*//color[in_range(@value,'4')]", true);

	__search_and_dum_range_assert(nCtx, "/breeds/*//color[in_range(@value,'22')]", false);

	free_xml_ctx_src(&nCtx);
	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

/*
int xml_ctx_strtof(xmlChar *str, float *result);
int xml_ctx_strtol(xmlChar *str, long *result);
*/

static void test_xml_ctx_xmlstr_to_float()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	xmlChar * val =(xmlChar*)"3.41";
	float result = .0f;
	int err = xml_ctx_strtof(val, &result);

	assert(err == 0);
	assert(result == 3.41f);

	xmlChar * val2 = (xmlChar*)"ABCD";
	result = .0f;
	err = xml_ctx_strtof(val2, &result);

	assert(err == 0);
	assert(result == 0.f);

	xmlChar * val3 = (xmlChar*)"-3.4133";
	result = .0f;
	err = xml_ctx_strtof(val3, &result);

	assert(err == 0);
	assert(result == -3.4133f);

	xmlChar * val4 = (xmlChar*)"33333333333333333333333333333333333333333333333333333333333333333333333333333333.4133";
	result = .0f;
	err = xml_ctx_strtof(val4, &result);

	assert(err == 1);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_xmlstr_to_long()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	xmlChar * val = (xmlChar*)"341";
	long result = 0;
	int err = xml_ctx_strtol(val, &result);

	assert(err == 0);
	assert(result == 341);

	xmlChar * val2 = (xmlChar*)"ABCD";
	result = 0;
	err = xml_ctx_strtol(val2, &result);

	assert(err == 0);
	assert(result == 0);

	xmlChar * val3 = (xmlChar*)"-34133";
	result = 0;
	err = xml_ctx_strtol(val3, &result);

	assert(err == 0);
	assert(result == -34133);

	xmlChar * val4 = (xmlChar*)"33333333333333333333333333333333333333333333333333333333333333333333333333333333";
	result = 0;
	err = xml_ctx_strtol(val4, &result);

	assert(err == 1);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_xpath_to_double()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	xml_source_t* result = xml_source_from_resname(ar, "basehero");
	xml_ctx_t *nCtx = xml_ctx_new(result);

	double dResult = 0;
	int errNo = xml_ctx_xpath_tod(nCtx, &dResult, "//hero/config/base-gp/@value");

	assert(errNo == 0);
	assert(dResult == 110.0);

	errNo = xml_ctx_xpath_tod(nCtx, &dResult, "//hero/@age");

	assert(errNo == 0);
	assert(dResult == 20.0);

	errNo = xml_ctx_xpath_tod(nCtx, &dResult, "sum(//hero/attributes/attribute/@value)");

	assert(errNo == 0);
	assert(dResult == 64.0);

	errNo = xml_ctx_xpath_tod(nCtx, &dResult, "//hero/attributes/attribute");
	assert(errNo == 1);

	free_xml_ctx_src(&nCtx);
	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_xpath_to_long()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	xml_source_t* result = xml_source_from_resname(ar, "basehero");
	xml_ctx_t *nCtx = xml_ctx_new(result);

	long lResult = 0;
	int errNo = xml_ctx_xpath_tol(nCtx, &lResult, "//hero/config/base-gp/@value");

	assert(errNo == 0);
	assert(lResult == 110);

	errNo = xml_ctx_xpath_tol(nCtx, &lResult, "//hero/@age");

	assert(errNo == 0);
	assert(lResult == 20);

	errNo = xml_ctx_xpath_tol(nCtx, &lResult, "sum(//hero/attributes/attribute/@value)");

	assert(errNo == 0);
	assert(lResult == 64);

	errNo = xml_ctx_xpath_tol(nCtx, &lResult, "//hero/attributes/attribute");
	assert(errNo == 1);

	free_xml_ctx_src(&nCtx);
	archive_resource_free(&ar);


	DEBUG_LOG("<<<\n");
}

static void test_xml_ctx_xpath_to_float()
{
	DEBUG_LOG_ARGS(">>> %s => %s\n", __FILE__, __func__);

	archive_resource_t* ar = archive_resource_memory(&_binary_zip_resource_7z_start, (size_t)&_binary_zip_resource_7z_size);
	xml_source_t* result = xml_source_from_resname(ar, "basehero");
	xml_ctx_t *nCtx = xml_ctx_new(result);

	float fResult = 0;
	int errNo = xml_ctx_xpath_tof(nCtx, &fResult, "//hero/config/base-gp/@value");

	assert(errNo == 0);
	assert(fResult == 110.f);

	errNo = xml_ctx_xpath_tof(nCtx, &fResult, "//hero/@age");

	assert(errNo == 0);
	assert(fResult == 20.f);

	errNo = xml_ctx_xpath_tof(nCtx, &fResult, "sum(//hero/attributes/attribute/@value)");

	assert(errNo == 0);
	assert(fResult == 64.f);

	errNo = xml_ctx_xpath_tof(nCtx, &fResult, "//hero/attributes/attribute");
	assert(errNo == 1);

	free_xml_ctx_src(&nCtx);
	archive_resource_free(&ar);

	DEBUG_LOG("<<<\n");
}

int 
main() 
{

	DEBUG_LOG(">> Start xml utils tests:\n");
	
	test_xml_ctx_empty();

	test_xml_ctx_file();

	test_xml_ctx_extra_src();

	test_xml_ctx_incl_src();

	test_xml_ctx_xpath();

	test_xml_ctx_xpath_format();

	test_xml_ctx_add_node_xpath();

	test_xml_ctx_xpath_in_range();

	test_xml_ctx_xmlstr_to_float();

	test_xml_ctx_xmlstr_to_long();

	test_xml_ctx_xpath_to_double();

	test_xml_ctx_xpath_to_long();

	test_xml_ctx_xpath_to_float();

	DEBUG_LOG("<< end xml utils tests:\n");

	return 0;
}

