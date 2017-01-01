//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __CPARAMINFO_H__
#define __CPARAMINFO_H__

#include "CClsModule.h"

class CParamInfo : public IParamInfo
{
public:
    CParamInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ IMethodInfo* methodInfo,
        /* [in] */ ParmElement* parmElement,
        /* [in] */ ParamDescriptor* paramDescriptor,
        /* [in] */ Int32 index);

    virtual ~CParamInfo();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI GetMethodInfo(
        /* [out] */ IMethodInfo** methodInfo);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetIndex(
        /* [out] */ Int32* index);

    CARAPI GetIOAttribute(
        /* [out] */ ParamIOAttribute* ioAttrib);

    CARAPI IsReturnValue(
        /* [out] */ Boolean* returnValue);

    CARAPI GetTypeInfo(
        /* [out] */ IDataTypeInfo** typeInfo);

    CARAPI GetAdvisedCapacity(
        /* [out] */ Int32* advisedCapacity);

    CARAPI IsUsingTypeAlias(
        /* [out] */ Boolean* usingTypeAlias);

    CARAPI GetUsedTypeAliasInfo(
        /* [out] */ ITypeAliasInfo** usedTypeAliasInfo);

private:
    CClsModule*         mClsModule;
    ParamDescriptor*    mParamDescriptor;
    TypeDescriptor*     mTypeDescriptor;

    MethodDescriptor*   mMethodDescriptor;

    IMethodInfo*        mMethodInfo;
    ParmElement*        mParmElement;

    Int32               mIndex;
    UInt32              mPointer;
};

#endif // __CPARAMINFO_H__
