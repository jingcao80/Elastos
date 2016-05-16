//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CCONSTRUCTORINFO_H__
#define __CCONSTRUCTORINFO_H__

#include "CMethodInfo.h"

class CConstructorInfo : public IConstructorInfo
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI GetName(
        /* [out] */ StringBuf * pName);

    CARAPI GetParamCount(
        /* [out] */ Int32 * pCount);

    CARAPI GetAllParamInfos(
        /* [out] */ BufferOf<IParamInfo *> * pParamInfos);

    CARAPI GetParamInfoByIndex(
        /* [in] */ Int32 index,
        /* [out] */ IParamInfo ** ppParamInfo);

    CARAPI GetParamInfoByName(
        /* [in] */ CString name,
        /* [out] */ IParamInfo ** ppParamInfo);

    CARAPI CreateArgumentList(
        /* [out] */ IArgumentList ** ppArgumentList);

    CARAPI CreateObject(
        /* [in] */ IArgumentList * pArgumentList,
        /* [out] */ PInterface * pObject);

    CARAPI CreateObjectInRegime(
        /* [in] */ PRegime rgm,
        /* [in] */ IArgumentList * pArgumentList,
        /* [out] */ PInterface * pObject);

    CARAPI GetClassInfo(
        /* [out] */ IClassInfo ** ppClassInfo);

    CARAPI AcquireParamList();

    CARAPI CreateObjInRgm(
        /* [in] */ PRegime rgm,
        /* [in] */ IArgumentList * pArgumentList,
        /* [out] */ PInterface * pObject);

    CConstructorInfo():m_cRef(0){}

    CARAPI Init(
        /* [in] */ CClsModule * pCClsModule,
        /* [in] */ MethodDescriptor *pMethodDescriptor,
        /* [in] */ UInt32 uIndex,
        /* [in] */ ClassID *pClsId);

    virtual ~CConstructorInfo();

    CMethodInfo    *m_pMethodInfo;

    char m_szUrn2[_MAX_PATH];
    ClassID m_instClsId;

//private:
    Int32           m_nOutParamIndex;

    char    m_szUrn[_MAX_PATH];
    ClassID m_clsId;
    Int32     m_cRef;
};

#endif // __CCONSTRUCTORINFO_H__
