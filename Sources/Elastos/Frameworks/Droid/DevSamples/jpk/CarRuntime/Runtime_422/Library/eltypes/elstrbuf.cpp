//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <eltypes.h>
#include <_pubcrt.h>
#include "convertstr.h"
#include <wchar.h>
#include <elstringapi.h>

#ifdef  __cplusplus
extern "C" {
#endif

_ELASTOS_NAMESPACE_USING

PCarQuintet __cdecl _StringBuf_Init(PVoid pv, Int32 length, Boolean bHeapAlloced)
{
    if (length < 0) return NULL;

    PCarQuintet pCq = (PCarQuintet)pv;

    if (pCq) {
        Int32 size = (length + 1) * sizeof(Char8);
        CarQuintetFlags flags = CarQuintetFlag_Type_Char8;
        if (bHeapAlloced) flags |= CarQuintetFlag_HeapAlloced;
        _CarQuintet_Init(pCq, pCq + 1, size, sizeof(Char8), flags);
#ifndef _RELEASE
        memset(pCq->m_pBuf, 0xCC, size);
#endif
        *(Char8 *)(pCq->m_pBuf) = '\0';
    }

    return pCq;
}

PCarQuintet __cdecl _StringBuf_Alloc(Int32 length)
{
    if (length < 0) return NULL;

    Int32 size = (length + 1) * sizeof(Char8);
    Int32 bufSize = sizeof(CarQuintet) + size;
    SharedBuffer* buf = SharedBuffer::Alloc(bufSize);
    PCarQuintet p = (PCarQuintet)(buf->GetData());
    return _StringBuf_Init(p, length, TRUE);
}

PCarQuintet __cdecl _StringBuf_Alloc_Box(char *pstr, Int32 size)
{
    if (!pstr || size < 0) return NULL;

    PCarQuintet pCq = (PCarQuintet)malloc(sizeof(CarQuintet));

    if (pCq) {
        _StringBuf_Box_Init(pCq, pstr, size, TRUE);
    }

    return pCq;
}

Int32 __cdecl _StringBuf_Insert(PCarQuintet pCq, Int32 offset, const char* s)
{
    if (!pCq || !pCq->m_pBuf || s == NULL) return -1;

    Int32 nLen = pCq->m_used - sizeof(Char8);  //original string length
    Int32 nCap = pCq->m_size - sizeof(Char8);
    offset *= sizeof(Char8);

    if ((offset < 0) || (offset > nLen)) {
        return -1;
    }

    Int32 nInsert = strlen(s) * sizeof(Char8);

    Int32 nRet = _BufferOf_Insert(pCq, offset, (Byte *)s, nInsert);

    if (pCq->m_used == pCq->m_size) {
        *(Char8 *)((Byte *)pCq->m_pBuf + nCap) = '\0';
        if (offset + nRet > nCap) nRet -= sizeof(Char8);
    }

    return nRet / sizeof(Char8);
}

Int32 __cdecl _StringBuf_Copy(PCarQuintet pCq, const char* s)
{
    if (!pCq || !pCq->m_pBuf || s == NULL) return -1;

    Int32 nLen = (strlen(s) + 1) * sizeof(Char8);
    Int32 nCap = pCq->m_size - sizeof(Char8);

    Int32 nRet =  _BufferOf_CopyEx(pCq, (Byte *)s, nLen);

    if (nRet > nCap) {
        *(Char8 *)((Byte *)pCq->m_pBuf + nCap) = '\0';
        return nCap / sizeof(Char8);
    }

    return nRet / sizeof(Char8) - 1;
}

Int32 __cdecl _StringBuf_Append_String(PCarQuintet pCq, const char* s,
    Int32 offset, Int32 count)
{
    if (!pCq || !pCq->m_pBuf || s == NULL || offset < 0 || count < 0) {
        return -1;
    }

    Int32 nLen = pCq->m_used - sizeof(Char8);  //original string length
    Int32 nCap = pCq->m_size - sizeof(Char8);

    Int32 len = 0;
    for(; len < offset + count; len++) {
        if (s[len] == '\0') break;
    }
    if (offset >= len) return -1;

    count = MIN(count, len - offset);

    offset *= sizeof(Char8);
    count *= sizeof(Char8);
    Int32 nRet = _BufferOf_Insert(pCq, nLen, (Byte *)s + offset,
        count);
    if (nRet == -1) {
        return -1;
    }
    if ((count + nLen > nCap) || (nRet < count)) {
        *(Char8 *)((Byte *)pCq->m_pBuf + nCap) = '\0';
        nRet -= sizeof(Char8);
    }

    return nRet / sizeof(Char8);
}

Int32 __cdecl _StringBuf_Append_Char16(PCarQuintet pCq, Char16 wc)
{
    if (!pCq || !pCq->m_pBuf) return -1;

    Char16 ch[2] = {wc, 0};
    StringBuf_<4> sb;
    sb.FromUnicode(ch);

    return _StringBuf_Append_String(pCq, sb.GetPayload(), sb.GetLength(), 1);
}

Int32 __cdecl _StringBuf_Append_Char8(PCarQuintet pCq, Char8 ch)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    if (ch == '\0') return 0;

    if (pCq->m_used > (Int32)(pCq->m_size - sizeof(Char8))) {
        return -1;
    }

    *(Char8 *)((Byte *)pCq->m_pBuf + (pCq->m_used - sizeof(Char8))) = ch;
    *(Char8 *)((Byte *)pCq->m_pBuf + pCq->m_used) =  '\0';
    pCq->m_used += sizeof(Char8);

    return 1;
}

