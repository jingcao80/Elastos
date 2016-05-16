
#define _NO_INCLIST
#include <eltypes.h>
#include <limits.h>
#include <_pubcrt.h>

#include "ucase.h"

#define UNIFYCHAR32CASE(c1,c2) u_isULowercase(c1) ? u_tolower(c2) : u_toupper(c2)

Int32 _getStringLength(const char *str)
{
    const char *pEnd = str +  strlen(str) + 1;
    Int32 len = -1, count = -1;

    while (str < pEnd && count != 0) {
        if ((*str & 0x80) == 0) {
            count = 1;
        }
        else if ((*str & 0xE0) == 0xC0) {
            count = 2;
        }
        else if ((*str & 0xF0) == 0xE0) {
            count = 3;
        }
        else if ((*str & 0xF8) == 0xF0) {
            count = 4;
        }
        else {
            return -1;
        }

        str += count;
        len++;
    }

    return len;
}

Int32 _getNextCharLength(const char *str)
{
    if (!str && !*str) return 0;

    if ((*str & 0x80) == 0) return 1;
    else if ((*str & 0xE0) == 0xC0) return 2;
    else if ((*str & 0xF0) == 0xE0) return 3;
    else if ((*str & 0xF8) == 0xF0) return 4;
    else return 0;

    return 0;
}

Int32 _getNextUTF16Char(Char32 *ch, const char *str)
{
    char u8char[5] = {0};
	Char16 u16char[3];
    Int32 len = _getNextCharLength(str);
    if (len == 0 || memchr(str, '\0', len) != 0) {
        *ch = 0;
        return 0;
    }

    memcpy(u8char, str, len);
    _StringBuf_ToUnicode(u16char, u8char, 3);
    *((Char16 *)ch) = *u16char;
    *((Char16 *)ch + 1) = *(u16char + 1);

    return len;
}

Int32 _getCharAtInternal(
    /* [in] */ const Byte* cur,
    /* [out] */ Int32* numRead)
{
    const Byte firstChar = *cur;
    Char32 mask, toIgnoreMask;
    Int32 numToRead = 0;
    Char32 utf32 = firstChar;

    if ((firstChar & 0x80) == 0) { // ASCII
        *numRead = 1;
        return *cur;
    }
    cur++;

    for (numToRead = 1, mask = 0x40, toIgnoreMask = 0xFFFFFF80;
         (firstChar & mask);
         numToRead++, toIgnoreMask |= mask, mask >>= 1) {
        // 0x3F == 00111111
        utf32 = (utf32 << 6) + (*cur++ & 0x3F);
    }
    toIgnoreMask |= mask;
    utf32 &= ~(toIgnoreMask << (6 * (numToRead - 1)));

    *numRead = numToRead;
    return (Int32)utf32;
}

Char32 _getStringCharAt(const char *str, Int32 index)
{
    Int32 count = 0;
    Char32 ret = 0xFFFFFFFF;
    char *p = (char *)str;
    char *pEnd = p +  strlen(p) + 1;

    while (p && *p && p < pEnd) {
        Int32 len = _getNextCharLength(p);
        if (!len || p + len >= pEnd) break;

        if (count == index) {
            Int32 numToRead;
            return _getCharAtInternal(p, &numToRead);
        }
        count++;
        p += len;
    }

    return ret;
}

Int32 _getStringIndexOf(const char *str, Char32 ch,
            StringCase strCase)
{
    Int32 count = 0, ret = -1;
    char *p = (char *)str;
    char *pEnd = p +  strlen(p) + 1;
    Char32 c = 0xFFFFFFFF;

    while (p && *p && p < pEnd) {
        Int32 len = _getNextUTF16Char(&c, p);
        if (!len || p + len >= pEnd) break;
        else p += len;

        if (strCase == StringCase_Insensitive) {
            c = UNIFYCHAR32CASE(ch, c);
        }
        if (c == (Char32)ch) {
            ret = count;
            break;
        }
        count++;
    }

    return ret;
}

Int32 _getStringLastIndexOf(const char *str, Char32 ch,
            StringCase strCase)
{
    Int32 count = 0, ret = -1;
    char *p = (char *)str;
    char *pEnd = p +  strlen(p) + 1;
    Char32 c = 0xFFFFFFFF;

    while (p && *p && p < pEnd) {
        Int32 len = _getNextUTF16Char(&c, p);
        if (!len || p + len >= pEnd) break;
        else p += len;

        if (strCase == StringCase_Insensitive) {
            c = UNIFYCHAR32CASE(ch, c);
        }

        if (c == (Char32)ch) ret = count;
        count++;
    }

    return ret;
}

