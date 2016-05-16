//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if !defined(__CONVERTSTR_H__)
#define __CONVERTSTR_H__

#define TEXT_BUF_SIZE 260

typedef struct NumberInfo
{
    _ELASTOS PCarQuintet pCq;
    union
    {
        _ELASTOS Int64  n;
        _ELASTOS Double d;
    } value;
    _ELASTOS Int32 fmt;
    _ELASTOS Int32 type;
} NumberInfo, *PNumberInfo;

_ELASTOS Int32 ConvertNumber(
    PNumberInfo pNumberInfo, _ELASTOS Int32 bits = 32);

#endif  //__CONVERTSTR_H__
