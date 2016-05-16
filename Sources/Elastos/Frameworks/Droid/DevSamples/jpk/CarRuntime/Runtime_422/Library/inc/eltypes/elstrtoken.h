//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if !defined(__EZSTRTOKEN_H__)
#define __EZSTRTOKEN_H__

#include <elrefbase.h>

#define TOKEN_HEAP_ALLOC_THRESHOLD 256

#ifdef __cplusplus

class StringTokenizer;

extern "C" {
    _ELASTOS Boolean __cdecl _StringTokenizer_HasMoreTokens(
        StringTokenizer *pTokenizer);
    _ELASTOS Int32 __cdecl _StringTokenizer_GetCount(
        StringTokenizer *pTokenizer);
    char* __cdecl _StringTokenizer_NextToken(StringTokenizer *pTokenizer);
}

/** @addtogroup BaseTypesRef
  *   @{
  */
class StringTokenizer : public Elastos::ElLightRefBase
{
friend _ELASTOS Boolean __cdecl _StringTokenizer_HasMoreTokens(
        StringTokenizer *pTokenizer);
friend _ELASTOS Int32 __cdecl _StringTokenizer_GetCount(
        StringTokenizer *pTokenizer);
friend char* __cdecl _StringTokenizer_NextToken(
        StringTokenizer *pTokenizer);

public:
    StringTokenizer(const char* str, const char* delim = " \t\n\r\f",
        _ELASTOS Boolean returnDelims = FALSE)
    {
        m_curPos = str;
        assert(m_curPos);
        m_delimiters = delim;
        m_retDelims = returnDelims;
        m_delimsChanged = TRUE;
        m_newPos = NULL;
        m_pBuf = NULL;
        m_bufSize = TOKEN_HEAP_ALLOC_THRESHOLD * 2 - 1;
    }

    _ELASTOS Int32 GetCount()
    {
        return _StringTokenizer_GetCount(this);
    }

    _ELASTOS Boolean HasMoreTokens()
    {
        if (!m_curPos || *m_curPos == '\0') return FALSE;
        return _StringTokenizer_HasMoreTokens(this);
    }

    _ELASTOS CString NextToken()
    {
        if (!m_curPos || *m_curPos == '\0') return _ELASTOS String(NULL);
        return _StringTokenizer_NextToken(this);
    }

    _ELASTOS CString NextToken(_ELASTOS String delim)
    {
        m_delimiters = delim;
        m_delimsChanged = TRUE;
        return NextToken();
    }

    StringTokenizer& operator>>(_ELASTOS StringBuf& sb)
    {
        sb.Copy(NextToken());
        return *this;
    }

    StringTokenizer& operator>>(_ELASTOS Boolean& boolean)
    {
        boolean = NextToken().ToBoolean();
        return *this;
    }

    StringTokenizer& operator>>(_ELASTOS Int32& value)
    {
        value = NextToken().ToInt32();
        return *this;
    }

    StringTokenizer& operator>>(_ELASTOS Int64& value)
    {
        value = NextToken().ToInt64();
        return *this;
    }

    StringTokenizer& operator>>(_ELASTOS Double& value)
    {
        value = NextToken().ToDouble();
        return *this;
    }

    ~StringTokenizer()
    {
        if (m_pBuf) free(m_tokenBuf);
    }

private:
    StringTokenizer() {}

    const char *m_curPos;
    const char *m_newPos;
    const char *m_delimiters;
    char *m_pBuf;
    _ELASTOS Int32   m_bufSize;
    _ELASTOS Boolean m_retDelims;
    _ELASTOS Boolean m_delimsChanged;
    char m_tokenBuf[TOKEN_HEAP_ALLOC_THRESHOLD];
};
/** @} */

#else //!__cplusplus

typedef const _ELASTOS PVoid StringTokenizer;

#endif //__cplusplus

#endif //__EZSTRTOKEN_H__
