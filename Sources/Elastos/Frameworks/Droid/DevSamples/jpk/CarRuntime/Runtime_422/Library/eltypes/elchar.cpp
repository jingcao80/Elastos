//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <ctype.h>
#include <elatypes.h>

#ifdef  __cplusplus
extern "C" {
#endif

#include <wctype.h>
#define __isascii isascii

int isleadbyte(int c)
{
    if (c >= 0x80 && c <= 0xFF)
        return c;

    return 0;
}

int iswascii(wint_t c)
{
    return __isascii(c);
}


_ELASTOS_NAMESPACE_USING

//---- Char8 ----
Boolean __cdecl _Char8_IsASCII(Char8 ch)
{
    return !!__isascii(ch);
}

Boolean __cdecl _Char8_IsControl(Char8 ch)
{
    return !!iscntrl(ch);
}

Boolean __cdecl _Char8_IsDigit(Char8 ch)
{
    return !!isdigit(ch);
}

Boolean __cdecl _Char8_IsLeadByte(Char8 ch)
{
    return !!isleadbyte(ch);
}

Boolean __cdecl _Char8_IsLetter(Char8 ch)
{
    return !!isalpha(ch);
}

Boolean __cdecl _Char8_IsLetterOrDigit(Char8 ch)
{
    return !!isalnum(ch);
}

Boolean __cdecl _Char8_IsLower(Char8 ch)
{
    return !!islower(ch);
}

Boolean __cdecl _Char8_IsNumber(Char8 ch)
{
    return !!isxdigit(ch);
}

Boolean __cdecl _Char8_IsPunctuation(Char8 ch)
{
    return !!ispunct(ch);
}

//Boolean __cdecl _Char8_IsSeparator(Char8 ch)
//{
//    assert(0);
//    return false;
//}

//Boolean __cdecl _Char8_IsSymbol(Char8 ch)
//{
//    assert(0);
//    return false;
//}

Boolean __cdecl _Char8_IsUpper(Char8 ch)
{
    return !!isupper(ch);
}

Boolean __cdecl _Char8_IsWhiteSpace(Char8 ch)
{
    return !!isspace(ch);
}

Char8 __cdecl _Char8_ToLower(Char8 ch)
{
    return (Char8)tolower(ch);
}

Char8 __cdecl _Char8_ToUpper(Char8 ch)
{
    return (Char8)toupper(ch);
}

//---- Char16 ----
Boolean __cdecl _Char16_IsASCII(Char16 ch)
{
    return !!iswascii(ch);
}

Boolean __cdecl _Char16_IsControl(Char16 ch)
{
    return !!iswcntrl(ch);
}

Boolean __cdecl _Char16_IsDigit(Char16 ch)
{
    return !!iswdigit(ch);
}

Boolean __cdecl _Char16_IsLetter(Char16 ch)
{
    return !!iswalpha(ch);
}

Boolean __cdecl _Char16_IsLetterOrDigit(Char16 ch)
{
    return !!iswalnum(ch);
}

Boolean __cdecl _Char16_IsLower(Char16 ch)
{
    return !!iswlower(ch);
}

Boolean __cdecl _Char16_IsNumber(Char16 ch)
{
    return !!iswxdigit(ch);
}

Boolean __cdecl _Char16_IsPunctuation(Char16 ch)
{
    return !!iswpunct(ch);
}

//Boolean __cdecl _Char16_IsSeparator(Char16 ch)
//{
//    assert(0);
//    return false;
//}

//Boolean __cdecl _Char16_IsSymbol(Char16 ch)
//{
//    assert(0);
//    return false;
//}

Boolean __cdecl _Char16_IsUpper(Char16 ch)
{
    return !!iswupper(ch);
}

Boolean __cdecl _Char16_IsWhiteSpace(Char16 ch)
{
    return !!iswspace(ch);
}

Char16 __cdecl _Char16_ToLower(Char16 ch)
{
    return (Char16)towlower(ch);
}

Char16 __cdecl _Char16_ToUpper(Char16 ch)
{
    return (Char16)towupper(ch);
}

#ifdef  __cplusplus
}
#endif
