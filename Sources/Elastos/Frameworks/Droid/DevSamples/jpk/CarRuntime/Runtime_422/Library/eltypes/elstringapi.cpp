
#include <sys/types.h>
#include <eltypes.h>
#include <elsharedbuf.h>
#include <elstring.h>
#include <stdlib.h>
#include <ctype.h>

#define _atoi64 atol

_ELASTOS_NAMESPACE_USING

EXTERN_C SharedBuffer* gElEmptyStringBuf = NULL;
EXTERN_C char* gElEmptyString = NULL;

static void InitString()
{
    SharedBuffer* buf = SharedBuffer::Alloc(1);
    char* str = (char*)buf->GetData();
    *str = 0;
    gElEmptyStringBuf = buf;
    gElEmptyString = str;
}

static void UninitString()
{
    SharedBuffer::GetBufferFromData(gElEmptyString)->Release();
    gElEmptyStringBuf = NULL;
    gElEmptyString = NULL;
}

class LibUtilsFirstStatics
{
public:
    LibUtilsFirstStatics()
    {
        InitString();
    }

    ~LibUtilsFirstStatics()
    {
        UninitString();
    }
};

static LibUtilsFirstStatics gFirstStatics;

#ifdef  __cplusplus
extern "C" {
#endif

Int32 _StringCaseCompare(const char *str1, const char *str2,
            Int32 count);

char* _getStringSubstring(const char *str, Int32 start,
            Int32 len, char *substr);
Int32 _getStringIndexOf(const char *str, Char32 ch,
            StringCase strCase);
Int32 _getStringLastIndexOf(const char *str, Char32 ch,
            StringCase strCase);
Int32 _getStringIndexOfAny(const char *str1, Char32 *str2,
            StringCase strCase);
Int32 _getStringLastIndexOfAny(const char *str1, Char32 *str2,
            StringCase strCase);
Int32 _getStringLowerCase(char *dest, Int32 dlen,
            const char *string, Int32 slen);
Int32 _getStringUpperCase(char *dest, Int32 dlen,
            const char *string, Int32 slen);
Char32 _getChar32LowerCase(Char32 ch);
Char32 _getChar32UpperCase(Char32 ch);
char* _getStringCaseStrstr(const char *string, const char *substr);
char* _getStringCaseStrpbrk(const char *string, const char *strCharSet);
char* _getStringCaseStrchr(const char *string, char ch);
char* _getStringCaseStrrchr(const char *string, char ch);


char* __cdecl _String_Duplicate(const char *strSource)
{
    size_t len;
    char * copy;

    if (!strSource) {
        return NULL;
    }

    len = strlen(strSource) + 1;
    if ((copy = (char *)malloc(len)) == NULL) return NULL;
    memcpy(copy, strSource, len);
    return(copy);
}

void __cdecl _String_Free(char * str)
{
    if (str) {
        free((void *)str);
    }
}

Int32 __cdecl _String_GetLength(const char *str, Int32 maxLen)
{
    if (!str || maxLen < -1) return -1;
    if (maxLen == 0) return 0;
    if (maxLen == -1)  return strlen(str);

    const char *p = str;
    int i = 0;
    while (*p) {
        if (i >= maxLen) return -1;
        p++;
        i++;
    }
    return (p - str);
}

Int32 __cdecl _String_Compare(const char *string1, const char *string2,
    StringCase stringCase)
{
    if (string1 == string2) return 0;
    if (!string1) return -2;
    if (!string2) return 2;

    if (stringCase == StringCase_Sensitive) {
        return strcmp(string1, string2);
    }
    else {
        Int32 count = MAX(strlen(string1), strlen(string2));
        return _StringCaseCompare(string1, string2, count);
    }
}

UInt32 _String_GetCharCount(const char *src, UInt32 src_len)
{
    if (src == NULL || src_len == 0) {
        return 0;
    }
    UInt32 ret = 0;
    const char* cur;
    const char* end;
    UInt32 num_to_skip;
    for (cur = src, end = src + src_len, num_to_skip = 1;
         cur < end;
         cur += num_to_skip, ret++) {
        const char first_char = *cur;
        num_to_skip = 1;
        if ((first_char & 0x80) == 0) {  // ASCII
            continue;
        }
        Int32 mask;

        for (mask = 0x40; (first_char & mask); num_to_skip++, mask >>= 1) {
        }
    }
    return ret;
}

static Int32 _String_GetCharInternal(const char* cur, UInt32 *num_read)
{
    const char first_char = *cur;
    if ((first_char & 0x80) == 0) { // ASCII
        *num_read = 1;
        return *cur;
    }
    cur++;
    Char32 mask, to_ignore_mask;
    UInt32 num_to_read = 0;
    Char32 utf32 = first_char;
    for (num_to_read = 1, mask = 0x40, to_ignore_mask = 0xFFFFFF80;
         (first_char & mask);
         num_to_read++, to_ignore_mask |= mask, mask >>= 1) {
        // 0x3F == 00111111
        utf32 = (utf32 << 6) + (*cur++ & 0x3F);
    }
    to_ignore_mask |= mask;
    utf32 &= ~(to_ignore_mask << (6 * (num_to_read - 1)));

    *num_read = num_to_read;
    return static_cast<Int32>(utf32);
}

Int32 _String_GetCharByOffset(const char *src, UInt32 src_len,
        UInt32 offset, UInt32 *next_offset)
{
    if (offset >= src_len) {
        return -1;
    }
    UInt32 dummy_offset;
    if (next_offset == NULL) {
        next_offset = &dummy_offset;
    }
    UInt32 num_read;
    Int32 ret = _String_GetCharInternal(src + offset, &num_read);
    if (ret >= 0) {
        *next_offset = offset + num_read;
    }

    return ret;
}

Int32 _String_GetChar(const char *src, UInt32 src_len,
        UInt32 index)
{
    Int32 offset = 0;

    if (index > 0) {
        Int32 i = 0;
        while (index > 0) {
            index--;
            if (i >= src_len) {
                return -1;
            }
            if ((src[i++] & 0xc0) == 0xc0) {
                if (i >= src_len) {
                    return -1;
                }
                while ((src[i++] & 0xc0) == 0x80) {
                    if (i >= src_len) {
                        return -1;
                    }
                }
                i--;
            }
        }
        offset = i;
    }
    return _String_GetCharByOffset(src, src_len, offset, NULL);
}

Int32 __cdecl _String_ToInt32(const char *string, UInt32 radix)
{
    if (!string) return -1;

    if (radix < 2 || radix > 36) {
        return -1;
    }

    Boolean negative = string[0] == '-';
    Int32 sum = 0;
    for (Int32 i= negative ? 1 : 0; string[i]; ++i) {
        char c = string[i];
        Int32 d = -1;
        if ('0' <= c && c <= '9') {
            d = c - '0';
        }
        else if ('a' <= c && c <= 'z') {
            d = 10 + (c - 'a');
        }
        else if ('A' <= c && c <= 'Z') {
            d = 10 + (c - 'A');
        }
        else {
            return -1;
        }
        if (d >= radix)
            return -1;
        sum = sum * radix + d;
    }
    return negative ? -sum : sum;
}

Int64 __cdecl _String_ToInt64(const char *string, UInt32 radix)
{
    if (!string) return -1;

    if (radix < 2 || radix > 36) {
        return -1;
    }

    Boolean negative = string[0] == '-';
    Int64 sum = 0;
    for (Int32 i= negative ? 1 : 0; string[i]; ++i) {
        char c = string[i];
        Int32 d = -1;
        if ('0' <= c && c <= '9') {
            d = c - '0';
        }
        else if ('a' <= c && c <= 'z') {
            d = 10 + (c - 'a');
        }
        else if ('A' <= c && c <= 'Z') {
            d = 10 + (c - 'A');
        }
        else {
            return -1;
        }
        if (d >= radix)
            return -1;
        sum = sum * radix + d;
    }
    return negative ? -sum : sum;
}

Boolean __cdecl _String_ToBoolean(const char *string)
{
    if (!string) return FALSE;
    if (strcmp(string, "1") == 0 || strcasecmp(string, "true") == 0) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}

Double __cdecl _String_ToDouble(const char *string)
{
    if (!string) return 0.0;
    return strtod(string, NULL);
}

//---- Contains ----
Boolean __cdecl _String_Contains(const char *string, const char *substr,
            StringCase stringCase)
{
   if (!string || !substr || substr[0] == 0) return FALSE;

    if (stringCase == StringCase_Sensitive) {
        return strstr(string, substr) != NULL ? TRUE : FALSE;
    }
    else {
        return _getStringCaseStrstr(string, substr) != NULL ? TRUE : FALSE;
    }
}

//---- StartWith ----
Boolean __cdecl _String_StartWith(const char *string, const char *substr,
            StringCase stringCase)
{
    if (!string || !substr) return FALSE;
    if (substr[0] == '\0') return TRUE;

    if (stringCase == StringCase_Sensitive) {
        return !strncmp(string, substr, strlen(substr));
    }
    else {
        return !_StringCaseCompare(string, substr, strlen(substr));
    }
}

//---- EndWith ----
Boolean __cdecl _String_EndWith(const char *string, const char *substr,
            StringCase stringCase)
{
    if (!string || !substr) return FALSE;
    if (substr[0] == '\0') return TRUE;

    const char *endstr = string + strlen(string) - strlen(substr);
    if (endstr < string) return FALSE;

    return _String_StartWith(endstr, substr, stringCase);
}

//---- RegionMatches ----
Boolean __cdecl _String_RegionMatches(const char *str,
            Int32 thisStart, const char * otherStr, Int32 start, Int32 length,
            StringCase stringCase)
{
    if (!str || !otherStr) return FALSE;
    if (otherStr[0] == '\0') return TRUE;

    Int32 otherStrLen = strlen(otherStr);
    if (start < 0 || otherStrLen - start < length) {
        return FALSE;
    }

    Int32 strLen = strlen(str);
    if (thisStart < 0 || strLen - thisStart < length) {
        return FALSE;
    }
    if (length <= 0) {
        return TRUE;
    }

    const char* str1 = str + thisStart;
    const char* str2 = otherStr + start;
    if (stringCase == StringCase_Sensitive) {
        return !strncmp(str1, str2, length);
    }
    else {
        return !_StringCaseCompare(str1, str2, length);
    }
}

//---- IndexOf ----
Int32 __cdecl _String_IndexOf_Char8(const char *string, char ch,
            StringCase stringCase)
{
    if (!string || !ch) return -1;

    const char *first = NULL;
    if (stringCase == StringCase_Sensitive) {
        first = strchr(string, ch);
    }
    else {
        first = _getStringCaseStrchr(string, ch);
    }
    if (first) return first - string;
    else return -1;
}

Int32 __cdecl _String_IndexOf_AnyChar8(const char *string, const char* strCharSet,
            StringCase stringCase)
{
    if (!string || !strCharSet) return -1;

    const char *ret = NULL;
    if (stringCase == StringCase_Sensitive) {
        ret = strpbrk(string, strCharSet);
    }
    else {
        ret = _getStringCaseStrpbrk(string, strCharSet);
    }
    if (ret) return ret - string;
    else return -1;
}

Int32 __cdecl _String_IndexOf_Char(const char *string, Char32 ch,
            StringCase stringCase)
{
    if (!string) return -1;

    return _getStringIndexOf(string, ch, stringCase);
}

Int32 __cdecl _String_IndexOf_AnyChar(const char *string, Char32 *strCharSet,
            StringCase stringCase)
{
    if (!string || !strCharSet) return -1;

    return _getStringIndexOfAny(string, strCharSet, stringCase);
}

Int32 __cdecl _String_IndexOf_Substring(const char *string, const char *substr,
            StringCase stringCase)
{
    if (!string || !substr) return -1;

    const char *index = NULL;
    if (stringCase == StringCase_Sensitive) {
        index = strstr(string, substr);
    }
    else {
        index = _getStringCaseStrstr(string, substr);
    }
    return index ? index - string : -1;
}

//---- LastIndexOf ----
Int32 __cdecl _String_LastIndexOf_Char8(const char *string, char ch,
            StringCase stringCase)
{
    if (!string || !ch) return -1;

    const char *last = NULL;
    if (stringCase == StringCase_Sensitive) {
        last = strrchr(string, ch);
    }
    else {
        last = _getStringCaseStrrchr(string, ch);
    }
    if (last) return last - string;
    else return -1;
}

Int32 __cdecl _String_LastIndexOf_AnyChar8(const char *string, const char* strCharSet,
            StringCase stringCase)
{
    if (!string) return -1;

    const char *ret = string;
    const char *last;
    while(*ret) {
        if (stringCase == StringCase_Sensitive) {
            last = strpbrk(ret, strCharSet);
        }
        else {
            last = _getStringCaseStrpbrk(ret, strCharSet);
        }
        if (last) ret = last + 1;
        else break;
    }
    return ret - string - 1;
}

Int32 __cdecl _String_LastIndexOf_Char(const char *string, Char32 ch,
            StringCase stringCase)
{
    if (!string) return -1;

    return _getStringLastIndexOf(string, ch, stringCase);
}

Int32 __cdecl _String_LastIndexOf_AnyChar(const char *string, Char32 *strCharSet,
            StringCase stringCase)
{
    if (!string || !strCharSet) return -1;

    return _getStringLastIndexOfAny(string, strCharSet, stringCase);
}

Int32 __cdecl _String_LastIndexOf_Substring(const char *string, const char *substr,
            StringCase stringCase)
{
    if (!string || !substr) return -1;

    if (substr[0] == '\0') {
        int len = strlen(string);
        return len ? len - 1 : 0;
    }

    const char *index = NULL;
    if (stringCase == StringCase_Sensitive) {
        index = strstr(string, substr);
    }
    else {
        index = _getStringCaseStrstr(string, substr);
    }
    if (!index) return -1;

    const char *last;
    do {
    	last = index;
    	if (stringCase == StringCase_Sensitive) {
        	index = strstr(last + 1, substr);
        }
        else {
            index = _getStringCaseStrstr(last + 1, substr);
        }
    } while(index);

    return last - string;
}

//---- Substring ----
char* __cdecl _String_Substring_Buffer(const char *string, Int32 start, PCarQuintet pCq)
{
    if (!string || !pCq || !pCq->m_pBuf || start < 0) return NULL;

	Int32 nlength = (int)strlen(string);
	if (start > nlength) return NULL;

    Int32 len = MIN(nlength - start + 1, pCq->m_size - 1);
    _getStringSubstring(string, start, len,
                (char *)pCq->m_pBuf);
    pCq->m_used = strlen((char *)pCq->m_pBuf) + 1;
    return  (char *)pCq->m_pBuf;
}

char* __cdecl _String_Substring(const char *string, Int32 start)
{
    if (!string || start < 0 || start > (int)strlen(string)) return NULL;

    return _getStringSubstring(string, start, -1, NULL);
}

char* __cdecl _String_Substring_Length_Buffer(const char *string, Int32 start, Int32 len,
    PCarQuintet pCq)
{
    if (!string || !pCq || !pCq->m_pBuf || start < 0 || len < 0) return NULL;

	Int32 nlength = (int)strlen(string);
	if (start > nlength) return NULL;

    len = MIN(nlength - start + 1, len);
    len = MIN(len, pCq->m_size - 1);
    _getStringSubstring(string, start, len, (char *)pCq->m_pBuf);
    pCq->m_used = strlen((char *)pCq->m_pBuf) + 1;
    return  (char *)pCq->m_pBuf;
}

//---- ToLowerCase ----
char* __cdecl _String_ToLowerCase(const char *string, PCarQuintet pCq)
{
    if (!string || !pCq || !pCq->m_pBuf) return NULL;
    Int32 len = strlen(string);
    Int32 Cap = pCq->m_size / sizeof(Char8) - 1;

    Int32 ret = _getStringLowerCase((char *)pCq->m_pBuf,
            Cap, string, len);
    if (ret == -1) return NULL;

    pCq->m_used = (ret + 1) * sizeof(Char8);
    *(Char8 *)((Byte *)pCq->m_pBuf + pCq->m_used - sizeof(Char8)) = '\0';
    return (char *)pCq->m_pBuf;
}

//---- ToUpperCase ----
char* __cdecl _String_ToUpperCase(const char *string, PCarQuintet pCq)
{
    if (!string || !pCq || !pCq->m_pBuf) return NULL;
    Int32 len = strlen(string) + 1;
    Int32 Cap = pCq->m_size / sizeof(Char8) - 1;

    Int32 ret = _getStringUpperCase((char *)pCq->m_pBuf,
            Cap, string, len);
    if (ret == -1) return NULL;

    pCq->m_used = (ret + 1) * sizeof(Char8);
    *(Char8 *)((Byte *)pCq->m_pBuf + pCq->m_used - sizeof(Char8)) = '\0';
    return (char *)pCq->m_pBuf;
}

Char32 __cdecl _String_Char32_ToLowerCase(Char32 ch)
{
    return _getChar32LowerCase(ch);
}

Char32 __cdecl _String_Char32_ToUpperCase(_ELASTOS Char32 ch)
{
    return _getChar32UpperCase(ch);
}

//---- TrimStart ----
char* __cdecl _String_TrimStart(const char *string, PCarQuintet pCq)
{
    if (!string || !pCq || !pCq->m_pBuf) return NULL;

    while (isspace(*string) && *string) {
        string++;
    }

    _StringBuf_Copy(pCq, string);

    return (char *)pCq->m_pBuf;
}

//---- TrimEnd ----
char* __cdecl _String_TrimEnd(const char *string, PCarQuintet pCq)
{
    if (!string || !pCq || !pCq->m_pBuf) return NULL;

    const char* pend = string + strlen(string) - 1;
    while (isspace(*pend) && pend >= string) {
        pend--;
    }

    Int32 nLen = (pend - string + 1) * sizeof(Char8);
    Int32 nCap = pCq->m_size - sizeof(Char8);
    nLen = MIN(nLen, nCap);

    memcpy(pCq->m_pBuf, string, nLen);
    *(Char8 *)((Byte *)pCq->m_pBuf + nLen) = '\0';
    pCq->m_used = nLen + sizeof(Char8);

    return (char *)pCq->m_pBuf;
}

//---- Trim ----
char* __cdecl _String_Trim(const char *string, PCarQuintet pCq)
{
    return _String_TrimEnd(_String_TrimStart(string, pCq), pCq);
}

//---- HashCode ----
Int32 _String_GetHashCode(const char *string)
{
    Int64 h = 0;

    if (string) {
        for ( ; *string; ++string) {
            h = 5 * h + *string;
        }
    }
    return (Int32)h;
}

#ifdef  __cplusplus
}
#endif