#define SetNumberInfo(number, mask) \
    {numberInfo.pCq = pCq; \
     numberInfo.value.number = value; \
     numberInfo.type = mask; \
     numberInfo.fmt = fmt;}

Int32 __cdecl _StringBuf_Append_Int8(PCarQuintet pCq, Int8 value, Flags32 fmt)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    NumberInfo numberInfo;
    SetNumberInfo(n, NumberFormat_IntegerMask);

    return ConvertNumber(&numberInfo, 8);
}

Int32 __cdecl _StringBuf_Append_Int16(PCarQuintet pCq, Int16 value,
    Flags32 fmt)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    NumberInfo numberInfo;
    SetNumberInfo(n, NumberFormat_IntegerMask);

    return ConvertNumber(&numberInfo, 16);
}

Int32 __cdecl _StringBuf_Append_Int32(PCarQuintet pCq, Int32 value,
    Flags32 fmt)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    NumberInfo numberInfo;
    SetNumberInfo(n, NumberFormat_IntegerMask);

    return ConvertNumber(&numberInfo, 32);
}

Int32 __cdecl _StringBuf_Append_Int64(PCarQuintet pCq, Int64 value,
    Flags32 fmt)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    NumberInfo numberInfo;
    SetNumberInfo(n, NumberFormat_IntegerMask);

    return ConvertNumber(&numberInfo, 64);
}

Int32 __cdecl _StringBuf_Append_Double(PCarQuintet pCq, Double value,
    Flags32 fmt)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    NumberInfo numberInfo;
    SetNumberInfo(d, NumberFormat_DoubleMask);

    return ConvertNumber(&numberInfo);
}

Int32 __cdecl _StringBuf_Replace(PCarQuintet pCq, Int32 offset,
    Int32 count, const char* s)
{
    if (!pCq || !pCq->m_pBuf || s == NULL || count < 0 || offset < 0 || offset >= pCq->m_used)
        return -1;

    Int32 nLen = _String_GetLength(s, count);
    if (nLen == -1) nLen = count;
    Int32 used = pCq->m_used;

    offset *= sizeof(Char8);


    Int32 nRet = _BufferOf_Replace(pCq, offset, (PByte)s, nLen);
    char *pBuf = (char *)pCq->m_pBuf + pCq->m_used;
    nRet = nRet / sizeof(Char8);

    if (used < pCq->m_used
            || (*pBuf != '\0'
            && pCq->m_used == pCq->m_size)) {
        if (pCq->m_used < pCq->m_size)
            pCq->m_used++;
        else {
            nRet--;
            pBuf--;
        }

        *pBuf = '\0';
    }
    return nRet;
}

