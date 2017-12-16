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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/res/ResourcesKey.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>

using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

CAR_INTERFACE_IMPL(ResourcesKey, Object, IResourcesKey)

#ifdef __clang__
__attribute__((no_sanitize("integer")))
#endif
ResourcesKey::ResourcesKey(
    /* [in] */ const String& resDir,
    /* [in] */ Int32 displayId,
    /* [in] */ IConfiguration* overrideConfiguration,
    /* [in] */ Float scale,
    /* [in] */ Boolean isThemeable,
    /* [in] */ IThemeConfig* themeConfig,
    /* [in] */ IBinder* token)
{
    CConfiguration::New((IConfiguration**)&mOverrideConfiguration);
    mResDir = resDir;
    mDisplayId = displayId;
    if (overrideConfiguration != NULL) {
        mOverrideConfiguration->SetTo(overrideConfiguration);
    }
    mScale = scale;
    mIsThemeable = isThemeable;
    mToken = token;

    Int32 hash = 17;
    hash = 31 * hash + (mResDir.IsNull() ? 0 : mResDir.GetHashCode());
    hash = 31 * hash + mDisplayId;
    hash = 31 * hash + Object::GetHashCode(mOverrideConfiguration);
    hash = 31 * hash + Elastos::Core::Math::FloatToInt32Bits(mScale);
    hash = 31 * hash + (mIsThemeable ? 1 : 0);
    Int32 configHash = Object::GetHashCode(themeConfig);
    hash = 31 * hash + configHash;
    mHash = hash;
}

Boolean ResourcesKey::HasOverrideConfiguration()
{
    return !Object::Equals(CConfiguration::EMPTY, mOverrideConfiguration);
}

ECode ResourcesKey::HasOverrideConfiguration(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = HasOverrideConfiguration();
    return NOERROR;
}

ECode ResourcesKey::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash)
    *hash = mHash;
    return NOERROR;
}

ECode ResourcesKey::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (IResourcesKey::Probe(other) == NULL) {
        return NOERROR;
    }

    ResourcesKey* peer = (ResourcesKey*) IResourcesKey::Probe(other);
    if (mResDir != peer->mResDir) {
        if (mResDir.IsNull() || peer->mResDir.IsNull()) {
            return NOERROR;
        }
        else if (!mResDir.Equals(peer->mResDir)) {
            return NOERROR;
        }
    }
    if (mDisplayId != peer->mDisplayId) {
        return NOERROR;
    }
    if (mOverrideConfiguration != peer->mOverrideConfiguration) {
        if (mOverrideConfiguration == NULL || peer->mOverrideConfiguration == NULL) {
            return NOERROR;
        }
        if (!Object::Equals(mOverrideConfiguration, peer->mOverrideConfiguration)) {
            return NOERROR;
        }
    }
    if (mScale != peer->mScale) {
        return NOERROR;
    }

    *result = mIsThemeable == peer->mIsThemeable;;
    return NOERROR;
}

ECode ResourcesKey::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    *str = StringUtils::ToString(mHash);
    return NOERROR;
}

ECode ResourcesKey::GetDisplayId(
    /* [out] */ Int32* displayId)
{
    VALIDATE_NOT_NULL(displayId)
    *displayId = mDisplayId;
    return NOERROR;
}

ECode ResourcesKey::SetDisplayId(
    /* [in] */ Int32 displayId)
{
    mDisplayId = displayId;
    return NOERROR;
}

ECode ResourcesKey::GetOverrideConfiguration(
    /* [out] */ IConfiguration** config)
{
    VALIDATE_NOT_NULL(config)
    *config = mOverrideConfiguration;
    REFCOUNT_ADD(*config)
    return NOERROR;
}

ECode ResourcesKey::SetOverrideConfiguration(
    /* [in] */ IConfiguration* config)
{
    mOverrideConfiguration = config;
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos