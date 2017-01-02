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

#ifndef __ELASTOS_DROID_PROVIDER_CLOCALGROUPSGROUP_H__
#define __ELASTOS_DROID_PROVIDER_CLOCALGROUPSGROUP_H__

#include "_Elastos_Droid_Provider_CLocalGroupsGroup.h"
#include "_Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CLocalGroupsGroup)
    , public Object
    , public ILocalGroupsGroup
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CLocalGroupsGroup();

    CARAPI constructor();

    CARAPI GetId(
        /* [out] */ Int64* id);

    CARAPI SetId(
        /* [in] */ Int64 id);

    CARAPI GetTitle(
        /* [out] */ String* title);

    CARAPI SetTitle(
        /* [in] */ const String& title);

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI SetCount(
        /* [in] */ Int32 count);

    CARAPI ToContentValues(
        /* [out] */ IContentValues** contentValues);

    CARAPI Save(
        /* [in] */ IContentResolver* cr,
        /* [out] */ Boolean* result);

    CARAPI Update(
        /* [in] */ IContentResolver* cr,
        /* [out] */ Boolean* result);

    CARAPI Delete(
        /* [in] */ IContentResolver* cr,
        /* [out] */ Boolean* result);

private:
    Int64 mId;
    String mTitle;
    Int32 mCount;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CLOCALGROUPSGROUP_H__
