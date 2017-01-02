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

#include "elastos/droid/systemui/keyguard/MultiUserAvatarCache.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::IInteger32;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

AutoPtr<MultiUserAvatarCache> MultiUserAvatarCache::sInstance;

MultiUserAvatarCache::MultiUserAvatarCache()
{
    CHashMap::New((IHashMap**)&mCache);
}

AutoPtr<MultiUserAvatarCache> MultiUserAvatarCache::GetInstance()
{
    if (sInstance == NULL) {
        sInstance = new MultiUserAvatarCache();
    }
    return sInstance;
}

ECode MultiUserAvatarCache::Clear(
    /* [in] */ Int32 userId)
{
    AutoPtr<IInteger32> key = CoreUtils::Convert(userId);
    return mCache->Remove(TO_IINTERFACE(key));
}

ECode MultiUserAvatarCache::Get(
    /* [in] */ Int32 userId,
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)

    AutoPtr<IInteger32> key = CoreUtils::Convert(userId);
    AutoPtr<IInterface> obj;
    mCache->Get(TO_IINTERFACE(key), (IInterface**)&obj);
    *drawable = IDrawable::Probe(obj);
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode MultiUserAvatarCache::Put(
    /* [in] */ Int32 userId,
    /* [in] */ IDrawable* image)
{
    AutoPtr<IInteger32> key = CoreUtils::Convert(userId);
    return mCache->Put(TO_IINTERFACE(key), TO_IINTERFACE(image));
}


} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
