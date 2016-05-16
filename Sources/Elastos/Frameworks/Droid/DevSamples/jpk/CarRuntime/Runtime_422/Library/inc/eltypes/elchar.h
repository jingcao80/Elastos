//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __ELACHAR_H__
#define __ELACHAR_H__

#ifdef __cplusplus
extern "C" {
#endif
    _ELASTOS Boolean __cdecl _Char8_IsASCII(_ELASTOS Char8 ch);
    _ELASTOS Boolean __cdecl _Char8_IsControl(_ELASTOS Char8 ch);
    _ELASTOS Boolean __cdecl _Char8_IsDigit(_ELASTOS Char8 ch);
    _ELASTOS Boolean __cdecl _Char8_IsLeadByte(_ELASTOS Char8 ch);
    _ELASTOS Boolean __cdecl _Char8_IsLetter(_ELASTOS Char8 ch);
    _ELASTOS Boolean __cdecl _Char8_IsLetterOrDigit(_ELASTOS Char8 ch);
    _ELASTOS Boolean __cdecl _Char8_IsLower(_ELASTOS Char8 ch);
    _ELASTOS Boolean __cdecl _Char8_IsNumber(_ELASTOS Char8 ch);
    _ELASTOS Boolean __cdecl _Char8_IsPunctuation(_ELASTOS Char8 ch);
    _ELASTOS Boolean __cdecl _Char8_IsUpper(_ELASTOS Char8 ch);
    _ELASTOS Boolean __cdecl _Char8_IsWhiteSpace(_ELASTOS Char8 ch);
    _ELASTOS Char8 __cdecl _Char8_ToLower(_ELASTOS Char8 ch);
    _ELASTOS Char8 __cdecl _Char8_ToUpper(_ELASTOS Char8 ch);

    _ELASTOS Boolean __cdecl _Char16_IsASCII(_ELASTOS Char16 ch);
    _ELASTOS Boolean __cdecl _Char16_IsControl(_ELASTOS Char16 ch);
    _ELASTOS Boolean __cdecl _Char16_IsDigit(_ELASTOS Char16 ch);
    _ELASTOS Boolean __cdecl _Char16_IsLetter(_ELASTOS Char16 ch);
    _ELASTOS Boolean __cdecl _Char16_IsLetterOrDigit(_ELASTOS Char16 ch);
    _ELASTOS Boolean __cdecl _Char16_IsLower(_ELASTOS Char16 ch);
    _ELASTOS Boolean __cdecl _Char16_IsNumber(_ELASTOS Char16 ch);
    _ELASTOS Boolean __cdecl _Char16_IsPunctuation(_ELASTOS Char16 ch);
    _ELASTOS Boolean __cdecl _Char16_IsUpper(_ELASTOS Char16 ch);
    _ELASTOS Boolean __cdecl _Char16_IsWhiteSpace(_ELASTOS Char16 ch);
    _ELASTOS Char16 __cdecl _Char16_ToLower(_ELASTOS Char16 ch);
    _ELASTOS Char16 __cdecl _Char16_ToUpper(_ELASTOS Char16 ch);
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

_ELASTOS_NAMESPACE_BEGIN

class Char
{
public:
     //---- Char8 ----
    static Boolean IsASCII(Char8 ch) {
        return _Char8_IsASCII(ch);
    }

    static Boolean IsControl(Char8 ch) {
        return _Char8_IsControl(ch);
    }

    static Boolean IsDigit(Char8 ch) {
        return _Char8_IsDigit(ch);
    }

    static Boolean IsLeadByte(Char8 ch) {
        return _Char8_IsLeadByte(ch);
    }

    static Boolean IsLetter(Char8 ch) {
        return _Char8_IsLetter(ch);
    }

    static Boolean IsLetterOrDigit(Char8 ch) {
        return _Char8_IsLetterOrDigit(ch);
    }

    static Boolean IsLower(Char8 ch) {
        return _Char8_IsLower(ch);
    }

    static Boolean IsNumber(Char8 ch) {
        return _Char8_IsNumber(ch);
    }

    static Boolean IsPunctuation(Char8 ch) {
        return _Char8_IsPunctuation(ch);
    }

    static Boolean IsUpper(Char8 ch) {
        return _Char8_IsUpper(ch);
    }

    static Boolean IsWhiteSpace(Char8 ch) {
        return _Char8_IsWhiteSpace(ch);
    }

    static Char8 ToLower(Char8 ch) {
        return _Char8_ToLower(ch);
    }

    static Char8 ToUpper(Char8 ch) {
        return _Char8_ToUpper(ch);
    }

    //---- Char16 ----
    static Boolean IsASCII(Char16 ch) {
        return _Char16_IsASCII(ch);
    }

    static Boolean IsControl(Char16 ch) {
        return _Char16_IsControl(ch);
    }

    static Boolean IsDigit(Char16 ch) {
        return _Char16_IsDigit(ch);
    }

    static Boolean IsLetter(Char16 ch) {
        return _Char16_IsLetter(ch);
    }

    static Boolean IsLetterOrDigit(Char16 ch) {
        return _Char16_IsLetterOrDigit(ch);
    }

    static Boolean IsLower(Char16 ch) {
        return _Char16_IsLower(ch);
    }

    static Boolean IsNumber(Char16 ch) {
        return _Char16_IsNumber(ch);
    }

    static Boolean IsPunctuation(Char16 ch) {
        return _Char16_IsPunctuation(ch);
    }

    static Boolean IsUpper(Char16 ch) {
        return _Char16_IsUpper(ch);
    }

    static Boolean IsWhiteSpace(Char16 ch) {
        return _Char16_IsWhiteSpace(ch);
    }

    static Char16 ToLower(Char16 ch) {
        return _Char16_ToLower(ch);
    }

    static Char16 ToUpper(Char16 ch) {
        return _Char16_ToUpper(ch);
    }
};

_ELASTOS_NAMESPACE_END

#endif //!__cplusplus

#endif //__ELACHAR_H__
