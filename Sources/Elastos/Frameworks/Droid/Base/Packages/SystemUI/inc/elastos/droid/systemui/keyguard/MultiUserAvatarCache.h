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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_MULTIUSERAVATARCACHE_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_MULTIUSERAVATARCACHE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Graphics.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Core::Object;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

class MultiUserAvatarCache
    : public Object
{
public:
    TO_STRING_IMPL("MultiUserAvatarCache")

    static CARAPI_(AutoPtr<MultiUserAvatarCache>) GetInstance();

    CARAPI Clear(
        /* [in] */ Int32 userId);

    CARAPI Get(
        /* [in] */ Int32 userId,
        /* [out] */ IDrawable** drawable);

    CARAPI Put(
        /* [in] */ Int32 userId,
        /* [in] */ IDrawable* image);

private:
    MultiUserAvatarCache();

private:
    static AutoPtr<MultiUserAvatarCache> sInstance;

    AutoPtr<IHashMap> mCache;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_MULTIUSERAVATARCACHE_H__