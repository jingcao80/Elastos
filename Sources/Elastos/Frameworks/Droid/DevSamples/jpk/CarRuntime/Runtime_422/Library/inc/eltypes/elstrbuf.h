//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __EZSTRBUF_H__
#define __EZSTRBUF_H__

#include <elcstring.h>

/** @addtogroup BaseTypesRef
  *   @{
  */
typedef enum
{
    NumberFormat_RightAlign              = 0x00000000,
    NumberFormat_LeftAlign               = 0x00010000,
    NumberFormat_Signed                  = 0x00020000,
    NumberFormat_ZeroPrefixed            = 0x00040000,
    NumberFormat_BlankPrefixed           = 0x00080000,
    NumberFormat_PoundSign               = 0x00100000,
    NumberFormat_PrefixMask              = 0x00FF0000,

    NumberFormat_Decimal                 = 0x00000000,
    NumberFormat_UsignedDecimal          = 0x01000000,
    NumberFormat_Octal                   = 0x02000000,
    NumberFormat_Hex                     = 0x04000000,
    NumberFormat_BigHex                  = 0x08000000,
    NumberFormat_IntegerMask             = 0x0F000000,

    NumberFormat_Double                  = 0x00000000,
    NumberFormat_ScientificDouble        = 0x10000000,
    NumberFormat_BigScientificDouble     = 0x20000000,
    NumberFormat_FlexScientificDouble    = 0x40000000,
    NumberFormat_BigFlexScientificDouble = 0x80000000,
    NumberFormat_DoubleMask              = 0xF0000000,

    NumberFormat_TypeMask                = 0xFF000000,

    NumberFormat_Mask                    = 0xFFFF0000,
} _NumberFormat;
/** @} */

//
// Memory structure of StringBuf:
//          __________
//         |  pBuf    |
//         |          v
//  +------------+    +-----------------------+
//  | CarQuintet | ---| char's Buffer         |
//  +------------+    +-----------------------+
//  ^
//  |____ StringBuf (m_pbuf)
//

