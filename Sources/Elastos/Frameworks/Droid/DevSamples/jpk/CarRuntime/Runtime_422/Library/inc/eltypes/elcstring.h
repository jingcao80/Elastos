//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __EZCSTRING_H__
#define __EZCSTRING_H__

#include <elstringapi.h>
#include <assert.h>

#ifdef __cplusplus

_ELASTOS_NAMESPACE_BEGIN

class StringBuf;

/** @addtogroup CARTypesRef
  *   @{
  */
class CString
{
public:
    // e.g., CString as;
    CString() : m_string(NULL) {}

//  This class accords with bitwise copy semantics, and needn't declare copy
//  contructor.
//  //e.g., CString as(str);
//  CString(const CString& str) : m_string(str.m_string) {};

    // e.g., CString as("Hello");
    CString(const char* pch) : m_string(pch) {}

    Int32 GetLength(Int32 maxLen = -1) const { // e.g., as.Length(64);
        return _String_GetLength(m_string, maxLen);
    }

    Int32 Compare(const CString& str, StringCase stringCase = StringCase_Sensitive) const { //e.g., as.Compare(str);
        return _String_Compare(m_string, str.m_string, stringCase);
    }

    Int32 Equals(const CString& str) const {
        return !Compare(str,StringCase_Sensitive);
    }

    Int32 EqualsIgnoreCase(const CString& str) const {
        return !Compare(str,StringCase_Insensitive);
    }

    Boolean IsNull() const {  // e.g., if (str.IsNull()) {...} or Boolean b = str.IsNull();
        return m_string == NULL;
    }

    Boolean IsEmpty() const {
        assert(m_string);
        return m_string[0] == '\0';
    }

    Boolean IsNullOrEmpty() const {
        return (m_string == NULL || m_string[0] == '\0');
    }

    Int32 ToInt32(UInt32 radix = 10) const {
        return _String_ToInt32(m_string, radix);
    }

    Int64 ToInt64(UInt32 radix = 10) const {
        return _String_ToInt64(m_string, radix);
    }

    Boolean ToBoolean() const {
        return _String_ToBoolean(m_string);
    }

    Double ToDouble() const {
        return _String_ToDouble(m_string);
    }

    Int32 GetCharCount() const {
        return _String_GetCharCount(m_string, GetLength());
    }

    Char32 GetChar(Int32 index) const {
        return _String_GetChar(m_string, GetLength(), index);
    }

    Int32 GetCharByOffset(UInt32 offset, UInt32 *nextOffset = NULL) const {
        return _String_GetCharByOffset(m_string, GetLength(), offset, nextOffset);
    }

    //---- Contains ----
    Boolean Contains(const CString&  substr, StringCase stringCase = StringCase_Sensitive) const {
        return _String_Contains(m_string, substr, stringCase);
    }

    Boolean StartWith(const CString&  substr,
            StringCase stringCase = StringCase_Sensitive) const {
        return _String_StartWith(m_string, substr, stringCase);
    }

    Boolean EndWith(const CString& substr,
            StringCase stringCase = StringCase_Sensitive) const {
        return _String_EndWith(m_string, substr, stringCase);
    }

    //---- IndexOf ----
    Int32 IndexOf(Char8 ch, Int32 start = 0,
        StringCase stringCase = StringCase_Sensitive) const {
        if ((start != 0) && (start < 0 || start > GetLength())) return -1;

        return _String_IndexOf_Char8(m_string + start, ch, stringCase);
    }

    Int32 IndexOfAny(const CString& strCharSet, Int32 start = 0,
            StringCase stringCase = StringCase_Sensitive) const {
        if ((start != 0) && (start < 0 || start > GetLength())) return -1;

        return _String_IndexOf_AnyChar8(m_string + start, strCharSet, stringCase);
    }

    Int32 IndexOfChar(Char32 ch, Int32 start = 0, StringCase stringCase  = StringCase_Sensitive) const {
        if ((start != 0) && (start < 0 || start > GetLength())) return -1;

        return _String_IndexOf_Char(m_string + start, ch, stringCase);
    }

    Int32 IndexOfAnyChar(Char32 *strCharSet, Int32 start = 0,
            StringCase stringCase = StringCase_Sensitive) const {
        if ((start != 0) && (start < 0 || start > GetLength())) return -1;

        return _String_IndexOf_AnyChar(m_string + start, strCharSet, stringCase);
    }

    Int32 IndexOf(const CString& str, Int32 start = 0, StringCase stringCase  = StringCase_Sensitive) const {
        if ((start != 0) && (start < 0 || start > GetLength())) return -1;

        return _String_IndexOf_Substring(m_string + start, str, stringCase);
    }

