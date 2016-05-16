//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <stdio.h>
#include "AObjectEx.h"
#include "_AObjectEx.cpp"

extern "C" _CBaseClassObject _g_AObjectEx_ClsObj;
_CBaseClassObject _g_AObjectEx_ClsObj(_AObjectExCreateObject);

#include <wchar.h>

ECode AObjectEx::Equals(
    /* [in] */ IInterface *pObject)
{
	if (NULL == _m_pOuter) return E_CLASS_NO_AGGREGATION;

    pObject = pObject->Probe(EIID_IInterface);
    if (pObject) {
        return pObject == _m_pOuter ? NOERROR : S_FALSE;
    }
    return S_FALSE;
}

ECode AObjectEx::GetHashCode(
    /* [out] */ int *pHashCode)
{
    if (NULL == _m_pOuter) return E_CLASS_NO_AGGREGATION;
    *pHashCode = m_nHashCode;
    return NOERROR;
}

ECode AObjectEx::GetClassID(
    /* [out] */ ClassID *pClsid)
{
    if (NULL == _m_pOuter) return E_CLASS_NO_AGGREGATION;
    *pClsid = m_clsid;
    return NOERROR;
}

void Addr2WString(Address addr, wchar_t *pwszBuf)
{
    int n;
    wchar_t v;

    for (n = 0; n < 8; n++) {
        if (addr & 0xf0000000) break;
        addr <<= 4;
    }

    while (n < 8) {
        v = (wchar_t)((addr & 0xf0000000) >> 28);
        v = (v < 10 ? (v + '0') : (v - 10 + 'A'));
        *pwszBuf = v;
        addr <<= 4;
        pwszBuf++;
        n++;
    }
    *pwszBuf = 0;
}

ECode AObjectEx::ToString(
    /* [out] */ StringBuf * pString)
{
    ECode ec;
    wchar_t wszName[_MAX_SEED_LEN];

    if (NULL == _m_pOuter) return E_CLASS_NO_AGGREGATION;

    if (pString == NULL) {
        return E_INVALID_ARGUMENT;
    }
    if (!m_pString) {
        pString->Copy(m_pString->GetPayload());
        return NOERROR;
    }
    ec = _DebugHelper_NameFromGuid(m_clsid.clsid, wszName);
    if (FAILED(ec)) wszName[0] = 0;
    Addr2WString((Address)_m_pOuter, wszName + wcslen(wszName));

    m_pString = StringBuf::Alloc(wcslen(wszName) * sizeof(wchar_t) + 1);
    if (!m_pString) m_pString->FromUnicode((Char16 *)wszName);
    pString->FromUnicode((Char16 *)wszName);
    return NOERROR;
}

ECode AObjectEx::OnAggregated(PInterface pOuter)
{
    IObject* pObject;
    ECode ec;

    pObject = (IObject*)pOuter->Probe(EIID_IObject);
    if (NULL == pObject) return E_NO_INTERFACE;
    _m_pOuter = pObject;

    ec = pObject->GetClassID(&m_clsid);
    if (FAILED(ec)) return E_NO_INTERFACE;

    m_nHashCode = (int)_m_pOuter;
    while (0 == (m_nHashCode & 0x03)) m_nHashCode >>= 2;

    return NOERROR;
}

ECode AObjectEx::OnUnaggregated(PInterface pOuter)
{
    _m_pOuter = NULL;
    if (!m_pString) StringBuf::Free(m_pString);
    return NOERROR;
}
