_CFLAGS:=$(CFLAGS)
CFLAGS:=$(_CFLAGS)
_LDFLAGS:=$(LDFLAGS)
LDFLAGS:=$(_LDFLAGS)

ARFLAGS?=rcs
PATHSEP?=/
BUILDROOT?=build

BUILDDIR?=$(BUILDROOT)$(PATHSEP)$(CC)
BUILDPATH?=$(BUILDDIR)$(PATHSEP)

ifndef PREFIX
	INSTALL_ROOT=$(BUILDPATH)
else
	INSTALL_ROOT=$(PREFIX)$(PATHSEP)
	ifeq ($(INSTALL_ROOT),/)
	INSTALL_ROOT=$(BUILDPATH)
	endif
endif

ifdef DEBUG
	CFLAGS+=-ggdb
	ifeq ($(DEBUG),)
	CFLAGS+=-Ddebug=1
	else 
	CFLAGS+=-Ddebug=$(DEBUG)
	endif
endif

ifeq ($(M32),1)
	CFLAGS+=-m32
	BIT_SUFFIX+=32
endif

#CFLAGS+=-std=c11 -Wpedantic -pedantic-errors -Wall -Wextra
CFLAGS+=-std=c11 -Wall -Wextra

BIT_SUFFIX=

ifeq ($(M32),1)
	CFLAGS+=-m32
	BIT_SUFFIX+=32
endif

CFLAGS+=-std=c11 -DIN_LIBXML -DLIBXML_STATIC -Wpedantic -Wall -Wextra

_SRC_FILES+=string_utils regex_utils resource xpath_utils file_path_utils xml_source xml_utils number_utils xslt_utils byte_utils

LIBNAME:=utils
LIBEXT:=a
LIB:=lib$(LIBNAME).$(LIBEXT)
LIB_TARGET:=$(BUILDPATH)$(LIB)

OBJS+=$(patsubst %,$(BUILDPATH)%,$(patsubst %,%.o,$(_SRC_FILES)))

CFLAGS+=-I/c/dev/include -I./src
LDFLAGS+=-L/c/dev/lib$(BIT_SUFFIX) -L./$(BUILDPATH)


THIRD_PARTY_LIBS=exslt xslt xml2 archive crypto nettle regex lzma z lz4 bz2 bcrypt zstd iconv
REGEX_LIBS=pcre2-8
#this c flags is used by regex lib
CFLAGS+=-DPCRE2_STATIC

OS_LIBS=kernel32 user32 gdi32 winspool comdlg32 advapi32 shell32 uuid ole32 oleaut32 comctl32 ws2_32

USED_LIBS=$(patsubst %,-l%, utils $(REGEX_LIBS) $(THIRD_PARTY_LIBS) $(OS_LIBS) dl_list )

LDFLAGS+=-static $(USED_LIBS)

#wc -c < filename => if needed for after compression size of bytes
RES=zip_resource
RES_O=$(RES).o
RES_O_PATH=$(BUILDPATH)$(RES_O)
RES_7Z=$(RES).7z
RES_FILES_PATTERN=./data/*
ZIP=7z
ZIP_ARGS=a -t7z
ZIP_CMD=$(ZIP) $(ZIP_ARGS)

all: mkbuilddir mkzip addzip $(LIB_TARGET)

$(LIB_TARGET): $(_SRC_FILES)
	$(AR) $(ARFLAGS) $(LIB_TARGET) $(OBJS)

$(_SRC_FILES):
	$(CC) $(CFLAGS) -c src/$@.c -o $(BUILDPATH)$@.o 

test_regex_utils: mkbuilddir mkzip addzip $(LIB_TARGET)
	$(CC) $(CFLAGS) ./test/$@.c ./src/regex_utils.c $(RES_O_PATH) -o $(BUILDPATH)$@.exe $(LDFLAGS)
	$(BUILDPATH)$@.exe

test_xslt_utils: mkbuilddir mkzip addzip $(LIB_TARGET)
	$(CC) $(CFLAGS) ./test/$@.c ./src/xslt_utils.c $(RES_O_PATH) -o $(BUILDPATH)$@.exe $(LDFLAGS)
	$(BUILDPATH)$@.exe

test_xml_utils: mkbuilddir mkzip addzip $(LIB_TARGET)
	$(CC) $(CFLAGS) ./test/$@.c ./src/xml_utils.c $(RES_O_PATH) -o $(BUILDPATH)$@.exe $(LDFLAGS)
	$(BUILDPATH)$@.exe

test_xml_source: mkbuilddir mkzip addzip $(LIB_TARGET)
	$(CC) $(CFLAGS) ./test/$@.c ./src/xml_source.c $(RES_O_PATH) -o $(BUILDPATH)$@.exe $(LDFLAGS)
	$(BUILDPATH)$@.exe

test_resource: mkbuilddir mkzip addzip $(LIB_TARGET)
	$(CC) $(CFLAGS) ./test/$@.c ./src/resource.c $(RES_O_PATH) -o $(BUILDPATH)$@.exe $(LDFLAGS)
	$(BUILDPATH)$@.exe

test_byte_utils: mkbuilddir mkzip addzip $(LIB_TARGET)
	$(CC) $(CFLAGS) ./test/$@.c ./src/byte_utils.c $(RES_O_PATH) -o $(BUILDPATH)$@.exe $(LDFLAGS)
	$(BUILDPATH)$@.exe

.PHONY: clean mkbuilddir mkzip addzip test 

test: test_regex_utils test_xslt_utils test_xml_utils test_xml_source test_resource test_byte_utils

addzip:
	cd $(BUILDPATH); \
	ld -r -b binary $(RES_7Z) -o $(RES_O)

mkzip:
	-$(ZIP_CMD) $(BUILDPATH)$(RES_7Z) $(RES_FILES_PATTERN)

mkbuilddir:
	mkdir -p $(BUILDDIR)
	
clean:
	-rm -dr $(BUILDROOT)

install:
	mkdir -p $(INSTALL_ROOT)include
	mkdir -p $(INSTALL_ROOT)lib$(BIT_SUFFIX)
	cp ./src/defs.h $(INSTALL_ROOT)include/defs.h
	cp ./src/file_path_utils.h $(INSTALL_ROOT)include/file_path_utils.h
	cp ./src/number_utils.h $(INSTALL_ROOT)include/number_utils.h
	cp ./src/regex_utils.h $(INSTALL_ROOT)include/regex_utils.h
	cp ./src/resource.h $(INSTALL_ROOT)include/resource.h
	cp ./src/string_utils.h $(INSTALL_ROOT)include/string_utils.h
	cp ./src/xml_source.h $(INSTALL_ROOT)include/xml_source.h
	cp ./src/xml_utils.h $(INSTALL_ROOT)include/xml_utils.h
	cp ./src/xpath_utils.h $(INSTALL_ROOT)include/xpath_utils.h
	cp ./src/xslt_utils.h $(INSTALL_ROOT)include/xslt_utils.h
	cp ./src/byte_utils.h $(INSTALL_ROOT)include/byte_utils.h
	cp $(BUILDPATH)$(LIB) $(INSTALL_ROOT)lib$(BIT_SUFFIX)/$(LIB)