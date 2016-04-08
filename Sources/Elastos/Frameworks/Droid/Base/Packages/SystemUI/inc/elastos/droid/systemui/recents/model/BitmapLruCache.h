
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_BITMAPLRUCACHE_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_BITMAPLRUCACHE_H__

#include "_SystemUI.h"
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/systemui/recents/model/KeyStoreLruCache.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::SystemUI::Recents::Model::KeyStoreLruCache;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Graphics::IBitmap)

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

/**
 * The Bitmap LRU cache.
 */
class BitmapLruCache
    : public KeyStoreLruCache<AutoPtr<IBitmap> >
{
public:
    BitmapLruCache(
        /* [in] */ Int32 cacheSize)
        : KeyStoreLruCache<AutoPtr<IBitmap> >(cacheSize)
    {
    }
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_BITMAPLRUCACHE_H__
