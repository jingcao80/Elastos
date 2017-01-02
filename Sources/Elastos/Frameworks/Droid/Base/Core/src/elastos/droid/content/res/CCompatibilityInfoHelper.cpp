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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/res/CCompatibilityInfoHelper.h"
#include "elastos/droid/content/res/CCompatibilityInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CCompatibilityInfoHelper, Singleton, ICompatibilityInfoHelper)

CAR_SINGLETON_IMPL(CCompatibilityInfoHelper)

ECode CCompatibilityInfoHelper::ComputeCompatibleScaling(
    /* [in] */ IDisplayMetrics* dm,
    /* [in] */ IDisplayMetrics* outDm,
    /* [out] */ Float* scale)
{
    VALIDATE_NOT_NULL(scale);
    *scale = CCompatibilityInfo::ComputeCompatibleScaling(dm, outDm);
    return NOERROR;
}

ECode CCompatibilityInfoHelper::GetDefault(
    /* [out] */ ICompatibilityInfo** info)
{
    VALIDATE_NOT_NULL(info);
    *info = CCompatibilityInfo::DEFAULT_COMPATIBILITY_INFO;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

