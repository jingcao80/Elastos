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

#ifndef __ELASTOS_DROID_CONTENT_RES_CRESOURCESHELPER_H__
#define __ELASTOS_DROID_CONTENT_RES_CRESOURCESHELPER_H__

#include "_Elastos_Droid_Content_Res_CResourcesHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CarClass(CResourcesHelper)
    , public Singleton
    , public IResourcesHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI SelectDefaultTheme(
        /* [in] */ Int32 curTheme,
        /* [in] */ Int32 targetSdkVersion,
        /* [out] */ Int32* value);

    CARAPI SelectSystemTheme(
        /* [in] */ Int32 curTheme,
        /* [in] */ Int32 targetSdkVersion,
        /* [in] */ Int32 orig,
        /* [in] */ Int32 holo,
        /* [in] */ Int32 dark,
        /* [in] */ Int32 deviceDefault,
        /* [out] */ Int32* value);

    CARAPI GetSystem(
        /* [out] */ IResources** res);

    CARAPI UpdateSystemConfiguration(
        /* [in] */ IConfiguration* config,
        /* [in] */ IDisplayMetrics* metrics,
        /* [in] */ ICompatibilityInfo* compat);
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTENT_RES_CRESOURCESHELPER_H__
