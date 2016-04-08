#include "elastos/droid/systemui/recents/model/DrawableLruCache.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

DrawableLruCache::DrawableLruCache(
    /* [in] */ Int32 cacheSize)
    : KeyStoreLruCache<IDrawable*>(cacheSize)
{
}

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos