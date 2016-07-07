
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
    : public KeyStoreLruCache
{
public:
    StringLruCache(
        /* [in] */ Int32 cacheSize)
        : KeyStoreLruCache(cacheSize)
    {
    }

    using KeyStoreLruCache::Get;

    CARAPI_(String) Get(
        /* [in] */ ITaskKey* key)
    {
        AutoPtr<IInterface> value = KeyStoreLruCache::Get(key);
        return Object::ToString(value);
    }

    CARAPI_(String) GetAndInvalidateIfModified(
        /* [in] */ ITaskKey* key)
    {
        AutoPtr<IInterface> value = KeyStoreLruCache::GetAndInvalidateIfModified(key);
        return Object::ToString(value);
    }

    CARAPI_(void) Put(
        /* [in] */ ITaskKey* key,
        /* [in] */ const String& value)
    {
        AutoPtr<ICharSequence> valueObj = CoreUtils::Convert(value);
        KeyStoreLruCache::Put(key, valueObj);
    }
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_STRINGLRUCACHE_H__
