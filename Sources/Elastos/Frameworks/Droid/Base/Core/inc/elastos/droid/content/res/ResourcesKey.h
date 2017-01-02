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

#ifndef __ELASTOS_DROID_CONTENT_RES_RESOURCESKEY_H__
#define __ELASTOS_DROID_CONTENT_RES_RESOURCESKEY_H__

#include "Elastos.Droid.Content.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

/** @hide */
class ResourcesKey
    : public Object
    , public IResourcesKey
{
public:
    CAR_INTERFACE_DECL()

    ResourcesKey(
        /* [in] */ const String& resDir,
        /* [in] */ Int32 displayId,
        /* [in] */ IConfiguration* overrideConfiguration,
        /* [in] */ Float scale,
        /* [in] */ Boolean isThemeable,
        /* [in] */ IThemeConfig* themeConfig,
        /* [in] */ IBinder* token);

    Boolean HasOverrideConfiguration();

    CARAPI HasOverrideConfiguration(
        /* [out] */ Boolean* result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean* result);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI GetDisplayId(
        /* [out] */ Int32* displayId);

    CARAPI SetDisplayId(
        /* [in] */ Int32 displayId);

    CARAPI GetOverrideConfiguration(
        /* [out] */ IConfiguration** config);

    CARAPI SetOverrideConfiguration(
        /* [in] */ IConfiguration* config);

public:
    Int32 mDisplayId;
    AutoPtr<IConfiguration> mOverrideConfiguration;// = new Configuration();

private:
    String mResDir;
    Float mScale;
    Boolean mIsThemeable;
    Int32 mHash;
    AutoPtr<IBinder> mToken;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_RESOURCESKEY_H__