PCarQuintet  __cdecl _StringBuf_Box_Init(PCarQuintet pCq, char *str, Int32 size,
    Boolean bHeapAlloced)
{
    if (!pCq) return NULL;

    Int32 used = 0;
    if (str) {
        if (size) {
            used = _String_GetLength(str, size);
            if (used == -1 || used == size) { //length >= size
                str[size - 1] = '\0';
                used = size;
            }
            else {
                used++;
            }
        }
        else {
            str[0] = '\0';
            used = 0;
        }
    }
    else {
        size = 0;
    }

    CarQuintetFlags flags = CarQuintetFlag_Type_Char8;
    if (bHeapAlloced) flags |= CarQuintetFlag_HeapAlloced;
    return _CarQuintet_Init(pCq, str, size * sizeof(Char8), used * sizeof(Char8), flags);
}

Char32 _getStringCharAt(const char *str, Int32 index);
Int32 _getStringLength(const char *str);

Int32 __cdecl _StringBuf_SetLength(PCarQuintet pCq, Int32 len)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    Int32 cap = pCq->m_size / sizeof(Char8) - 1;
    Int32 oldlen = _String_GetLength((char *)pCq->m_pBuf, cap);

    if (len < 0) return oldlen;
    if (oldlen != -1) {
        if (oldlen < len) len = oldlen;
    }
    else {
        if (cap < len) len = cap;
    }

    *((char *)pCq->m_pBuf + len) = '\0';
    pCq->m_used = (len + 1) * sizeof(Char8);
    return len;
}

//---------
Int32 __cdecl _StringBuf_ToInt32(const PCarQuintet pCq, UInt32 radix)
{
    if (!pCq || !pCq->m_pBuf) return 0;
    return _String_ToInt32((const char*)pCq->m_pBuf, radix);
}

Int64 __cdecl _StringBuf_ToInt64(const PCarQuintet pCq, UInt32 radix)
{
    if (!pCq || !pCq->m_pBuf) return 0;
    return _String_ToInt64((const char*)pCq->m_pBuf, radix);
}

Boolean __cdecl _StringBuf_ToBoolean(const PCarQuintet pCq)
{
    if (!pCq || !pCq->m_pBuf) return FALSE;
    return _String_ToBoolean((const char*)pCq->m_pBuf);
}

Double __cdecl _StringBuf_ToDouble(const PCarQuintet pCq)
{
    assert(pCq && pCq->m_pBuf);
    if (!pCq || !pCq->m_pBuf) return 0.0;
    return _String_ToDouble((const char*)pCq->m_pBuf);
}

//---------
Int32 __cdecl _StringBuf_GetCharCount(const PCarQuintet pCq)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    return _getStringLength((char *)pCq->m_pBuf);
}

Char32 __cdecl _StringBuf_GetChar(const PCarQuintet pCq, Int32 index)
{
    if (!pCq || !pCq->m_pBuf) return 0xFFFFFFFF;
    if ((index < 0) || (index >= (Int32)(pCq->m_used / sizeof(Char8)))) {
        return 0xFFFFFFFF;
    }

    return _getStringCharAt((char *)pCq->m_pBuf, index);
}

//---- Contains ----
Boolean __cdecl _StringBuf_Contains(const PCarQuintet pCq, const char *substr,
    StringCase stringCase)
{
    if (!pCq || !pCq->m_pBuf) return FALSE;
    return _String_Contains((const char*)pCq->m_pBuf, substr, stringCase);
}

//---- StartWith ----
Boolean __cdecl _StringBuf_StartWith(const PCarQuintet pCq, const char *substr,
    StringCase stringCase)
{
    if (!pCq || !pCq->m_pBuf) return FALSE;
    return _String_StartWith((const char*)pCq->m_pBuf, substr, stringCase);
}

//---- EndWith ----
Boolean __cdecl _StringBuf_EndWith(const PCarQuintet pCq, const char *substr,
    StringCase stringCase)
{
    if (!pCq || !pCq->m_pBuf) return FALSE;
    return _String_EndWith((const char*)pCq->m_pBuf, substr, stringCase);
}

//---- IndexOf ----
Int32 __cdecl _StringBuf_IndexOf_Char8(const PCarQuintet pCq, char ch,
    StringCase stringCase)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    return _String_IndexOf_Char8((const char*)pCq->m_pBuf, ch, stringCase);
}

Int32 __cdecl _StringBuf_IndexOf_AnyChar8(const PCarQuintet pCq,
    const char* strCharSet, StringCase stringCase)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    return _String_IndexOf_AnyChar8((const char*)pCq->m_pBuf, strCharSet, stringCase);
}

Int32 __cdecl _StringBuf_IndexOf_Char(const PCarQuintet pCq,
    Char32 ch, StringCase stringCase)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    return _String_IndexOf_Char((const char*)pCq->m_pBuf, ch, stringCase);
}

Int32 __cdecl _StringBuf_IndexOf_AnyChar(const PCarQuintet pCq,
    Char32 *strCharSet, StringCase stringCase)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    return _String_IndexOf_AnyChar((const char*)pCq->m_pBuf, strCharSet,
        stringCase);
}

Int32 __cdecl _StringBuf_IndexOf_Substring(const PCarQuintet pCq,
    const char *substr, StringCase stringCase)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    return _String_IndexOf_Substring((const char*)pCq->m_pBuf, substr, stringCase);
}

//---- LastIndexOf ----
Int32 __cdecl _StringBuf_LastIndexOf_Char8(const PCarQuintet pCq, char ch,
    StringCase stringCase)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    return _String_LastIndexOf_Char8((const char*)pCq->m_pBuf, ch, stringCase);
}

Int32 __cdecl _StringBuf_LastIndexOf_AnyChar8(const PCarQuintet pCq,
    const char* strCharSet, StringCase stringCase)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    return _String_LastIndexOf_AnyChar8((const char*)pCq->m_pBuf, strCharSet, stringCase);
}

Int32 __cdecl _StringBuf_LastIndexOf_Char(const PCarQuintet pCq, Char32 ch,
    StringCase stringCase)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    return _String_LastIndexOf_Char((const char*)pCq->m_pBuf, ch, stringCase);
}

Int32 __cdecl _StringBuf_LastIndexOf_AnyChar(const PCarQuintet pCq,
    Char32 *strCharSet, StringCase stringCase)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    return _String_LastIndexOf_AnyChar((const char*)pCq->m_pBuf, strCharSet,
        stringCase);
}

Int32 __cdecl _StringBuf_LastIndexOf_Substring(const PCarQuintet pCq,
    const char *substr, StringCase stringCase)
{
    if (!pCq || !pCq->m_pBuf) return -1;
    return _String_LastIndexOf_Substring((const char*)pCq->m_pBuf, substr,
        stringCase);
}

//---- Substring ----
char* __cdecl _StringBuf_Substring(const PCarQuintet pCq, Int32 start, PCarQuintet sub)
{
    if (!pCq || !pCq->m_pBuf || !sub || !sub->m_pBuf) return NULL;
    return _String_Substring_Buffer((const char*)pCq->m_pBuf, start, sub);
}

char* __cdecl _StringBuf_Substring_Length(const PCarQuintet pCq,
    Int32 start, Int32 len, PCarQuintet sub)
{
    if (!pCq || !pCq->m_pBuf || !sub || !sub->m_pBuf) return NULL;
    return _String_Substring_Length_Buffer((const char*)pCq->m_pBuf, start, len,
        sub);
}

