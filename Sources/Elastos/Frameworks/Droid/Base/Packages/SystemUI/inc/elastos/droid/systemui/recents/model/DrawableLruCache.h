
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_DRAWABLELRUCACHE_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_DRAWABLELRUCACHE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/systemui/recents/model/KeyStoreLruCache.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::SystemUI::Recents::Model::KeyStoreLruCache;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

/**
 * The Drawable LRU cache.
 */
class DrawableLruCache
    : public KeyStoreLruCache
{
public:
    DrawableLruCache(
        /* [in] */ Int32 cacheSize)
        : KeyStoreLruCache(cacheSize)
    {
    }

    using KeyStoreLruCache::Get;

    CARAPI_(AutoPtr<IDrawable>) Get(
        /* [in] */ ITaskKey* key)
    {
        AutoPtr<IInterface> value = KeyStoreLruCache::Get(key);
        return IDrawable::Probe(value);
    }

    CARAPI_(AutoPtr<IDrawable>) GetAndInvalidateIfModified(
        /* [in] */ ITaskKey* key)
    {
        AutoPtr<IInterface> value = KeyStoreLruCache::GetAndInvalidateIfModified(key);
        return IDrawable::Probe(value);
    }

    CARAPI_(void) Put(
        /* [in] */ ITaskKey* key,
        /* [in] */ IDrawable* value)
    {
        KeyStoreLruCache::Put(key, value);
    }
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_DRAWABLELRUCACHE_H__
