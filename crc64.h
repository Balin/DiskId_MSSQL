/*
 * crc64.h
 */

#ifndef __CRC64P_H_INCLUDED
#define __CRC64P_H_INCLUDED

__int64  crc64( const void *data, const size_t len  );

static const __int64 s_i64POLYNOM = 0xC96C5795D7870F42;

#endif // __CRC64P_H_INCLUDED
