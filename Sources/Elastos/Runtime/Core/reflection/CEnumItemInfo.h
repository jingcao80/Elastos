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

#ifndef __CENUMITEMINFO_H__
#define __CENUMITEMINFO_H__

#include "refutil.h"

class CEnumItemInfo : public IEnumItemInfo
{
public:
    CEnumItemInfo(
        /* [in] */ IEnumInfo* enumInfo,
        /* [in] */ const String& name,
        /* [in] */ Int32 value);

    virtual ~CEnumItemInfo();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI GetEnumInfo(
        /* [out] */ IEnumInfo** enumInfo);

    CARAPI GetValue(
        /* [out] */ Int32* value);

private:
    IEnumInfo*  mEnumInfo;
    String      mName;
    Int32       mValue;
};

#endif // __CENUMITEMINFO_H__
