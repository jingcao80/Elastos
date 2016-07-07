
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_KEYSTORELRUCACHE_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_KEYSTORELRUCACHE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/recents/model/Task.h"
#define HASH_FOR_CORE
#include <elastos/corehash.h>
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/droid/utility/LruCache.h>

using Elastos::Droid::SystemUI::Recents::Model::ITaskKey;
using Elastos::Droid::SystemUI::Recents::Model::Task;
using Elastos::Droid::Utility::LruCache;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Utility::IHashMap;
using Elastos::Utility::CHashMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

/**
 * An LRU cache that support querying the keys as well as values. By using the Task's key, we can
 * prevent holding onto a reference to the Task resource data, while keeping the cache data in
 * memory where necessary.
 */
class KeyStoreLruCache
    : public Object
{
private:
    class MyLruCache
        : public LruCache
    {
    public:
        CARAPI constructor(
            /* [in] */ Int32 cacheSize,
            /* [in] */ KeyStoreLruCache* host);

        CARAPI EntryRemoved(
            /* [in] */ Boolean evicted,
            /* [in] */ IInterface* taskId,
            /* [in] */ IInterface* oldV,
            /* [in] */ IInterface* newV);

    private:
        KeyStoreLruCache* mHost;
    };

public:
    KeyStoreLruCache(
        /* [in] */ Int32 cacheSize);

    /** Gets a specific entry in the cache. */
    CARAPI_(AutoPtr<IInterface>) Get(
        /* [in] */ ITaskKey* key);

    /**
     * Returns the value only if the Task has not updated since the last time it was in the cache.
     */
    CARAPI_(AutoPtr<IInterface>) GetAndInvalidateIfModified(
        /* [in] */ ITaskKey* key);

    /** Puts an entry in the cache for a specific key. */
    CARAPI_(void) Put(
        /* [in] */ ITaskKey* key,
        /* [in] */ IInterface* value);

    /** Removes a cache entry for a specific key. */
    CARAPI_(void) Remove(
        /* [in] */ ITaskKey* key);

    /** Removes all the entries in the cache. */
    CARAPI_(void) EvictAll();

    /** Returns the size of the cache. */
    CARAPI_(Int32) Size();

    /** Trims the cache to a specific size */
    CARAPI_(void) TrimToSize(
        /* [in] */ Int32 cacheSize);

public:
    // We keep a set of keys that are associated with the LRU cache, so that we can find out
    // information about the Task that was previously in the cache.
    AutoPtr<IHashMap> mTaskKeys;
    // The cache implementation
    AutoPtr<ILruCache> mCache;
};

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_KEYSTORELRUCACHE_H__
