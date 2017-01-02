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

#ifndef __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERRESULT_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERRESULT_H__

#include "_Elastos_Droid_Content_CContentProviderResult.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CContentProviderResult)
    , public Object
    , public IContentProviderResult
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CContentProviderResult();

    virtual ~CContentProviderResult();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IUri* uri);

    CARAPI constructor(
        /* [in] */ Int32 count);

    CARAPI constructor(
        /* [in] */ IContentProviderResult* cpr,
        /* [in] */ Int32 userId);

    CARAPI GetUri(
        /* [out] */ IUri** uri);

    CARAPI GetCount(
        /* [out] */ Int32* count);

    CARAPI SetUri(
        /* [in] */ IUri* uri);

    CARAPI SetCount(
        /* [in] */ Int32 count);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);
private:
    AutoPtr<IUri> mUri;
    Int32 mCount;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCONTENTPROVIDERRESULT_H__
