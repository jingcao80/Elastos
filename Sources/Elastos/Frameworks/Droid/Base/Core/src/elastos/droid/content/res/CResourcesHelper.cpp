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

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/content/res/CResourcesHelper.h"
#include "elastos/droid/content/res/CResources.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(CResourcesHelper, Singleton, IResourcesHelper)

CAR_SINGLETON_IMPL(CResourcesHelper)

ECode CResourcesHelper::SelectDefaultTheme(
    /* [in] */ Int32 curTheme,
    /* [in] */ Int32 targetSdkVersion,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CResources::SelectDefaultTheme(curTheme, targetSdkVersion);
    return NOERROR;
}

ECode CResourcesHelper::SelectSystemTheme(
    /* [in] */ Int32 curTheme,
    /* [in] */ Int32 targetSdkVersion,
    /* [in] */ Int32 orig,
    /* [in] */ Int32 holo,
    /* [in] */ Int32 dark,
    /* [in] */ Int32 deviceDefault,
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = CResources::SelectSystemTheme(curTheme, targetSdkVersion, orig, holo, dark, deviceDefault);
    return NOERROR;
}

ECode CResourcesHelper::GetSystem(
    /* [out] */ IResources** res)
{
    VALIDATE_NOT_NULL(res);
    AutoPtr<IResources> r = CResources::GetSystem();
    *res = r;
    REFCOUNT_ADD(*res);
    return NOERROR;
}

ECode CResourcesHelper::UpdateSystemConfiguration(
    /* [in] */ IConfiguration* config,
    /* [in] */ IDisplayMetrics* metrics,
    /* [in] */ ICompatibilityInfo* compat)
{
    CResources::UpdateSystemConfiguration(config, metrics, compat);
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos
