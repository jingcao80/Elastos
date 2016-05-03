
#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_KEYSTORELRUCACHE_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_KEYSTORELRUCACHE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/recents/model/Task.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/utility/LruCache.h"

using Elastos::Droid::SystemUI::Recents::Model::ITaskKey;
using Elastos::Droid::SystemUI::Recents::Model::Task;
using Elastos::Droid::Utility::LruCache;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Utility::IHashMap;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Core::IInteger32)

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
template<typename V>
class KeyStoreLruCache
    : public Object
{
private:
    class MyLruCache
        : public LruCache<AutoPtr<IInteger32>, V>
    {
    public:
        MyLruCache(
            /* [in] */ Int32 cacheSize,
            /* [in] */ KeyStoreLruCache<V>* host)
            : LruCache<AutoPtr<IInteger32>, V>(cacheSize)
            , mHost(host)
        {
        }

        CARAPI_(void) EntryRemoved(
            /* [in] */ Boolean evicted,
            /* [in] */ AutoPtr<IInteger32> taskId,
            /* [in] */ V oldV,
            /* [in] */ V newV)
        {
            mHost->mTaskKeys->Remove(taskId);
        }

    private:
        KeyStoreLruCache<V>* mHost;
    };

public:
    KeyStoreLruCache(
        /* [in] */ Int32 cacheSize);

    /** Gets a specific entry in the cache. */
    CARAPI_(V) Get(
        /* [in] */ ITaskKey* key);

    /**
     * Returns the value only if the Task has not updated since the last time it was in the cache.
     */
    CARAPI_(V) GetAndInvalidateIfModified(
        /* [in] */ ITaskKey* key);

    /** Puts an entry in the cache for a specific key. */
    CARAPI_(void) Put(
        /* [in] */ ITaskKey* key,
        /* [in] */ V value);

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
    AutoPtr<LruCache<AutoPtr<IInteger32>, V> > mCache;
};

//============================================
// KeyStoreLruCache
//============================================

template<typename V>
KeyStoreLruCache<V>::KeyStoreLruCache(
    /* [in] */ Int32 cacheSize)
{
    AutoPtr<MyLruCache> ml = new MyLruCache(cacheSize, this);
    mCache = ml;
}

template<typename V>
V KeyStoreLruCache<V>::Get(
    /* [in] */ ITaskKey* key)
{
    AutoPtr<Task::TaskKey> tk = (Task::TaskKey*)key;
    Int32 id = tk->mId;
    AutoPtr<IInteger32> i;
    CInteger32::New(id, (IInteger32**)&i);
    return mCache->Get(i);
}

template<typename V>
V KeyStoreLruCache<V>::GetAndInvalidateIfModified(
    /* [in] */ ITaskKey* key)
{
    AutoPtr<Task::TaskKey> tk = (Task::TaskKey*)key;
    Int32 id = tk->mId;
    AutoPtr<IInteger32> i;
    CInteger32::New(id, (IInteger32**)&i);
    AutoPtr<IInterface> lastKeyObj;
    mTaskKeys->Get(i, (IInterface**)&lastKeyObj);
    AutoPtr<ITaskKey> lastKey = ITaskKey::Probe(lastKeyObj);
    AutoPtr<Task::TaskKey> tkk = (Task::TaskKey*)lastKey.Get();
    if (lastKey != NULL && (tkk->mLastActiveTime < tk->mLastActiveTime)) {
        // The task has updated (been made active since the last time it was put into the
        // LRU cache) so invalidate that item in the cache
        Remove(key);
        return (V)NULL;
    }
    // Either the task does not exist in the cache, or the last active time is the same as
    // the key specified, so return what is in the cache
    return mCache->Get(i);
}

template<typename V>
void KeyStoreLruCache<V>::Put(
    /* [in] */ ITaskKey* key,
    /* [in] */ V value)
{
    AutoPtr<Task::TaskKey> tk = (Task::TaskKey*)key;
    Int32 id = tk->mId;
    AutoPtr<IInteger32> i;
    CInteger32::New(id, (IInteger32**)&i);

    mCache->Put(i, value);
    mTaskKeys->Put(i, key);
}

template<typename V>
void KeyStoreLruCache<V>::Remove(
    /* [in] */ ITaskKey* key)
{
    AutoPtr<Task::TaskKey> tk = (Task::TaskKey*)key;
    Int32 id = tk->mId;
    AutoPtr<IInteger32> i;
    CInteger32::New(id, (IInteger32**)&i);
    mCache->Remove(i);
    mTaskKeys->Remove(i);
}

template<typename V>
void KeyStoreLruCache<V>::EvictAll()
{
    mCache->EvictAll();
    mTaskKeys->Clear();
}

template<typename V>
Int32 KeyStoreLruCache<V>::Size()
{
    return mCache->Size();
}

template<typename V>
void KeyStoreLruCache<V>::TrimToSize(
    /* [in] */ Int32 cacheSize)
{
    mCache->Resize(cacheSize);
}

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MODEL_KEYSTORELRUCACHE_H__
