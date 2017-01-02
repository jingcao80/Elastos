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

#ifndef __ELASTOS_DROID_CONTENT_CCONTENTRESOLVEROPENRESOURCEIDRESULT_H__
#define __ELASTOS_DROID_CONTENT_CCONTENTRESOLVEROPENRESOURCEIDRESULT_H__

#include "_Elastos_Droid_Content_CContentResolverOpenResourceIdResult.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::Res::IResources;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CContentResolverOpenResourceIdResult)
    , public Object
    , public IContentResolverOpenResourceIdResult
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CContentResolverOpenResourceIdResult();

    CARAPI constructor(
        /* [in] */ IResources* res,
        /* [in] */ Int32 id);

    CARAPI GetResources(
        /* [out] */ IResources** resources);

    CARAPI SetResources(
        /* [in] */ IResources* resources);

    CARAPI GetResourceId(
        /* [out] */ Int32* id);

    CARAPI SetResourceId(
        /* [in] */ Int32 id);

private:
    AutoPtr<IResources> mRes;
    Int32 mId;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCONTENTRESOLVEROPENRESOURCEIDRESULT_H__
