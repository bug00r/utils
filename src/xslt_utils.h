#ifndef XSLT_UTILS_H
#define XSLT_UTILS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <libxml/hash.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/extensions.h>
#include <libxslt/variables.h>
#include <libxslt/documents.h>
#include <libxslt/xsltutils.h>

#include "string_utils.h"
#include "xml_utils.h"
#include "dl_list.h"

typedef struct {
    xml_ctx_t           *xml;           //required
    xsltStylesheetPtr   stylesheet;     //required (automatic cleaned if exist)
    const char          **text_params;  //optional (NULL)
    const char          **xpath_params; //optional (NULL)
    const char          * output;       //optional (NULL)
    FILE                *profile;       //optional (NULL)
    dl_list_t           *errors;        //automatic usage
} xslt_ctx_t;


void xslt_ctx_init(xslt_ctx_t *ctx);
void xslt_ctx_cleanup(xslt_ctx_t *ctx);
xmlDocPtr do_xslt(xslt_ctx_t * ctx);
void xslt_print_err(xslt_ctx_t * ctx);

#endif