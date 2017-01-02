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

#include "elastos/droid/view/CCompatibilityInfoHolder.h"
#include "elastos/droid/content/res/CCompatibilityInfo.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Content::Res::CCompatibilityInfo;

namespace Elastos {
namespace Droid {
namespace View {

CCompatibilityInfoHolder::CCompatibilityInfoHolder()
{
    mCompatInfo = CCompatibilityInfo::DEFAULT_COMPATIBILITY_INFO;
}

ECode CCompatibilityInfoHolder::Set(
    /* [in] */ ICompatibilityInfo* compatInfo)
{
    Boolean isRequired, isSupports;
    if (compatInfo != NULL && ((compatInfo->IsScalingRequired(&isRequired), isRequired)
        || !(compatInfo->SupportsScreen(&isSupports), isSupports))) {
        mCompatInfo = compatInfo;
    }
    else {
        mCompatInfo = CCompatibilityInfo::DEFAULT_COMPATIBILITY_INFO;
    }

    return NOERROR;
}

ECode CCompatibilityInfoHolder::Get(
    /* [out] */ ICompatibilityInfo** compatInfo)
{
    VALIDATE_NOT_NULL(compatInfo);
    *compatInfo = mCompatInfo;
    REFCOUNT_ADD(*compatInfo);

    return NOERROR;
}

ECode CCompatibilityInfoHolder::GetIfNeeded(
    /* [out] */ ICompatibilityInfo** compatInfo)
{
    VALIDATE_NOT_NULL(compatInfo);
    ICompatibilityInfo* ci = mCompatInfo;
    if (mCompatInfo == NULL ||
        mCompatInfo == CCompatibilityInfo::DEFAULT_COMPATIBILITY_INFO) {
        *compatInfo = NULL;
    }
    else {
        *compatInfo = mCompatInfo;
        REFCOUNT_ADD(*compatInfo);
    }

    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
