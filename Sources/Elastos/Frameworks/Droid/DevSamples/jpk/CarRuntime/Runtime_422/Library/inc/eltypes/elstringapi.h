
//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __ELSTRINGAPI_H__
#define __ELSTRINGAPI_H__

#include <elquintettype.h>

_ELASTOS_NAMESPACE_BEGIN

/** @addtogroup BaseTypesRef
  *   @{
  */
typedef enum _StringCase
{
    StringCase_Sensitive         = 0x0000,
    StringCase_Insensitive       = 0x0001,
} StringCase;

typedef enum _Encoding
{
    Encoding_Default             = 0x0000,
    Encoding_ASCII               = 0x0001,
    Encoding_UTF7                = 0x0002,
    Encoding_UTF8                = 0x0003,
    Encoding_GB18030             = 0x0004,
    Encoding_LATIN_1             = 0x0005,
    Encoding_BIG5                = 0x0006,
    Encoding_EUC_JP              = 0x0007,
} Encoding;
/** @} */

_ELASTOS_NAMESPACE_END

#ifdef __cplusplus
extern "C" {
#endif
    //needed by String and CString
    _ELASTOS Int32 __cdecl _String_Compare(const char *string1,
        const char *string2, _ELASTOS StringCase stringCase);

#ifdef __cplusplus
    _ELASTOS Int32 __cdecl _String_ToInt32(const char *string, _ELASTOS UInt32 radix = 10);
    _ELASTOS Int64 __cdecl _String_ToInt64(const char *string, _ELASTOS UInt32 radix = 10);
#else
    _ELASTOS Int32 __cdecl _String_ToInt32(const char *string, _ELASTOS UInt32 radix);
    _ELASTOS Int64 __cdecl _String_ToInt64(const char *string, _ELASTOS UInt32 radix);
#endif
    _ELASTOS Boolean __cdecl _String_ToBoolean(const char *string);
    _ELASTOS Double __cdecl _String_ToDouble(const char *string);

    _ELASTOS UInt32 __cdecl _String_GetCharCount(const char *string,
        _ELASTOS UInt32 src_len);
    _ELASTOS Int32 __cdecl _String_GetChar(const char *string,
        _ELASTOS UInt32 src_len, _ELASTOS UInt32 Index);
    _ELASTOS Int32 __cdecl _String_GetCharByOffset(const char *string,
        _ELASTOS UInt32 src_len, _ELASTOS UInt32 offset, _ELASTOS UInt32 *nextOffset);

    _ELASTOS Boolean __cdecl _String_Contains(const char *string,
        const char *substr, _ELASTOS StringCase stringCase);
    _ELASTOS Boolean __cdecl _String_StartWith(const char *string,
        const char *substr, _ELASTOS StringCase stringCase);
    _ELASTOS Boolean __cdecl _String_EndWith(const char *string,
        const char *substr, _ELASTOS StringCase stringCase);

    _ELASTOS Boolean __cdecl _String_RegionMatches(const char *string,
            _ELASTOS Int32 thisStart, const char * otherStr,
            _ELASTOS Int32 start, _ELASTOS Int32 length,
            _ELASTOS StringCase stringCase);

    _ELASTOS Int32 __cdecl _String_IndexOf_Char(const char *string,
        _ELASTOS Char32 ch, _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _String_IndexOf_Substring(const char *string,
        const char *substr, _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _String_LastIndexOf_Char(const char *string,
        _ELASTOS Char32 ch, _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _String_LastIndexOf_Substring(const char *string,
        const char *substr, _ELASTOS StringCase stringCase);

    _ELASTOS Int32 __cdecl _String_GetHashCode(const char *string);

    //needed by CString
    _ELASTOS Int32 __cdecl _String_GetLength(const char *string,
        _ELASTOS Int32 nMaxLen);
    char* __cdecl _String_Duplicate(const char *strSource);
    void __cdecl _String_Free(char * str);


    _ELASTOS Int32 __cdecl _String_IndexOf_Char8(const char *string, char ch,
        _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _String_IndexOf_AnyChar8(const char *string,
        const char* strCharSet, _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _String_IndexOf_AnyChar(const char *string,
        _ELASTOS Char32 *strCharSet, _ELASTOS StringCase stringCase);


    _ELASTOS Int32 __cdecl _String_LastIndexOf_Char8(const char *string, char ch,
        _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _String_LastIndexOf_AnyChar8(const char *string,
        const char* strCharSet, _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _String_LastIndexOf_AnyChar(const char *string,
        _ELASTOS Char32 *strCharSet, _ELASTOS StringCase stringCase);


    char* __cdecl _String_Substring_Buffer(const char *string, _ELASTOS Int32 start,
        _ELASTOS PCarQuintet pCq);
    char* __cdecl _String_Substring(const char *string, _ELASTOS Int32 start);
    char* __cdecl _String_Substring_Length_Buffer(const char *string,
        _ELASTOS Int32 start, _ELASTOS Int32 len, _ELASTOS PCarQuintet pCq);

    char* __cdecl _String_ToLowerCase(const char *string, _ELASTOS PCarQuintet pCq);
    _ELASTOS Char32 __cdecl _String_Char32_ToLowerCase(_ELASTOS Char32 ch);
    char* __cdecl _String_ToUpperCase(const char *string, _ELASTOS PCarQuintet pCq);
    _ELASTOS Char32 __cdecl _String_Char32_ToUpperCase(_ELASTOS Char32 ch);

    char* __cdecl _String_TrimStart(const char *string, _ELASTOS PCarQuintet pCq);
    char* __cdecl _String_TrimEnd(const char *string, _ELASTOS PCarQuintet pCq);
    char* __cdecl _String_Trim(const char *string, _ELASTOS PCarQuintet pCq);

    _ELASTOS Int32 __cdecl _String_ConvertString(const char *string, _ELASTOS PCarQuintet pCq);

    _ELASTOS PCarQuintet  __cdecl _StringBuf_Box_Init(_ELASTOS PCarQuintet pCq,
        char *str, _ELASTOS Int32 size, _ELASTOS Boolean bHeapAlloced);

    _ELASTOS Int32 _StringBuf_ToUnicode(_ELASTOS PChar16 u16str, const char *string,
        _ELASTOS Int32 count);

#ifdef __cplusplus
}
#endif

#endif //__ELSTRINGAPI_H__
