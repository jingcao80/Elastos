
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