//---- ToLowerCase ----
PCarQuintet __cdecl _StringBuf_ToLowerCase(PCarQuintet pCq)
{
    if (!pCq || !pCq->m_pBuf) return NULL;
    char *string = _String_Duplicate((const char*)pCq->m_pBuf);
    if (!string) return NULL;

    _String_ToLowerCase(string, pCq);
    _String_Free(string);
    return pCq;
}

//---- ToUpperCase ----
PCarQuintet __cdecl _StringBuf_ToUpperCase(PCarQuintet pCq)
{
    if (!pCq || !pCq->m_pBuf) return NULL;
    char *string = _String_Duplicate((const char*)pCq->m_pBuf);
    if (!string) return NULL;

    _String_ToUpperCase(string, pCq);
    _String_Free(string);
    return pCq;
}

//---- TrimStart ----
PCarQuintet __cdecl _StringBuf_TrimStart(PCarQuintet pCq)
{
    if (!pCq || !pCq->m_pBuf) return NULL;
    _String_TrimStart((const char*)pCq->m_pBuf, pCq);
    return pCq;
}

//---- TrimEnd ----
PCarQuintet __cdecl _StringBuf_TrimEnd(PCarQuintet pCq)
{
    if (!pCq || !pCq->m_pBuf) return NULL;
    _String_TrimEnd((const char*)pCq->m_pBuf, pCq);
    return pCq;
}

//---- Trim ----
PCarQuintet __cdecl _StringBuf_Trim(PCarQuintet pCq)
{
    if (!pCq || !pCq->m_pBuf) return NULL;
    _String_Trim((const char*)pCq->m_pBuf, pCq);
    return pCq;
}

//---- PadLeft ----
PCarQuintet __cdecl _StringBuf_PadLeft(PCarQuintet pCq, Int32 width)
{
    return _StringBuf_PadLeft_Char8(pCq, width, ' ');
}

PCarQuintet __cdecl _StringBuf_PadLeft_Char8(PCarQuintet pCq, Int32 width,
        Char8 ch)
{
    if (!pCq || !pCq->m_pBuf || width < 0) return NULL;
    Int32 len = pCq->m_used - 1;
    if (width <= len) return NULL;

    width = MIN(width, pCq->m_size - 1);
    Int32 move = width - len;
    memmove((char *)pCq->m_pBuf + move, pCq->m_pBuf, len);
    memset(pCq->m_pBuf, ch, move);

    *((char *)pCq->m_pBuf + width) = '\0';
    pCq->m_used = width + sizeof(Char8);
    return pCq;
}

//StringBuf& StringBuf::PadCharLeft(Int32 width, Char32 ch);
//

//---- PadRight ----
PCarQuintet _StringBuf_PadRight(PCarQuintet pCq, Int32 width)
{
    return _StringBuf_PadRight_Char8(pCq, width, ' ');
}

PCarQuintet _StringBuf_PadRight_Char8(PCarQuintet pCq, Int32 width, Char8 ch)
{
    if (!pCq || !pCq->m_pBuf) return NULL;

    Int32 len = pCq->m_used - 1;
    if (width <= len) return NULL;

    width = MIN(width, pCq->m_size - 1);
    Int32 pad = width - len;
    memset((char *)pCq->m_pBuf + len, ch, pad);

    *((char *)pCq->m_pBuf + width) = '\0';
    pCq->m_used = width + sizeof(Char8);
    return pCq;
}

//StringBuf& StringBuf::PadCharRight(Int32 width, Char32 ch);
//

#define U16_IS_SURROGATE(c) (((c)&0xfffff800)==0xd800)

#define HexToChar(digit) \
    {if (digit < 10) ch = (char)('0' + digit); \
     else ch = (char)('a' - 10 + digit);}