    //---- LastIndexOf ----
    Int32 LastIndexOf(Char8 ch, Int32 start = 0,
            StringCase stringCase = StringCase_Sensitive) const {
        if ((start != 0) && (start < 0 || start > GetLength())) return -1;

        return _String_LastIndexOf_Char8(m_string + start, ch, stringCase);
    }

    Int32 LastIndexOfAny(const CString& strCharSet, Int32 start = 0,
            StringCase stringCase = StringCase_Sensitive) const {
        if ((start != 0) && (start < 0 || start > GetLength())) return -1;

        return _String_LastIndexOf_AnyChar8(m_string + start, strCharSet, stringCase);
    }

    Int32 LastIndexOfChar(Char32 ch, Int32 start = 0,
        StringCase stringCase  = StringCase_Sensitive) const {
        if ((start != 0) && (start < 0 || start > GetLength())) return -1;

        return _String_LastIndexOf_Char(m_string + start, ch, stringCase);
    }

    Int32 LastIndexOfAnyChar(Char32 *strCharSet, Int32 start = 0,
            StringCase stringCase = StringCase_Sensitive) const {
        if ((start != 0) && (start < 0 || start > GetLength())) return -1;

        return _String_LastIndexOf_AnyChar(m_string + start, strCharSet,
            stringCase);
    }

    Int32 LastIndexOf(const CString& str, Int32 start = 0,
        StringCase stringCase  = StringCase_Sensitive) const {
        if ((start != 0) && (start < 0 || start > GetLength())) return -1;

        return _String_LastIndexOf_Substring(m_string + start, str, stringCase);
    }

    //---- Substring ----
    CString Substring(Int32 start, StringBuf& sub) {
        return CString(_String_Substring_Buffer(m_string, start,
            (PCarQuintet)&sub));
    }

    CString Substring(Int32 start) {
        return CString(_String_Substring(m_string, start));
    }

    CString Substring(Int32 start, Int32 len, StringBuf& sub) {
        return CString(_String_Substring_Length_Buffer(m_string, start, len,
            (PCarQuintet)&sub));
    }

    //---- ToLowerCase ----
    CString ToLowerCase(StringBuf& lowser) {
        return CString(_String_ToLowerCase(m_string, (PCarQuintet)&lowser));
    }

    //---- ToUpperCase ----
    CString ToUpperCase(StringBuf& upper) {
        return CString(_String_ToUpperCase(m_string, (PCarQuintet)&upper));
    }

    //---- TrimStart ----
    CString TrimStart(StringBuf& str) const {
        return CString(_String_TrimStart(m_string, (PCarQuintet)&str));
    }

    //---- TrimEnd ----
    CString TrimEnd(StringBuf& str) const {
        return CString(_String_TrimEnd(m_string, (PCarQuintet)&str));
    }

    //---- Trim ----
    CString Trim(StringBuf& str) const {
        return CString(_String_Trim(m_string, (PCarQuintet)&str));
    }

    operator const char*() const
    {  //  for 3rd party API such as foo(char* pch);
        return (char *)m_string;
    }

    void operator=(const CString& str) { // e.g., str1 = str2;
        m_string = str.m_string;
    }

    void operator=(const StringBuf& asb) { // e.g., str = asb;
        m_string = (char *)((PCarQuintet)&asb)->m_pBuf;
    }

    void operator=(const char* pch) {  // e.g., str = "Hello";
        m_string = pch;
    }

    const Char8 operator[](Int32 idx) const {
        assert(idx >= 0);
        return m_string[idx];
    }

    Int32 GetHashCode() {
        return _String_GetHashCode(m_string);
    }

    static CString Duplicate(const CString& strSource) {
        return CString(_String_Duplicate(strSource));
    }

    static void Free(CString& str) {
        _String_Free((char *)str.m_string);
    }

    Char16 *ToUnicode(Char16 *u16str, Int32 count) {
        _StringBuf_ToUnicode(u16str, m_string, count);
        return u16str;
    }

protected:
    void operator==(const char *) {}
    void operator!=(const char *) {}
    void operator!() {}
    void operator*() {}

    void operator+=(const char *) {}
    void operator+(const char *) {}
    void operator+=(const int) {}
    void operator-=(const int) {}
    void operator+(const int) {}
    void operator-(const int) {}

    const char* m_string;
};
/** @} */

_ELASTOS_NAMESPACE_END

#else //!__cplusplus

typedef const char* CString;

#endif //__cplusplus

#endif //__EZCSTRING_H__
