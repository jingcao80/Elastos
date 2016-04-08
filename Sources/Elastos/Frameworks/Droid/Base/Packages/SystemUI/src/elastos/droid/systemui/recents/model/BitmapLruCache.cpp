#include "elastos/droid/systemui/recents/model/BitmapLruCache.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

BitmapLruCache::BitmapLruCache(
    /* [in] */ Int32 cacheSize)
    : KeyStoreLruCache<IBitmap*>(cacheSize)
{
}

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos