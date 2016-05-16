//==========================================================================
// Copyright (c) 2000-2006,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <eltypes.h>

#ifdef  __cplusplus
extern "C" {
#endif

_ELASTOS_NAMESPACE_USING

#define IS_DELIM() \
    while (*pdelim != '\0') { \
        if (ch == *pdelim) { \
            return TRUE; \
        } \
        pdelim++; \
    } \
    return FALSE;

CAR_INLINE Boolean IsDelimW(wchar_t ch, const wchar_t *pdelim)
{
    IS_DELIM();
}

CAR_INLINE Boolean IsDelimA(char ch, const char *pdelim)
{
    IS_DELIM();
}

CAR_INLINE const wchar_t * ScanTokenW(const wchar_t *startPos,
            const wchar_t *delim, Boolean retDelims)
{
    const wchar_t *pos = startPos;
    while (*pos != L'\0') {
        if (IsDelimW(*pos, delim)) break;
        pos++;
    }
    if (retDelims && (pos == startPos) && IsDelimW(*pos, delim)) {
        pos++;
    }
    return pos;
}

CAR_INLINE const char * ScanTokenA(const char *startPos, const char *delim,
            Boolean retDelims)
{
    const char *pos = startPos;
    while (*pos != '\0') {
        if (IsDelimA(*pos, delim)) break;
        pos++;
    }
    if (retDelims && (pos == startPos) && IsDelimA(*pos, delim)) {
        pos++;
    }
    return pos;
}

CAR_INLINE const wchar_t * SkipDelimitersW(const wchar_t *startPos,
    const wchar_t *delim, Boolean retDelims)
{
    const wchar_t *pos = startPos;
    while (!retDelims && *pos != L'\0') {
        if (!IsDelimW(*pos, delim)) break;
        pos++;
    }
    return pos;
}

CAR_INLINE const char * SkipDelimitersA(const char *startPos,
    const char *delim, Boolean retDelims)
{
    const char *pos = startPos;
    while (!retDelims && *pos != '\0') {
        if (!IsDelimA(*pos, delim)) break;
        pos++;
    }
    return pos;
}

Int32 __cdecl _StringTokenizer_GetCount(StringTokenizer *pTokenizer)
{
    if (!pTokenizer->m_curPos || !pTokenizer->m_delimiters) {
        return -1;
    }

    Int32 count = 0;
	const char *pos = pTokenizer->m_curPos;
	while (*pos != '\0') {
	    pos = SkipDelimitersA(pos, pTokenizer->m_delimiters, pTokenizer->m_retDelims);
	    if (*pos == '\0') break;
	    pos = ScanTokenA(pos, pTokenizer->m_delimiters, pTokenizer->m_retDelims);
	    count++;
	}

	return count;
}

Boolean __cdecl _StringTokenizer_HasMoreTokens(StringTokenizer *pTokenizer)
{
    if (pTokenizer->m_delimiters == NULL) return FALSE;

	pTokenizer->m_newPos = SkipDelimitersA(pTokenizer->m_curPos,
	        pTokenizer->m_delimiters, pTokenizer->m_retDelims);
	return (*pTokenizer->m_newPos != '\0');
}

char* __cdecl _StringTokenizer_NextToken(StringTokenizer *pTokenizer)
{
	if (pTokenizer->m_delimiters == NULL) return NULL;

	pTokenizer->m_curPos = (pTokenizer->m_newPos && !pTokenizer->m_delimsChanged)
	    ? pTokenizer->m_newPos
	    : SkipDelimitersA(pTokenizer->m_curPos, pTokenizer->m_delimiters,
	            pTokenizer->m_retDelims);

	pTokenizer->m_delimsChanged = FALSE;
	pTokenizer->m_newPos = NULL;

	if (*pTokenizer->m_curPos == '\0') return NULL;

	const char *pos = ScanTokenA(pTokenizer->m_curPos, pTokenizer->m_delimiters, pTokenizer->m_retDelims);
    Int32 size = pos - pTokenizer->m_curPos;
    char *pBuf = pTokenizer->m_tokenBuf;
    if (size >= TOKEN_HEAP_ALLOC_THRESHOLD) {
        if (pTokenizer->m_pBuf && size > pTokenizer->m_bufSize) {
            free(pTokenizer->m_pBuf);
            pTokenizer->m_bufSize = size;
            pTokenizer->m_pBuf = NULL;
        }

        if (!pTokenizer->m_pBuf) {
            pTokenizer->m_bufSize = size;
            pTokenizer->m_pBuf = (char *)malloc((pTokenizer->m_bufSize + 1) * sizeof(char));
            if (!pTokenizer->m_pBuf) return NULL;
        }
        pBuf = pTokenizer->m_pBuf;
    }

    memcpy(pBuf, pTokenizer->m_curPos, size * sizeof(char));
    pBuf[size] = '\0';

    pTokenizer->m_curPos = pos;
    return pBuf;
}

#ifdef  __cplusplus
}
#endif
