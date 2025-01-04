#ifndef UTILS_DEFS_H
#define UTILS_DEFS_H

#ifndef EXTERN_BLOB
	#define EXTERN_BLOB(name,suffix) \
		extern unsigned char _binary_ ## name ## _ ## suffix ## _start; \
        extern unsigned char _binary_ ## name ## _ ## suffix ## _end; \
		extern unsigned char _binary_ ## name ## _ ## suffix ## _size
#endif

#ifndef UNUSED
	#define UNUSED(x) (void)(x)
#endif

#ifndef DEBUG_LOG_ARGS
	#if defined(debug) && debug != 0
		#define DEBUG_LOG_ARGS(fmt, ...) printf(" %s:%s:%i => ", __FILE__, __func__, __LINE__);printf((fmt), __VA_ARGS__)
	#else
		#define DEBUG_LOG_ARGS(fmt, ...)
	#endif
#endif

#ifndef DEBUG_LOG
	#if defined(debug) && debug != 0
		#define DEBUG_LOG(msg) printf(" %s:%s:%i => ", __FILE__, __func__, __LINE__);printf((msg))
	#else
		#define DEBUG_LOG(msg)
	#endif
#endif

/* --- PRINTF_BYTE_TO_BINARY macro's --- */
//src: https://stackoverflow.com/questions/111928/is-there-a-printf-converter-to-print-in-binary-format/25108449#25108449
#define PRINTF_BINARY_PATTERN_INT8 "%c%c%c%c%c%c%c%c"
#define PRINTF_BYTE_TO_BINARY_INT8(i)    \
    (((i) & 0x80ll) ? '1' : '0'), \
    (((i) & 0x40ll) ? '1' : '0'), \
    (((i) & 0x20ll) ? '1' : '0'), \
    (((i) & 0x10ll) ? '1' : '0'), \
    (((i) & 0x08ll) ? '1' : '0'), \
    (((i) & 0x04ll) ? '1' : '0'), \
    (((i) & 0x02ll) ? '1' : '0'), \
    (((i) & 0x01ll) ? '1' : '0')

#define PRINTF_BINARY_PATTERN_INT16 \
    PRINTF_BINARY_PATTERN_INT8              PRINTF_BINARY_PATTERN_INT8
#define PRINTF_BYTE_TO_BINARY_INT16(i) \
    PRINTF_BYTE_TO_BINARY_INT8((i) >> 8),   PRINTF_BYTE_TO_BINARY_INT8(i)
#define PRINTF_BINARY_PATTERN_INT32 \
    PRINTF_BINARY_PATTERN_INT16             PRINTF_BINARY_PATTERN_INT16
#define PRINTF_BYTE_TO_BINARY_INT32(i) \
    PRINTF_BYTE_TO_BINARY_INT16((i) >> 16), PRINTF_BYTE_TO_BINARY_INT16(i)
#define PRINTF_BINARY_PATTERN_INT64    \
    PRINTF_BINARY_PATTERN_INT32             PRINTF_BINARY_PATTERN_INT32
#define PRINTF_BYTE_TO_BINARY_INT64(i) \
    PRINTF_BYTE_TO_BINARY_INT32((i) >> 32), PRINTF_BYTE_TO_BINARY_INT32(i)
/* --- end macros --- */

#endif