#ifdef __cplusplus
extern "C" {
#endif
    _ELASTOS PCarQuintet __cdecl _StringBuf_Init(_ELASTOS PVoid pv,
        _ELASTOS Int32 length, _ELASTOS Boolean bHeapAlloced);
    _ELASTOS PCarQuintet __cdecl _StringBuf_Alloc(_ELASTOS Int32 length);
    _ELASTOS PCarQuintet __cdecl _StringBuf_Alloc_Box(char *pstr,
        _ELASTOS Int32 size);

    _ELASTOS Int32 __cdecl _StringBuf_EstimateWStringLength(
        const _ELASTOS PCarQuintet pCq);

    _ELASTOS Int32 __cdecl _StringBuf_ToInt32(const _ELASTOS PCarQuintet pCq,
        _ELASTOS UInt32 radix);
    _ELASTOS Int64 __cdecl _StringBuf_ToInt64(const _ELASTOS PCarQuintet pCq,
        _ELASTOS UInt32 radix);
    _ELASTOS Boolean __cdecl _StringBuf_ToBoolean(const _ELASTOS PCarQuintet pCq);
    _ELASTOS Double __cdecl _StringBuf_ToDouble(const _ELASTOS PCarQuintet pCq);

    _ELASTOS Int32 __cdecl _StringBuf_GetCharCount(_ELASTOS PCarQuintet pCq);
    _ELASTOS Int32 __cdecl _StringBuf_SetLength(_ELASTOS PCarQuintet pCq,
        _ELASTOS Int32 len);
    _ELASTOS Char32 __cdecl _StringBuf_GetChar(_ELASTOS PCarQuintet pCq,
        _ELASTOS Int32 index);

    _ELASTOS Int32 __cdecl _StringBuf_Insert(_ELASTOS PCarQuintet pCq,
        _ELASTOS Int32 offset, const char* s);
    _ELASTOS Int32 __cdecl _StringBuf_Copy(_ELASTOS PCarQuintet pCq,
        const char *s);
    _ELASTOS Int32 __cdecl _StringBuf_Replace(_ELASTOS PCarQuintet pCq,
        _ELASTOS Int32 index, _ELASTOS Int32 count, const char* s);

    _ELASTOS Int32 __cdecl _StringBuf_Append_String(_ELASTOS PCarQuintet pCq,
        const char *s, _ELASTOS Int32 offset, _ELASTOS Int32 count);
    _ELASTOS Int32 __cdecl _StringBuf_Append_Char16(_ELASTOS PCarQuintet pCq,
        _ELASTOS Char16 wc);
    _ELASTOS Int32 __cdecl _StringBuf_Append_Char8(_ELASTOS PCarQuintet pCq,
        _ELASTOS Char8 ch);

    _ELASTOS Int32 __cdecl _StringBuf_Append_Int8(_ELASTOS PCarQuintet pCq,
        _ELASTOS Int8 value, _ELASTOS Flags32 fmt);
    _ELASTOS Int32 __cdecl _StringBuf_Append_Int16(_ELASTOS PCarQuintet pCq,
        _ELASTOS Int16 value, _ELASTOS Flags32 fmt);
    _ELASTOS Int32 __cdecl _StringBuf_Append_Int32(_ELASTOS PCarQuintet pCq,
        _ELASTOS Int32 value, _ELASTOS Flags32 fmt);
    _ELASTOS Int32 __cdecl _StringBuf_Append_Int64(_ELASTOS PCarQuintet pCq,
        _ELASTOS Int64 value, _ELASTOS Flags32 fmt);
    _ELASTOS Int32 __cdecl _StringBuf_Append_Double(_ELASTOS PCarQuintet pCq,
        _ELASTOS Double value, _ELASTOS Flags32 fmt);

    _ELASTOS PCarQuintet __cdecl _StringBuf_Append_DateTime(_ELASTOS PCarQuintet pCq,
        const char *fmt, const _ELASTOS PDateTime pDateTime);

    _ELASTOS PCarQuintet  __cdecl _StringBuf_Box_Init(_ELASTOS PCarQuintet pCq,
        char *str, _ELASTOS Int32 size, _ELASTOS Boolean bHeapAlloced);

    //--------
    _ELASTOS Boolean __cdecl _StringBuf_Contains(const _ELASTOS PCarQuintet pCq,
        const char *substr, _ELASTOS StringCase stringCase);

    _ELASTOS Boolean __cdecl _StringBuf_StartWith(const _ELASTOS PCarQuintet pCq,
        const char *substr, _ELASTOS StringCase stringCase);
    _ELASTOS Boolean __cdecl _StringBuf_EndWith(const _ELASTOS PCarQuintet pCq,
        const char *substr, _ELASTOS StringCase stringCase);

    _ELASTOS Int32 __cdecl _StringBuf_IndexOf_Char8(const _ELASTOS PCarQuintet pCq, char ch,
        _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _StringBuf_IndexOf_AnyChar8(const _ELASTOS PCarQuintet pCq,
        const char* strCharSet, _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _StringBuf_IndexOf_Char(const _ELASTOS PCarQuintet pCq,
        _ELASTOS Char32 ch, _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _StringBuf_IndexOf_AnyChar(const _ELASTOS PCarQuintet pCq,
        _ELASTOS Char32 *strCharSet, _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _StringBuf_IndexOf_Substring(const _ELASTOS PCarQuintet pCq,
        const char *substr, _ELASTOS StringCase stringCase);

    _ELASTOS Int32 __cdecl _StringBuf_LastIndexOf_Char8(const _ELASTOS PCarQuintet pCq, char ch,
        _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _StringBuf_LastIndexOf_AnyChar8(const _ELASTOS PCarQuintet pCq,
        const char* strCharSet, _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _StringBuf_LastIndexOf_Char(const _ELASTOS PCarQuintet pCq,
        _ELASTOS Char32 ch, _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _StringBuf_LastIndexOf_AnyChar(const _ELASTOS PCarQuintet pCq,
        _ELASTOS Char32 *strCharSet, _ELASTOS StringCase stringCase);
    _ELASTOS Int32 __cdecl _StringBuf_LastIndexOf_Substring(const _ELASTOS PCarQuintet pCq,
        const char *substr, _ELASTOS StringCase stringCase);

    char* __cdecl _StringBuf_Substring(const _ELASTOS PCarQuintet pCq, _ELASTOS Int32 start,
        _ELASTOS PCarQuintet sub);
    char* __cdecl _StringBuf_Substring_Length(const _ELASTOS PCarQuintet pCq, _ELASTOS Int32 start,
    	_ELASTOS Int32 len, _ELASTOS PCarQuintet sub);

    _ELASTOS PCarQuintet __cdecl _StringBuf_ToLowerCase(_ELASTOS PCarQuintet pCq);
    _ELASTOS PCarQuintet __cdecl _StringBuf_ToUpperCase(_ELASTOS PCarQuintet pCq);

    _ELASTOS PCarQuintet __cdecl _StringBuf_TrimStart(_ELASTOS PCarQuintet pCq);
    _ELASTOS PCarQuintet __cdecl _StringBuf_TrimEnd(_ELASTOS PCarQuintet pCq);
    _ELASTOS PCarQuintet __cdecl _StringBuf_Trim(_ELASTOS PCarQuintet pCq);

    _ELASTOS PCarQuintet __cdecl _StringBuf_PadLeft(_ELASTOS PCarQuintet pCq,
        _ELASTOS Int32 width);
    _ELASTOS PCarQuintet __cdecl _StringBuf_PadLeft_Char8(
        _ELASTOS PCarQuintet pCq, _ELASTOS Int32 width, _ELASTOS Char8 ch);

    _ELASTOS PCarQuintet __cdecl _StringBuf_PadRight(_ELASTOS PCarQuintet pCq,
        _ELASTOS Int32 width);
    _ELASTOS PCarQuintet __cdecl _StringBuf_PadRight_Char8(
        _ELASTOS PCarQuintet pCq, _ELASTOS Int32 width, _ELASTOS Char8 ch);

    _ELASTOS PCarQuintet __cdecl _StringBuf_EncodeURL(_ELASTOS PCarQuintet pCq,
        const char* s, const char* Reserved);
    _ELASTOS PCarQuintet __cdecl _StringBuf_DecodeURL(_ELASTOS PCarQuintet pCq,
        const char* s);

    _ELASTOS Int32 _StringBuf_ToUnicode(_ELASTOS PChar16 u16str, const char *string,
        _ELASTOS Int32 count);
    _ELASTOS Int32 __cdecl _StringBuf_FromUnicode(char *string, _ELASTOS PChar16 u16str,
        _ELASTOS Int32 count);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

_ELASTOS_NAMESPACE_BEGIN

CAR_INLINE _ELASTOS Flags32 MakeNumberFormat(_ELASTOS Flags32 fmt,
        _ELASTOS Int32 width = 0, _ELASTOS Int32 precision = 0)
{
    return ((fmt & 0xFFFF0000) | (width & 0xFF) | ((precision  <<  8) & 0xFF00));
}

/** @addtogroup CARTypesRef
  *   @{
  */
//------------------------------------------------------------------------------

class StringBuf : public CarQuintet
{
public:
    char *GetPayload() const
    {
        return (char *)m_pBuf;
    }

    Int32 GetCapacity() const
    {
        if (!m_pBuf || m_size < (Int32)sizeof(Char8)) return 0;
        return m_size / sizeof(Char8) - 1;
    }

    Int32 GetLength() const
    {
        if (m_used < (Int32)sizeof(Char8)) return 0;
        return m_used / sizeof(Char8) - 1;
    }

    Int32 GetAvailableSpace() const
    {
        return (m_size - m_used) / sizeof(Char8);
    }

    Int32 SetLength(Int32 len) const
    {
        return _StringBuf_SetLength((PCarQuintet)this, len);
    }

    Int32 ToInt32(UInt32 radix = 10) const {
        return _StringBuf_ToInt32((const PCarQuintet)this, radix);
    }

    Int64 ToInt64(UInt32 radix = 10) const {
        return _StringBuf_ToInt64((const PCarQuintet)this, radix);
    }

    Boolean ToBoolean() const {
        return _StringBuf_ToBoolean((const PCarQuintet)this);
    }

    Double ToDouble() const {
        return _StringBuf_ToDouble((const PCarQuintet)this);
    }

    Int32 GetCharCount() const
    {
        if (m_used < (Int32)sizeof(Char8)) return 0;
        return _StringBuf_GetCharCount((PCarQuintet)this);
    }

    Char32 GetChar(Int32 index) const
    {
        if (m_used < (Int32)sizeof(Char8)) return 0xFFFFFFFF;
        return _StringBuf_GetChar((PCarQuintet)this, index);
    }

    //---- Contains ----
    Boolean Contains(const char* substr,
            StringCase stringCase = StringCase_Sensitive) const {
        return _StringBuf_Contains((const PCarQuintet)this, substr,
            stringCase);
    }

    Boolean StartWith(const char* substr,
            StringCase stringCase = StringCase_Sensitive) const {
        return _StringBuf_StartWith((const PCarQuintet)this, substr, stringCase);
    }

    Boolean EndWith(const char* substr,
            StringCase stringCase = StringCase_Sensitive) const {
        return _StringBuf_EndWith((const PCarQuintet)this, substr, stringCase);
    }

    //---- IndexOf ----
    Int32 IndexOf(Char8 ch,  StringCase stringCase = StringCase_Sensitive) const {
        return _StringBuf_IndexOf_Char8((const PCarQuintet)this, ch, stringCase);
    }

    Int32 IndexOfAny(const char* strCharSet,
            StringCase stringCase  = StringCase_Sensitive) const {
        return _StringBuf_IndexOf_AnyChar8((const PCarQuintet)this, strCharSet,
            stringCase);
    }

    Int32 IndexOfChar(Char32 ch,
            StringCase stringCase = StringCase_Sensitive) const {
        return _StringBuf_IndexOf_Char((const PCarQuintet)this, ch, stringCase);
    }

    Int32 IndexOfAnyChar(Char32 *strCharSet,
            StringCase stringCase = StringCase_Sensitive) const {
        return _StringBuf_IndexOf_AnyChar((const PCarQuintet)this, strCharSet,
            stringCase);
    }

    Int32 IndexOf(const char* str,
            StringCase stringCase  = StringCase_Sensitive) const {
        return _StringBuf_IndexOf_Substring((const PCarQuintet)this, str,
            stringCase);
    }

    //---- LastIndexOf ----
    Int32 LastIndexOf(Char8 ch,
            StringCase stringCase = StringCase_Sensitive) const {
        return _StringBuf_LastIndexOf_Char8((const PCarQuintet)this, ch, stringCase);
    }

    Int32 LastIndexOfAny(const char* strCharSet,
            StringCase stringCase  = StringCase_Sensitive) const {
        return _StringBuf_LastIndexOf_AnyChar8((const PCarQuintet)this, strCharSet,
            stringCase);
    }

    Int32 LastIndexOfChar(Char32 ch,
            StringCase stringCase = StringCase_Sensitive) const {
        return _StringBuf_LastIndexOf_Char((const PCarQuintet)this, ch, stringCase);
    }

    Int32 LastIndexOfAnyChar(Char32 *strCharSet,
            StringCase stringCase = StringCase_Sensitive) const {
        return _StringBuf_LastIndexOf_AnyChar((const PCarQuintet)this, strCharSet,
            stringCase);
    }

    Int32 LastIndexOf(const char* str,
            StringCase stringCase  = StringCase_Sensitive) const {
        return _StringBuf_LastIndexOf_Substring((const PCarQuintet)this, str,
            stringCase);
    }

    //---- Substring ----
    char* Substring(Int32 start, StringBuf& sub) {
        return _StringBuf_Substring((const PCarQuintet)this, start,
            (PCarQuintet)&sub);
    }

    char* Substring(Int32 start, Int32 len, StringBuf& sub) {
        return _StringBuf_Substring_Length((const PCarQuintet)this, start,
            len, (PCarQuintet)&sub);
    }

    //---- ToLowerCase ----
    StringBuf& ToLowerCase() {
        _StringBuf_ToLowerCase((PCarQuintet)this);
        return *this;
    }

    //---- ToUpperCase ----
    StringBuf& ToUpperCase() {
        _StringBuf_ToUpperCase((PCarQuintet)this);
        return *this;
    }

    //---- TrimStart ----
    StringBuf& TrimStart() {
        _StringBuf_TrimStart((PCarQuintet)this);
        return *this;
    }

    //---- TrimEnd ----
    StringBuf& TrimEnd() {
        _StringBuf_TrimEnd((PCarQuintet)this);
        return *this;
    }

    //---- Trim ----
    StringBuf& Trim() {
        _StringBuf_Trim((PCarQuintet)this);
        return *this;
    }


    //
    StringBuf& PadLeft(Int32 width) {
        _StringBuf_PadLeft((PCarQuintet)this, width);
        return *this;
    }

    StringBuf& PadLeft(Int32 width, Char8 ch) {
        _StringBuf_PadLeft_Char8((PCarQuintet)this, width, ch);
        return *this;
    }

    StringBuf& PadRight(Int32 width) {
        _StringBuf_PadRight((PCarQuintet)this, width);
        return *this;
    }

    StringBuf& PadRight(Int32 width, Char8 ch) {
        _StringBuf_PadRight_Char8((PCarQuintet)this, width, ch);
        return *this;
    }

    operator char*() const
    {
        return (char *)m_pBuf;
    }

    operator CString() const
    {
        return (char *)m_pBuf;
    }

    Boolean IsNull() const
    {
        return m_pBuf == NULL;
    }

    Boolean IsEmpty() const
    {
        assert(m_pBuf);
        return (m_used == sizeof(Char8)) && (*(Char8 *)m_pBuf == '\0');
    }

    Boolean IsNullOrEmpty() const
    {
        return (m_pBuf == NULL)
                || ((m_used == sizeof(Char8)) && (*(Char8 *)m_pBuf == '\0'));
    }

    void SetEmpty()
    {
        assert(m_pBuf);
        m_used = sizeof(Char8);
        *(Char8 *)m_pBuf = '\0';
    }

    StringBuf& Insert(Int32 offset, const char* s)
    {
        _StringBuf_Insert(this, offset, (const char*)s);
        return *this;
    }

    StringBuf& Append(const char* s)
    {
        return Insert(GetLength(), s);
    }

    StringBuf& Append(const char* s, Int32 offset, Int32 count)
    {
        _StringBuf_Append_String(this, s, offset, count);
        return *this;
    }

    StringBuf& Append(Char16 wc)
    {
        _StringBuf_Append_Char16(this, wc);
        return *this;
    }

    StringBuf& Append(Char8 ac)
    {
        _StringBuf_Append_Char8(this, ac);
        return *this;
    }

    StringBuf& Append(Boolean boolean)
    {
        if (boolean) {
            return Append("True");
        }
        else {
            return Append("False");
        }
    }

    StringBuf& Append(Int8 value, Flags32 fmt = 0)
    {
        _StringBuf_Append_Int8(this, value, fmt);
        return *this;
    }

    StringBuf& Append(Int16 value, Flags32 fmt = 0)
    {
        _StringBuf_Append_Int16(this, value, fmt);
        return *this;
    }

    StringBuf& Append(Int32 value, Flags32 fmt = 0)
    {
        _StringBuf_Append_Int32(this, value, fmt);
        return *this;
    }

    StringBuf& Append(Int64 value, Flags32 fmt = 0)
    {
        _StringBuf_Append_Int64(this, value, fmt);
        return *this;
    }

    StringBuf& Append(Double value, Flags32 fmt = 0)
    {
        _StringBuf_Append_Double(this, value, fmt);
        return *this;
    }

    StringBuf& Append(const char* fmt, const DateTime& dateTime){
        _StringBuf_Append_DateTime(this, fmt, (PDateTime)&dateTime);
        return *this;
    }

    StringBuf& operator<<(const char* s)
    {
        return Append(s);
    }

    StringBuf& operator<<(Char8 ac)
    {
        return Append(ac);
    }

    StringBuf& operator<<(Boolean boolean)
    {
        return Append(boolean);
    }

    StringBuf& operator<<(Int8 value)
    {
        return Append(value);
    }

    StringBuf& operator<<(Int16 value)
    {
        return Append(value);
    }

    StringBuf& operator<<(Int32 value)
    {
        return Append(value);
    }

    StringBuf& operator<<(Int64 value)
    {
        return Append(value);
    }

    StringBuf& operator<<(Double value)
    {
        return Append(value);
    }

    StringBuf& Replace(Int32 offset, Int32 count, const char* s)
    {
        _StringBuf_Replace(this, offset, count, s);
        return *this;
    }

    StringBuf& Copy(const char* s)
    {
        if (s == NULL) SetEmpty();
        _StringBuf_Copy(this, s);
        return *this;
    }

    StringBuf& Copy(const char* s, Int32 len)
    {
        SetEmpty();
        _StringBuf_Append_String(this, s, 0, len);
        return *this;
    }

    // append multiple char string to StringBuf
    StringBuf& __cdecl Concatenate(const char* firststr, ...);

    Int32 Compare(const char* s, StringCase stringCase = StringCase_Sensitive) const {
        return _String_Compare((char *)m_pBuf, s, stringCase);
    }

    Int32 Compare(const StringBuf& sb, StringCase stringCase = StringCase_Sensitive) const {
        return _String_Compare((char *)m_pBuf, sb, stringCase);
    }

    StringBuf *Clone() const {
        return (StringBuf *)_CarQuintet_Clone((const PCarQuintet)this);
    }

    Char8 &operator[](Int32 index) {
        assert(index >= 0 && index < GetLength());
        return ((Char8*)m_pBuf)[index];
    }

    const Char8 &operator[](Int32 index) const {
        assert(index >= 0 && index < GetLength());
        return ((Char8*)m_pBuf)[index];
    }

    static StringBuf *Alloc(Int32 length) {
        return (StringBuf *)_StringBuf_Alloc(length);
    }

    static StringBuf *Alloc(char *pstr, Int32 size) {
        assert(pstr);
        return (StringBuf *)_StringBuf_Alloc_Box(pstr, size);
    }

    static void Free(StringBuf *pBuf) {
        _CarQuintet_Free(pBuf);
    }

    StringBuf(char *pstr, Int32 size) {
        assert(pstr && size >= 0);
        _StringBuf_Box_Init(this, pstr, size, FALSE);
    }

    StringBuf& EncodeURL(const char* str, const char* Reserved = NULL) {
        SetEmpty();
        _StringBuf_EncodeURL(this, str, Reserved);
        return *this;
    }

    StringBuf& DecodeURL(const char* s) {
        SetEmpty();
        _StringBuf_DecodeURL(this, s);
        return *this;
    }

    Char16 *ToUnicode(Char16 *u16str, Int32 count) {
        _StringBuf_ToUnicode(u16str, (char *)m_pBuf, count);
        return u16str;
    }

    StringBuf& FromUnicode(Char16 *u16str) {
        SetEmpty();
        Int32 ret = _StringBuf_FromUnicode((char *)m_pBuf, u16str, m_size);
        m_used = ret + 1;
        return *this;
    }

private:
    void operator = (const StringBuf& buf) {}
    StringBuf(const StringBuf& buf) {}
    void* operator new(size_t) { return (void *)1; } // prohibit 'new' operator

    // prohibit comparison operator
    _ELASTOS Boolean operator<(const StringBuf& str) const {return FALSE;} // comparisons
    _ELASTOS Boolean operator>(const StringBuf& str) const {return FALSE;}
    _ELASTOS Boolean operator==(const StringBuf& str) const {return FALSE;}
    _ELASTOS Boolean operator!=(const StringBuf& str) const {return FALSE;}
    _ELASTOS Boolean operator<=(const StringBuf& str) const {return FALSE;}
    _ELASTOS Boolean operator>=(const StringBuf& str) const {return FALSE;}

    _ELASTOS Boolean operator<(const char* str) const {return FALSE;} // comparisons String
    _ELASTOS Boolean operator>(const char* str) const {return FALSE;}
    _ELASTOS Boolean operator==(const char* str) const {return FALSE;}
    _ELASTOS Boolean operator!=(const char* str) const {return FALSE;}
    _ELASTOS Boolean operator<=(const char* str) const {return FALSE;}
    _ELASTOS Boolean operator>=(const char* str) const {return FALSE;}

protected:
    StringBuf() {}
};
/** @} */

/** @addtogroup BaseTypesRef
  *   @{
  */
template<Int32 Length>
class StringBuf_ : public StringBuf
{
public:
    StringBuf_()
    {
        _CarQuintet_Init(this, m_buf, (Length + 1) *  sizeof(Char8),
                        sizeof(Char8), CarQuintetFlag_Type_Char8);
#ifndef _RELEASE
        _MemoryBuf_SetByteValue(this, 0xCC);
#endif
        *(Char8 *)m_pBuf = '\0';
    }

protected:
    Char8 m_buf[Length ?((Length + 1) ? (Length + 1) : (UInt32)(-1)) : sizeof(Char8)];
private:
    // prohibit "=" operator
    StringBuf_& operator = (const StringBuf& buf) {}
    StringBuf_& operator = (const StringBuf_& buf) {}
    StringBuf_(const StringBuf& buf) {}
    StringBuf_(const StringBuf_& buf) {}
};
/** @} */

//==========================================================================
// StringBuf CAR_INLINE functions for CPP style
//

CAR_INLINE StringBuf& __cdecl StringBuf::Concatenate(const char* firststr,...)
{
    va_list argptr;
    va_start(argptr, firststr);

    const char* pstr = firststr;
    while (pstr != NULL) {
        Insert(GetLength(), pstr);
        pstr = va_arg(argptr, const char*);
    }
    va_end(argptr);

    return *this;
}

typedef StringBuf*                  PStringBuf;

#define NULL_STRINGBUF \
            *(const StringBuf *)_CarQuintet_GetNullValue( \
            CarQuintetFlag_Type_Char8)

_ELASTOS_NAMESPACE_END

#else //!__cplusplus

typedef CarQuintet StringBuf;

#define NULL_STRINGBUF 0

//StringBuf
#define STRINGBUF_BOX(pCq, pBuf, size, used) \
            do { \
                assert((size) >= 0 && (used) >= 0); \
                (pCq)->m_flags = CarQuintetFlag_Type_Char8; \
                (pCq)->m_reserve = 0; \
                (pCq)->m_size = size + 1; \
                (pCq)->m_used = used + 1; \
                (pCq)->m_pBuf = pBuf; \
            } while (0)

#define STRINGBUF_UNBOX(Cq, ppBuf, pSize, pUsed) \
            do { \
                (*ppBuf) = (Cq).m_pBuf; \
                *(Int32*)pSize= (Cq).m_size / sizeof(char) - 1; \
                *(Int32*)pUsed = (Cq).m_used / sizeof(char) - 1; \
            } while (0)

#endif //__cplusplus

#endif //__EZSTRBUF_H__
