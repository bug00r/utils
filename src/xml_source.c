#include "xml_source.h"

static xml_source_t* xml_source_new(xml_source_type_t type, resource_file_t *res_file) {
    
    xml_source_t* newxml_source = NULL;

    xml_source_t _tmp_newxml_source = { type, &res_file->file_size, res_file->data, { res_file } };

    newxml_source = malloc(sizeof(xml_source_t));
    
    memcpy(newxml_source, &_tmp_newxml_source, sizeof(xml_source_t));
    
    return newxml_source;
}

static xml_source_t* _xml_source_from_res_search(archive_resource_t* ar, const char *searchname) {
    
    xml_source_t *result = NULL;

    resource_search_result_t* searchresult = archive_resource_search_by_name(ar, (const unsigned char *)searchname);

    if ( searchresult->cnt == 1 ) {
        result = xml_source_new(RESOURCE_FILE, searchresult->files[0]);
    }

    resource_search_result_free(&searchresult);

    return result;
}

xml_source_t* xml_source_from_resname(archive_resource_t* ar, const char *name) {
    
    xml_source_t *result = NULL;

    if (ar != NULL && name != NULL) {
        char * searchname = format_string_new("xml/%s.xml", name);
        result = _xml_source_from_res_search(ar, searchname);
        free(searchname);
    }

    return result;
}

xml_source_t* xml_source_from_resname_full(archive_resource_t* ar, const char *path, const char *name, const char *suffix) {
    xml_source_t *result = NULL;

    if (ar != NULL && name != NULL && path != NULL && suffix != NULL) {
        char * searchname = format_string_new("%s%s.%s", path, name, suffix);

        printf("search xsl: %s\n", searchname);

        result = _xml_source_from_res_search(ar, searchname);
        
        free(searchname);
    }

    return result;
}

xml_source_t* xml_source_from_resfile(resource_file_t *resfile) {

    xml_source_t *result = NULL;

    if (resfile) {
        result = xml_source_new(RESOURCE_FILE, resfile);
    }

    return result;

}

void xml_source_free(xml_source_t **source) {

    if( source != NULL && *source != NULL ) {
    
        xml_source_t *_delete_source = *source;
    
        switch(_delete_source->type) {
            case RESOURCE_FILE: resource_file_free((resource_file_t**)&_delete_source->data.resfile);
                                break;
        }
    
        free(_delete_source);
    
        *source = NULL;
    }
}
