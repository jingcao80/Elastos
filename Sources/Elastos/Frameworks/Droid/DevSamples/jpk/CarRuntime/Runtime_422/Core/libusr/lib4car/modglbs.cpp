//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#include <elapi.h>
#include <elastos.h>
_ELASTOS_NAMESPACE_USING

class ModuleGlobals
{
public:
    ModuleGlobals()
    {
        m_value = NULL;
        m_pMem = NULL;
        m_pObj = NULL;
    }

    ~ModuleGlobals()
    {
        if (NULL != m_pMem) free(m_pMem);
        if (NULL != m_pObj) m_pObj->Release();
    }

public:
    PVoid   m_value;
    PVoid   m_pMem;
    PInterface m_pObj;
    PVoid   m_reserved;
};

#if defined(_GNUC)

DECL_SECTION(".modglbs")
STATIC ModuleGlobals s_moduleGlobals;
#elif defined(_EVC) || defined(_MSVC)

#pragma data_seg(".modglbs")
__declspec(allocate(".modglbs"))
STATIC ModuleGlobals s_moduleGlobals;

#else

#(Unknown the C++ compiler)

#endif

ELAPI EzSetModuleGlobalValue(
    /* [in] */ PVoid value)
{
    s_moduleGlobals.m_value = value;
    return NOERROR;
}

ELAPI EzSetModuleGlobalMem(
    /* [in] */ PVoid pMem)
{
    if (NULL != s_moduleGlobals.m_pMem) free(s_moduleGlobals.m_pMem);
    s_moduleGlobals.m_pMem = pMem;
    return NOERROR;
}

ELAPI EzSetModuleGlobalObj(
    /* [in] */ PInterface pObj)
{
    if (NULL != s_moduleGlobals.m_pObj) s_moduleGlobals.m_pObj->Release();
    s_moduleGlobals.m_pObj = pObj;
    return NOERROR;
}

ELAPI EzGetModuleGlobalValue(
    /* [out] */ PVoid *pValue)
{
    *pValue = s_moduleGlobals.m_value;
    return NOERROR;
}

ELAPI EzGetModuleGlobalMem(
    /* [out] */ PVoid *ppMem)
{
    *ppMem = s_moduleGlobals.m_pMem;
    return NOERROR;
}

ELAPI EzGetModuleGlobalObj(
    /* [out] */ PInterface *ppObj)
{
    *ppObj = s_moduleGlobals.m_pObj;
    if (NULL != *ppObj) (*ppObj)->AddRef();
    return NOERROR;
}
