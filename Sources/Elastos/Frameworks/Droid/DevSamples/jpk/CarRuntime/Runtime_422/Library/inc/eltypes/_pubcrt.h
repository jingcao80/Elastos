//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __PUBCRT_H__
#define __PUBCRT_H__

#include <eltypes.h>

#if defined(_x86) || defined(_wince) || (defined(__GNUC__) && (__GNUC__ >= 4))
#define _LITTLE_ENDIAN_DEFINED
#endif

typedef union
{
#ifdef _LITTLE_ENDIAN_DEFINED
    struct
    {
        int lo, hi;
    } ints;
#else
    struct
    {
        int hi, lo;
    } ints;
#endif
    double d;
} fd_twoints;

#define __HI(x) x.ints.hi
#define __LO(x) x.ints.lo

#define EXP_MASK  0x7FF00000
#define DOUBLE_HI_MASK  ULLONG(0xffffffff00000000)
#define DOUBLE_LO_MASK  ULLONG(0x00000000ffffffff)
#define DOUBLE_FRACTION_MASK  ULLONG(0xfffffffffffff)
#define DOUBLE_EXP_MASK  ULLONG(0x7ff0000000000000)

#ifdef __cplusplus
extern "C" {
#endif

_ELASTOS Int32 _getNextCharLength(const char *string);

_ELASTOS ECode __stdcall CrtGetBootModule(const char *szName,
                        _ELASTOS UInt32 *puAddr, _ELASTOS UInt32 *puSize);

_ELASTOS ECode __stdcall CrtFindService(
                 _ELASTOS String szName, IInterface **ppObj);


_ELASTOS ECode CrtInitializeLock(_ELASTOS PVoid*);
_ELASTOS ECode CrtEnterLock(_ELASTOS PVoid);
_ELASTOS ECode CrtLeaveLock(_ELASTOS PVoid);
_ELASTOS ECode CrtDeleteLock(_ELASTOS PVoid*);

#ifdef __cplusplus
}
#endif

#define _MAX_PATH     260     /* max length of full pathname */
#define _MAX_PATH_MB  520     //max length of full pathname for hanzi
#define _MAX_FNAME  256      /* max length of file name compont */

#define _MAX_LINE   512

#endif //__PUBCRT_H__
