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

#ifndef __CANNOTATIONINFO_H__
#define __CANNOTATIONINFO_H__

#include "CEntryList.h"

class CAnnotationInfo
    : public ElLightRefBase
    , public IAnnotationInfo
{
public:
    CAnnotationInfo(
        /* [in] */ CClsModule* clsModule,
        /* [in] */ AnnotationDescriptor* annoDesc);

    virtual ~CAnnotationInfo();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetNamespace(
        /* [out] */ String* ns);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI GetKeys(
        /* [out, callee] */ ArrayOf<String>** keys);

    CARAPI GetValue(
        /* [in] */ const String& key,
        /* [out] */ String* value);

    CARAPI GetValues(
        /* [out, callee] */ ArrayOf<String>** values);

private:
    CARAPI_(void) AcquireKeys();

    CARAPI_(void) AcquireValues();

private:
    AnnotationDescriptor* mAnnotationDescriptor;
    AutoPtr< ArrayOf<String> > mKeys;
    AutoPtr< ArrayOf<String> > mValues;

    Int32 mBase;
};

#endif // __CANNOTATIONINFO_H__
