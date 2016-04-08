#include "elastos/droid/systemui/recents/model/KeyStoreLruCache.h"
#include "elastos/droid/systemui/recents/model/Task.h"

using Elastos::Core::CInteger32;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Model {

//============================================
// KeyStoreLruCache::MyLruCache
//============================================

template<typename V>
KeyStoreLruCache<V>::MyLruCache<V>::MyLruCache(
    /* [in] */ Int32 cacheSize,
    /* [in] */ KeyStoreLruCache<V>* host)
    : LruCache<IInteger32*, V>(cacheSize)
    , mHost(host)
{
}

template<typename V>
void KeyStoreLruCache<V>::MyLruCache<V>::EntryRemoved(
    /* [in] */ Boolean evicted,
    /* [in] */ IInteger32* taskId,
    /* [in] */ V oldV,
    /* [in] */ V newV)
{
    mHost->mTaskKeys->Remove(taskId);
}

//============================================
// KeyStoreLruCache
//============================================

template<typename V>
KeyStoreLruCache<V>::KeyStoreLruCache(
    /* [in] */ Int32 cacheSize)
{
    AutoPtr<MyLruCache<V> > ml = new MyLruCache<V>(cacheSize, this);
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
    AutoPtr<Task::TaskKey> tkk = (Task::TaskKey*)lastKey;
    if (lastKey != NULL && (tkk->mLastActiveTime < tk->mLastActiveTime)) {
        // The task has updated (been made active since the last time it was put into the
        // LRU cache) so invalidate that item in the cache
        Remove(key);
        return NULL;
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
    return NOERROR;
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