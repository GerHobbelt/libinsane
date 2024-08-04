#ifndef __LIBINSANE_ENDIANESS_H
#define __LIBINSANE_ENDIANESS_H

#if defined(OS_LINUX)

#include <endian.h>

#elif defined(OS_MACOS)

#include <libkern/OSByteOrder.h>

#define htobe16(x) OSSwapHostToBigInt16(x)
#define htole16(x) OSSwapHostToLittleInt16(x)
#define be16toh(x) OSSwapBigToHostInt16(x)
#define le16toh(x) OSSwapLittleToHostInt16(x)

#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)

#define htobe64(x) OSSwapHostToBigInt64(x)
#define htole64(x) OSSwapHostToLittleInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define le64toh(x) OSSwapLittleToHostInt64(x)

#elif defined(OS_WINDOWS)

// XXX(Jflesch): assuming Windows x86 --> little endian

#define le16toh(v) (v)
#define le32toh(v) (v)
#define htole32(v) (v)
#define htole16(v) (v)

static inline uint16_t be16toh(uint16_t v)
{
	return ((v << 8) | (v >> 8));
}

static inline uint16_t htobe16(uint16_t v)
{
	return ((v << 8) | (v >> 8));
}

#else

#error "Unsupported platform!"

#endif

#endif
