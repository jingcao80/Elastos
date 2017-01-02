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

#ifndef __ELASTOS_DROID_CONTENT_CINTENTFILTERCOMPARISON_H__
#define __ELASTOS_DROID_CONTENT_CINTENTFILTERCOMPARISON_H__

#include "_Elastos_Droid_Content_CIntentFilterComparison.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentFilterComparison)
    , public Object
    , public IIntentFilterComparison
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CIntentFilterComparison();

    /**
     * Return the Intent that this FilterComparison represents.
     * @return Returns the Intent held by the FilterComparison.  Do
     * not modify!
     */
    CARAPI GetIntent(
        /* [out] */ IIntent** intent);

    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* isEqual);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

    CARAPI constructor(
        /* [in] */ IIntent* intent);

private:
    AutoPtr<IIntent> mIntent;
    Int32 mHashCode;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CINTENTFILTERCOMPARISON_H__
