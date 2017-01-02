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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_BITMAPLRUCACHE_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_BITMAPLRUCACHE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/systemui/recents/model/KeyStoreLruCache.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::SystemUI::Recents::Model::KeyStoreLruCache;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

/**
 * The Bitmap LRU cache.
 */
class BitmapLruCache
    : public KeyStoreLruCache
{
public:
    BitmapLruCache(
        /* [in] */ Int32 cacheSize)
        : KeyStoreLruCache(cacheSize)
    {
    }

    using KeyStoreLruCache::Get;

    CARAPI_(AutoPtr<IBitmap>) Get(
        /* [in] */ ITaskKey* key)
    {
        AutoPtr<IInterface> value = KeyStoreLruCache::Get(key);
        return IBitmap::Probe(value);
    }

    CARAPI_(AutoPtr<IBitmap>) GetAndInvalidateIfModified(
        /* [in] */ ITaskKey* key)
    {
        AutoPtr<IInterface> value = KeyStoreLruCache::GetAndInvalidateIfModified(key);
        return IBitmap::Probe(value);
    }

    CARAPI_(void) Put(
        /* [in] */ ITaskKey* key,
        /* [in] */ IBitmap* value)
    {
        KeyStoreLruCache::Put(key, value);
    }
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_BITMAPLRUCACHE_H__
