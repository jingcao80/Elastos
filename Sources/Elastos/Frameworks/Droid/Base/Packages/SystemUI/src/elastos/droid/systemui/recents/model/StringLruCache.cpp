#include "elastos/droid/systemui/recents/model/StringLruCache.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

StringLruCache::StringLruCache(
    /* [in] */ Int32 cacheSize)
    : KeyStoreLruCache<String>(cacheSize)
{
}

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos