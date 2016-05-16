//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CLSBASE_H__
#define __CLSBASE_H__

#include <stdint.h>
//#include <stdlib.h>
#include <elatypes.h>
#include <string.h>
#include <clsdef.h>

#if defined(_arm) && defined(__GNUC__) && (__GNUC__ >= 4) || defined(_MSVC)
#define ALIGN_BOUND 8
#else
#define ALIGN_BOUND 4
#endif

inline int RoundUp4(int n)
{
    return ((n) + 4 - 1) & ~(4 - 1);
}

namespace ErrRec {
    extern int nNumber;
    extern char szName[];
    extern char szMessage[];
}

#define _Return(ret) return ret

#define _ReturnOK(ret)                  \
{                                       \
    ErrRec::nNumber = CLS_NoError;      \
    return ret;                         \
}

#define _ReturnError(err)                   \
{                                           \
    ErrRec::nNumber = err;                  \
    strcpy(ErrRec::szName, #err);           \
    return err;                             \
}

extern void ExtraMessage(const char *, const char *s2 = NULL,
                const char *s3 = NULL, const char *s4 = NULL);

#endif // __CLSBASE_H__
