
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_DRAWABLELRUCACHE_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_DRAWABLELRUCACHE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/systemui/recents/model/KeyStoreLruCache.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::SystemUI::Recents::Model::KeyStoreLruCache;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Graphics::Drawable::IDrawable)

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

/**
 * The Drawable LRU cache.
 */
class DrawableLruCache
    : public KeyStoreLruCache<AutoPtr<IDrawable> >
{
public:
    DrawableLruCache(
        /* [in] */ Int32 cacheSize)
        : KeyStoreLruCache<AutoPtr<IDrawable> >(cacheSize)
    {
    }
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_DRAWABLELRUCACHE_H__