PCarQuintet __cdecl _StringBuf_EncodeURL(PCarQuintet pCq, const char* s,
    const char* Reserved)
{
    if (!pCq || !pCq->m_pBuf || s == NULL) return NULL;
    Char8 *str = (Char8 *)pCq->m_pBuf;

    for (Int32 i = 0; i < (Int32)strlen(s) && pCq->m_used < pCq->m_size; ) {
        const char *c = s + i;
        if ((*c >= 'a' && *c <= 'z') || (*c >= 'A' && *c <= 'Z') ||
            (*c >= '0' && *c <= '9') || !!strchr(" -_.*", *c) ||
            (Reserved != NULL && Reserved[0] != '\0' && !!strchr(Reserved, *c))) {
            if (*c == ' ') {
                *str++ = '+';
            }
            else {
                *str++ = (Char8)*c;
            }
            pCq->m_used += sizeof(Char8);
            i++;
        }
        else {
            Char8 buf[10] = {0};    // rather arbitrary limit, but safe for now
            Char32 ch = _getStringCharAt(c, 0);
            strncpy(buf, (Char8 *)&ch, sizeof(Char32));

            Int32 ret = strlen(buf);
            i += ret;
            if((pCq->m_used + ret * 3 * (Int32)sizeof(Char8)) > pCq->m_size) break;

            for (Int32 j = 0; j < ret; j++) {
                *str++ = '%';
                char ch;
                HexToChar(((buf[j] >> 4) & 0xF));
                // converting to use uppercase letter as part of
		        // the hex value if ch is a letter.
                if (ch >= 'a' && ch <='z') {
                    ch -= ('a' - 'A');
                }
                *str++ = ch;
                HexToChar((buf[j] & 0xF));
                if (ch >= 'a' && ch <='z') {
                    ch -= ('a' - 'A');
                }
                *str++ = ch;
                pCq->m_used += 3 * sizeof(Char8);
            }
        }
    }

    *str = '\0';
    return pCq;
}

#define StrToHex(start, end) \
    {while(start < end) { \
         if ((*start >= '0') && (*start <= '9')) buf[pos] = (buf[pos] << 4) + *start - '0'; \
         else if ((*start >= 'a') && (*start <= 'f') ) buf[pos] = (buf[pos] << 4) + *start - 'a' + 10; \
         else if ((*start >= 'A') && (*start <= 'F') ) buf[pos] = (buf[pos] << 4) + *start - 'A' + 10; \
         else buf[pos] = '\0'; \
         start++;}}

PCarQuintet __cdecl _StringBuf_DecodeURL(PCarQuintet pCq, const char* s)
{
    if (!pCq || !pCq->m_pBuf || s == NULL) return NULL;

    Int32 numChars = strlen(s);
    Int32 i = 0;
    Char8 *str = (Char8 *)pCq->m_pBuf;

    while (i < numChars && pCq->m_used < pCq->m_size) {
        Char8 c = s[i];
        switch(c) {
            case '+':
                *str++ = ' ';
                i++;
                pCq->m_used += sizeof(Char16);
                break;
            case '%': {
                Char8 *buf = (Char8 *)malloc((numChars - i) / 3 + 1);
                if (!buf) return NULL;
                memset(buf, 0, (numChars - i) / 3 + 1);
                int pos = 0;

                while (((i + 2) < numChars) && (c == '%')) {
                    const char *start = s + i + 1;
                    StrToHex(start, (s + i + 3));
                    pos++;
                    i += 3;

                    if (i < numChars) c = s[i];
                }

                Int32 ret = strlen(buf);
                if (ret > (pCq->m_size - pCq->m_used)) return NULL;
                strncpy(str, buf, ret);

                str += ret;
                pCq->m_used += ret * sizeof(Char8);
                free(buf);
                break;
            }
            default:
                *str++ = (Char8)c;
                i++;
                pCq->m_used += sizeof(Char8);
                break;
        }
    }

    *str = '\0';
    return pCq;
}

