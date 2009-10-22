/*
	*
	* Copyright (c) 2009
	* OpenDbg TeaM
	*
*/
#ifndef DEFINES_H_
#define DEFINES_H_

//typedef unsigned 
typedef unsigned __int64	u64;
typedef unsigned long		u32;
typedef unsigned short		u16;
typedef unsigned char		u8;

typedef __int64				s64;
typedef int					s32;
typedef short				s16;
typedef char				s8;
typedef const char			cs8;
typedef const u8			cu8;

#if defined(_WIN64)
typedef             __int64 s3264;
typedef unsigned    __int64 u3264;  
#else
typedef long                s3264;
typedef unsigned long       u3264;
#endif

#define p8(x)  ((u8*)(x))
#define pcu8(x) ((cu8*)(x))
#define p16(x) ((u16*)(x))
#define p32(x) ((u32*)(x))
#define p64(x) ((u64*)(x))
#define p3264(x) ((u3264*)(x))
#define pv(x)  ((void*)(x))
#define ppv(x) ((void**)(x))

#endif
