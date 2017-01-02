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

#ifndef __ELASTOS_DROID_CONTENT_RES_CCOLORSTATELISTHELPER_H__
#define __ELASTOS_DROID_CONTENT_RES_CCOLORSTATELISTHELPER_H__

#include "_Elastos_Droid_Content_Res_CColorStateListHelper.h"
#include "elastos/droid/content/res/CColorStateList.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Content::Res::IResources;
using Org::Xmlpull::V1::IXmlPullParser;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CarClass(CColorStateListHelper)
    , public Singleton
    , public IColorStateListHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI ValueOf(
        /* [in] */ Int32 color,
        /* [out] */ IColorStateList** ppList);

    CARAPI CreateFromXml(
        /* [in] */ IResources* pR,
        /* [in] */ IXmlPullParser* pParser,
        /* [out] */ IColorStateList** ppList);

};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CCOLORSTATELISTHELPER_H__
