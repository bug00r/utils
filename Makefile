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

_SRC_FILES+=string_utils file_path_utils number_utils byte_utils

LIBNAME:=utils
LIBEXT:=a
LIB:=lib$(LIBNAME).$(LIBEXT)
LIB_TARGET:=$(BUILDPATH)$(LIB)

OBJS+=$(patsubst %,$(BUILDPATH)%,$(patsubst %,%.o,$(_SRC_FILES)))

CFLAGS+=-I/c/dev/include -I./src
LDFLAGS+=-L/c/dev/lib$(BIT_SUFFIX) -L./$(BUILDPATH)

LDFLAGS+=-static -ldl_list

all: mkbuilddir $(LIB_TARGET)

$(LIB_TARGET): $(_SRC_FILES)
	$(AR) $(ARFLAGS) $(LIB_TARGET) $(OBJS)

$(_SRC_FILES):
	$(CC) $(CFLAGS) -c src/$@.c -o $(BUILDPATH)$@.o 

test_byte_utils: mkbuilddir $(LIB_TARGET)
	$(CC) $(CFLAGS) ./test/$@.c ./src/byte_utils.c $(RES_O_PATH) -o $(BUILDPATH)$@.exe $(LDFLAGS)
	$(BUILDPATH)$@.exe

.PHONY: clean mkbuilddir mkzip addzip test 

test: test_byte_utils

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
	cp ./src/string_utils.h $(INSTALL_ROOT)include/string_utils.h
	cp ./src/byte_utils.h $(INSTALL_ROOT)include/byte_utils.h
	cp $(BUILDPATH)$(LIB) $(INSTALL_ROOT)lib$(BIT_SUFFIX)/$(LIB)