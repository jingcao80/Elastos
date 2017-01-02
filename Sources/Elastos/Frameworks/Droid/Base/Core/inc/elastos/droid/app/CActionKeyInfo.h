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

#ifndef __ELASTOS_DROID_APP_CACTIONKEYINFO_H__
#define __ELASTOS_DROID_APP_CACTIONKEYINFO_H__

#include "_Elastos_Droid_App_CActionKeyInfo.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CActionKeyInfo)
    , public Object
    , public IActionKeyInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CActionKeyInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI GetKeyCode(
        /* [out] */ Int32* keyCode);

    CARAPI GetQueryActionMsg(
        /* [out] */ String* actionMsg);

    CARAPI GetSuggestActionMsg(
        /* [out] */ String* actionMsg);

    CARAPI GetSuggestActionMsgColumn(
        /* [out] */ String* column);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel *source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel *dest);

private:
    Int32 mKeyCode;
    String mQueryActionMsg;
    String mSuggestActionMsg;
    String mSuggestActionMsgColumn;
};

}// namespace App
}// namespace Droid
}// namespace Elastos

#endif
