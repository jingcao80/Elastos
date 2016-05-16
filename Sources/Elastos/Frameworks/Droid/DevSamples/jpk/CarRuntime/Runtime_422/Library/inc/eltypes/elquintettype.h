//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __ELQUINTETTYPE_H__
#define __ELQUINTETTYPE_H__

_ELASTOS_NAMESPACE_BEGIN

/** @addtogroup CARTypesRef
  *   @{
  */
typedef enum _CarQuintetFlag
{
    CarQuintetFlag_HeapAlloced      = 0x00010000,
    CarQuintetFlag_AutoRefCounted   = 0x00100000,

    CarQuintetFlag_Type_Unknown     = 0,

    CarQuintetFlag_Type_Int8        = 1,
    CarQuintetFlag_Type_Int16       = 2,
    CarQuintetFlag_Type_Int32       = 3,
    CarQuintetFlag_Type_Int64       = 4,

    CarQuintetFlag_Type_Byte        = 5,
    CarQuintetFlag_Type_UInt8       = CarQuintetFlag_Type_Byte,
    CarQuintetFlag_Type_UInt16      = 6,
    CarQuintetFlag_Type_UInt32      = 7,
    CarQuintetFlag_Type_UInt64      = 8,

    CarQuintetFlag_Type_Boolean     = CarQuintetFlag_Type_Byte,
    CarQuintetFlag_Type_Float       = 9,
    CarQuintetFlag_Type_Double      = 10,

    CarQuintetFlag_Type_Char8       = 11,
    CarQuintetFlag_Type_Char16      = CarQuintetFlag_Type_UInt16,
    CarQuintetFlag_Type_Char32      = CarQuintetFlag_Type_UInt32,
    CarQuintetFlag_Type_CString     = 12,
    CarQuintetFlag_Type_String      = 13,

    CarQuintetFlag_Type_EMuid       = 14,
    CarQuintetFlag_Type_EGuid       = 15,
    CarQuintetFlag_Type_ECode       = CarQuintetFlag_Type_Int32,
    CarQuintetFlag_Type_Enum        = CarQuintetFlag_Type_Int32,
    CarQuintetFlag_Type_Struct      = 16,
    CarQuintetFlag_Type_IObject     = 17,
    CarQuintetFlag_Type_RefObject   = 18,
    CarQuintetFlag_Type_LightRefObject   = 19,

    CarQuintetFlag_TypeMask         = 0x0000ffff
} CarQuintetFlag;

typedef Int32 CarQuintetFlags;
typedef Int32 CarQuintetLocks;

typedef struct CarQuintet
{
    CarQuintetFlags m_flags;
    CarQuintetLocks m_reserve;
    MemorySize      m_used;
    MemorySize      m_size;
    PVoid           m_pBuf;
} CarQuintet, *PCarQuintet, *PCARQUINTET;

/** @} */

_ELASTOS_NAMESPACE_END

#endif // __ELQUINTETTYPE_H__

