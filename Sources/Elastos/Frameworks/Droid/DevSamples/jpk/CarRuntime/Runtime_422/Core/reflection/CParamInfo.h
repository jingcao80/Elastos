//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CPARAMINFO_H__
#define __CPARAMINFO_H__

#include "CClsModule.h"

class CParamInfo : public IParamInfo
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI GetMethodInfo(
        /* [out] */ IMethodInfo ** ppMethodInfo);

    CARAPI GetName(
        /* [out] */ StringBuf * pName);

    CARAPI GetIndex(
        /* [out] */ Int32 * pIndex);

    CARAPI GetIOAttribute(
        /* [out] */ ParamIOAttribute * pIOAttrib);

    CARAPI IsReturnValue(
        /* [out] */ Boolean * pReturnValue);

    CARAPI GetTypeInfo(
        /* [out] */ IDataTypeInfo ** ppTypeInfo);

    CARAPI GetAdvisedCapacity(
        /* [out] */ Int32 * pAdvisedCapacity);

    CARAPI IsUsingTypeAlias(
        /* [out] */ Boolean * pUsingTypeAlias);

    CARAPI GetUsedTypeAliasInfo(
        /* [out] */ ITypeAliasInfo ** ppUsedTypeAliasInfo);

    CParamInfo(
        /* [in] */ CClsModule *pCClsModule,
        /* [in] */ IMethodInfo *pMethodInfo,
        /* [in] */ ParmElement *pParmElement,
        /* [in] */ ParamDescriptor *pParamDescriptor,
        /* [in] */ Int32 iIndex);

    virtual ~CParamInfo();

private:
    CClsModule      *m_pCClsModule;
    ParamDescriptor *m_pParamDescriptor;
    TypeDescriptor  *m_pTypeDescriptor;

    MethodDescriptor   *m_pMethodDescriptor;

    IMethodInfo     *m_pMethodInfo;
    ParmElement     *m_pParmElement;

    Int32              m_iIndex;
    UInt32              m_uPointer;

    Int32  m_cRef;
};

#endif // __CPARAMINFO_H__
