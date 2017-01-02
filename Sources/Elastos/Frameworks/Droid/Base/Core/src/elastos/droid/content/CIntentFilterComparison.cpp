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

#include "elastos/droid/content/CIntentFilterComparison.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CIntentFilterComparison, Object, IIntentFilterComparison)

CAR_OBJECT_IMPL(CIntentFilterComparison)

CIntentFilterComparison::CIntentFilterComparison()
    : mHashCode(0)
{}

ECode CIntentFilterComparison::GetIntent(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = mIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CIntentFilterComparison::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual)
    IIntentFilterComparison* cmp = IIntentFilterComparison::Probe(obj);
    if (cmp != NULL) {
        AutoPtr<IIntent> otherIntent;
        FAIL_RETURN(cmp->GetIntent((IIntent**)&otherIntent))
        return mIntent->FilterEquals(otherIntent, isEqual);
    }
    *isEqual = FALSE;
    return NOERROR;
}

ECode CIntentFilterComparison::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = mHashCode;
    return NOERROR;
}

ECode CIntentFilterComparison::constructor(
    /* [in] */ IIntent* intent)
{
    VALIDATE_NOT_NULL(intent)
    mIntent = intent;
    FAIL_RETURN(intent->FilterHashCode(&mHashCode));
    return NOERROR;
}

}
}
}