char* _getStringSubstring(const char *str, Int32 start, Int32 len, char *substr)
{
    Int32 count = 0;
    Int32 slen = strlen(str);
    char *p = (char *)str;
    char *pEnd = p +  slen + 1;
    char *p1 = p, *p2 = pEnd;
    char *p3 = p;

    if (start == slen) {
        p1 += slen;
    }
    else {
        while (p && *p && p < pEnd) {
            Int32 charlen = _getNextCharLength(p);
            if (!charlen || p + charlen >= pEnd) break;
            else p += charlen;

            if (count == start) {
                p1 = p3;
                if (len == -1) break;
            }
            if (count == start + len && len != -1) {
                p2 = p3;
                break;
            }
            count++;
            p3 = p;
        }
    }

    if (substr) {
    	*(substr + (p2 - p1)) = '\0';
        return strncpy(substr, p1, p2 - p1);
    }
    else {
        return p1;
    }
}

Int32 _getStringLowerCase(char *dest, Int32 dlen, const char *string, Int32 slen)
{
    Char32 c = 0xFFFFFFFF, ch = 0xFFFFFFFF;
    char *pdest = dest;
    char *pEnd = (char *)string + slen + 1;
    Int32 count = 0, len = 0;

    while (string && *string && string < pEnd && dlen > 0) {
        count = _getNextUTF16Char(&c, string);
        if (!count || string + count >= pEnd) break;

        ch = u_tolower(c);
        if (ch == c) {
            strncpy(dest, string, count);
            len = count;
        }
        else {
            Char16 u16[3] = {*(Char16 *)&ch, *((Char16 *)&ch + 1), 0};
            _StringBuf_FromUnicode(dest, u16, dlen);
            len = _getNextCharLength(dest);
        }
        dest += len;
        dlen -= len;
        string += count;
    }

    return dest - pdest;
}

Int32 _getStringUpperCase(char *dest, Int32 dlen,const char *string, Int32 slen)
{
    Char32 c = 0xFFFFFFFF, ch = 0xFFFFFFFF;
    char *pdest = dest;
    char *pEnd = (char *)string + slen + 1;
    Int32 count = 0, len = 0;

    while (string && *string && string < pEnd && dlen > 0) {
        count = _getNextUTF16Char(&c, string);
        if (!count || string + count >= pEnd) break;

        ch = u_toupper(c);
        if (ch == c) {
            strncpy(dest, string, count);
            len = count;
        }
        else {
            Char16 u16[3] = {*(Char16 *)&ch, *((Char16 *)&ch + 1), 0};
            _StringBuf_FromUnicode(dest, u16, dlen);
            len = _getNextCharLength(dest);
        }
        dest += len;
        dlen -= len;
        string += count;
    }

    return dest - pdest;
}

Char32 _getChar32LowerCase(Char32 ch)
{
    return u_tolower(ch);
}

Char32 _getChar32UpperCase(Char32 ch)
{
    return u_toupper(ch);
}

Int32 _StringCaseCompare(const char *str1, const char *str2,
        Int32 count)
{
    Char32 c1 = 0xFFFFFFFF, c2 = 0xFFFFFFFF;
    char *strEnd1 = (char *)str1 + strlen(str1) + 1;
    char *strEnd2 = (char *)str2 + strlen(str2) + 1;
    Int32 ret = -1, flag = 0;
    Int32 charlen1 = 0, charlen2 = 0;

    if (count == strlen(str1)) flag = 1;
    while (c1 && c2 && count > 0) {
        charlen1 = _getNextUTF16Char(&c1, str1);
        if (!charlen1 || str1 + charlen1 >= strEnd1) break;

        charlen2 = _getNextUTF16Char(&c2, str2);
        if (!charlen2 || str2 + charlen2 >= strEnd2) break;

        c2 = UNIFYCHAR32CASE(c1, c2);
        ret = c1 - c2;
        if (ret != 0) break;

        count -= flag ? charlen1 : charlen2;
        str1 += charlen1;
        str2 += charlen2;
    }

    if (ret == 0 && count > 0) return -1;
    return ret;
}

