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

#ifndef __CINTRINSICINFO_H__
#define __CINTRINSICINFO_H__

#include <elastos.h>
#include <clsdef.h>

_ELASTOS_NAMESPACE_USING

class CIntrinsicInfo
    : public ElLightRefBase
    , public IDataTypeInfo
{
public:
    CIntrinsicInfo(
        /* [in] */ CarDataType dataType,
        /* [in] */ UInt32 uSize);

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetSize(
        /* [out] */ MemorySize* size);

    CARAPI GetDataType(
        /* [out] */ CarDataType* dataType);

private:
    CarDataType mDataType;
    UInt32      mSize;
};

#endif // __CINTRINSICINFO_H__
