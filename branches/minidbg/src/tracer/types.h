#pragma once
typedef unsigned __int64 u64;
typedef unsigned long    u32;
typedef unsigned short   u16;
typedef unsigned char    u8;

typedef __int64     s64;
typedef int         s32;
typedef short       s16;
typedef char        s8;
typedef const char  cs8;
typedef const u8    cu8;
	
#if defined(_WIN64)
    typedef				__int64 s3264;
    typedef unsigned	__int64 u3264;	
#else
    typedef long			s3264;
    typedef unsigned long	u3264;
#endif