void UnicodeToUTF16(Char16 *u16str, Char32 unicode)
{
    unicode -= 0x10000;
    *u16str = 0xD800 | (unicode >> 10);
    *(u16str + 1) = 0xDC00 | (unicode & 0x3FF);
}

Int32 __cdecl _StringBuf_ToUnicode(Char16 *u16str, const char *u8str, Int32 count)
{
    if (u16str == NULL || u8str == NULL || count < 0) {
        return NULL;
    }

    int size = strlen(u8str);
    int s = 0, d = 0;

    while (d < count) {
        unsigned char c = u8str[s];
        if ((c & 0x80) == 0 && s < size) {
            u16str[d++] = u8str[s++];
        }
        else if((c & 0xE0) == 0xC0 && s + 1 < size) {//< 110x-xxxx 10xx-xxxx
            Char16 &wideChar = u16str[d++];

            wideChar  = (u8str[s + 0] & 0x3F) << 6;
            wideChar |= (u8str[s + 1] & 0x3F);

            s += 2;
        }
        else if((c & 0xF0) == 0xE0 && s + 2 < size) {//< 1110-xxxx 10xx-xxxx 10xx-xxxx
            Char16 &wideChar = u16str[d++];

            wideChar  = (u8str[s + 0] & 0x1F) << 12;
            wideChar |= (u8str[s + 1] & 0x3F) << 6;
            wideChar |= (u8str[s + 2] & 0x3F);

            s += 3;
        }
        else if ((c & 0xF8) == 0xF0 && s + 3 < size && d + 1 < count) {//< 1111-0xxx 10xx-xxxx 10xx-xxxx 10xx-xxxx
            Char32 wideChar;

            wideChar  = (u8str[s + 0] & 0x0F) << 18;
            wideChar |= (u8str[s + 1] & 0x3F) << 12;
            wideChar |= (u8str[s + 2] & 0x3F) << 6;
            wideChar |= (u8str[s + 3] & 0x3F);

            s += 4;
            UnicodeToUTF16(&u16str[d], wideChar);
            d += 2;
        }
        else {
            break;
        }
    }

    if (d < count) u16str[d] = L'\0';

    return d;
}

Char32 UTF16ToUnicode(Char16 lead, Char16 trail)
{
    Char32 unicode = lead & 0x3FF;
    unicode = (unicode << 10) | (trail & 0x3FF);
    unicode += 0x10000;

    return unicode;
}

Int32 __cdecl _StringBuf_FromUnicode(char *u8str, PChar16 u16str, Int32 count)
{
    if (!u8str || !u16str) return NULL;

    Int32 len = 0;
    while (*u16str) {
        Char16 ch = *u16str;
        if (ch < 0x80 && len < count) {
            u8str[len++] = (char)ch;
        }
        else if(ch < 0x800 && len + 1 < count) {
            u8str[len++] = 0xc0 | (ch >> 6);
            u8str[len++] = 0x80 | (ch & 0x3f);
        }
        else if(U16_IS_SURROGATE(ch)) {
            u16str++;
            Char32 ch32 = UTF16ToUnicode(ch, *u16str);
            if (len + 3 < count) {
                u8str[len++] = 0xf0 | (ch32 >> 18);
                u8str[len++] = 0x80 | ((ch32 >> 12) & 0x3f);
                u8str[len++] = 0x80 | ((ch32 >> 6) & 0x3f);
                u8str[len++] = 0x80 | (ch32 & 0x3f);
            }
            else {
                break;
            }
        }
        else if (len + 2 < count) {
            u8str[len++] = 0xe0 | (ch >> 12);
            u8str[len++] = 0x80 | ((ch >> 6) & 0x3f);
            u8str[len++] = 0x80 | (ch & 0x3f);
        }
        else {
            break;
        }

        u16str++;
    }

    if (len < count) u8str[len] = '\0';
    return len;
}

#ifdef  __cplusplus
}
#endif