Int32 _getStringIndexOfAny(const char *str1, Char32 *str2,
            StringCase strCase)
{
    const char *p = str1;
    char *pEnd = (char *)str1 + strlen(str1) + 1;
    Char32 c1 = 0xFFFFFFFF, c2 = 0xFFFFFFFF;
    Int32 ret = 0x7FFFFFFF;

    while ((c2 = *str2++) && (ret != 0)) {
        while (c1) {
            Int32 len = _getNextUTF16Char(&c1, p);
            if (!len || p + len >= pEnd) break;
			else p += len;

            if (strCase == StringCase_Insensitive) {
                c2 = UNIFYCHAR32CASE(c1, c2);
            }
            if (c1 == c2) {
                ret = MIN(ret, p - str1 - 1);
                break;
            }
        }
        p = str1;
    }

    if (ret != 0x7FFFFFFF) return ret;
    else return -1;
}

Int32 _getStringLastIndexOfAny(const char *str1, Char32 *str2,
            StringCase strCase)
{
    const char *p = str1;
    char *pEnd = (char *)str1 + strlen(str1) + 1;
    Char32 c1 = 0xFFFFFFFF, c2 = 0xFFFFFFFF;
    Int32 ret = -1;

    while ((c2 = *str2++)) {
        while (c1) {
            Int32 len = _getNextUTF16Char(&c1, p);
            if (!len || p + len >= pEnd) break;
            else p += len;

            if (strCase == StringCase_Insensitive) {
                c2 = UNIFYCHAR32CASE(c1, c2);
            }
            if (c1 == c2) {
                ret = MAX(ret, p - str1 - 1);
            }
        }
        p = str1;
        c1 = 0xFFFFFFFF;
    }

    return ret;
}

char* _getStringCaseStrstr(const char *string, const char *substr)
{
    Char32 c1 = 0xFFFFFFFF, c2 = 0xFFFFFFFF;
    Int32 len1 = 0, len2 = 0, len = 0;

    if ((len2 = _getNextUTF16Char(&c2, substr)) != 0) {
        substr += len2;
        len = strlen(substr);
        do {
            do {
                if ((len1 = _getNextUTF16Char(&c1, string)) == 0) return NULL;
                string += len1;

                c2 = UNIFYCHAR32CASE(c1, c2);
				if (c2 == c1) break;
            } while (1);
        } while (_StringCaseCompare(string, substr, len) != 0);
        string -= len1;
    }

    return ((char *) string);
}

char* _getStringCaseStrpbrk(const char *string, const char *strCharSet)
{
    Char32 c1 = 0xFFFFFFFF, c2 = 0xFFFFFFFF;
    char *ret = NULL;
    Int32 len1 = 0, len2 = 0;

    while ((len1 = _getNextUTF16Char(&c1, string)) != 0 && !ret) {
        const char *str = strCharSet;
        while ((len2 = _getNextUTF16Char(&c2, str)) != 0) {
            c2 = UNIFYCHAR32CASE(c1, c2);
            if (c1 == c2) {
                ret = (char *)string;
                break;
            }
            str += len2;
        }
        string += len1;
    }

    return ret;
}

char* _getStringCaseStrchr(const char *string, char ch)
{
    Char32 c1 = 0xFFFFFFFF, c2 = 0xFFFFFFFF;
    char p[2] = {ch, 0};
    char *ret = NULL;
    Int32 len1 = 0, len2 = 0;

    len1 = _getNextUTF16Char(&c1, p);
    while((len2 = _getNextUTF16Char(&c2, string)) != 0) {
        c2 = UNIFYCHAR32CASE(c1, c2);
        if (c2 == c1) {
            ret = (char *)string;
            break;
        }
        string += len2;
    }

    return ret;
}

char* _getStringCaseStrrchr(const char *string, char ch)
{
    Char32 c1 = 0xFFFFFFFF, c2 = 0xFFFFFFFF;
    char p[2] = {ch, 0};
    char *ret = NULL;
    Int32 len1 = 0, len2 = 0;

    len1 = _getNextUTF16Char(&c1, p);
    while((len2 = _getNextUTF16Char(&c2, string)) != 0) {
        c2 = UNIFYCHAR32CASE(c1, c2);
        if (c2 == c1) {
            ret = (char *)string;
        }
        string += len2;
    }

    return ret;
}
