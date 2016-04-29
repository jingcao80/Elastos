
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_STRINGLRUCACHE_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_STRINGLRUCACHE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/recents/model/KeyStoreLruCache.h"

using Elastos::Droid::SystemUI::Recents::Model::KeyStoreLruCache;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

/**
 * The String LRU cache.
 */
class StringLruCache
    : public KeyStoreLruCache<String>
{
public:
    StringLruCache(
        /* [in] */ Int32 cacheSize)
        : KeyStoreLruCache<String>(cacheSize)
    {
    }
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_STRINGLRUCACHE_H__
