//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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

ECode KeyStoreLruCache::MyLruCache::constructor(
    /* [in] */ Int32 cacheSize,
    /* [in] */ KeyStoreLruCache* host)
{
    mHost = host;
    return LruCache::constructor(cacheSize);
}

ECode KeyStoreLruCache::MyLruCache::EntryRemoved(
    /* [in] */ Boolean evicted,
    /* [in] */ IInterface* taskId,
    /* [in] */ IInterface* oldV,
    /* [in] */ IInterface* newV)
{
    mHost->mTaskKeys->Remove(taskId);
    return NOERROR;
}

//============================================
// KeyStoreLruCache
//============================================

KeyStoreLruCache::KeyStoreLruCache(
    /* [in] */ Int32 cacheSize)
{
    AutoPtr<MyLruCache> ml = new MyLruCache();
    ml->constructor(cacheSize, this);
    mCache = ml.Get();
    CHashMap::New((IHashMap**)&mTaskKeys);
}

AutoPtr<IInterface> KeyStoreLruCache::Get(
    /* [in] */ ITaskKey* key)
{
    AutoPtr<Task::TaskKey> tk = (Task::TaskKey*)key;
    Int32 id = tk->mId;
    AutoPtr<IInteger32> i;
    CInteger32::New(id, (IInteger32**)&i);
    AutoPtr<IInterface> value;
    mCache->Get(i, (IInterface**)&value);
    return value;
}

AutoPtr<IInterface> KeyStoreLruCache::GetAndInvalidateIfModified(
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
        return NULL;
    }
    // Either the task does not exist in the cache, or the last active time is the same as
    // the key specified, so return what is in the cache
    AutoPtr<IInterface> value;
    mCache->Get(i, (IInterface**)&value);
    return value;
}

void KeyStoreLruCache::Put(
    /* [in] */ ITaskKey* key,
    /* [in] */ IInterface* value)
{
    AutoPtr<Task::TaskKey> tk = (Task::TaskKey*)key;
    Int32 id = tk->mId;
    AutoPtr<IInteger32> i;
    CInteger32::New(id, (IInteger32**)&i);

    mCache->Put(i, value, NULL);
    mTaskKeys->Put(i, key);
}

void KeyStoreLruCache::Remove(
    /* [in] */ ITaskKey* key)
{
    AutoPtr<Task::TaskKey> tk = (Task::TaskKey*)key;
    Int32 id = tk->mId;
    AutoPtr<IInteger32> i;
    CInteger32::New(id, (IInteger32**)&i);
    mCache->Remove(i, NULL);
    mTaskKeys->Remove(i);
}

void KeyStoreLruCache::EvictAll()
{
    mCache->EvictAll();
    mTaskKeys->Clear();
}

Int32 KeyStoreLruCache::Size()
{
    Int32 size;
    mCache->GetSize(&size);
    return size;
}

void KeyStoreLruCache::TrimToSize(
    /* [in] */ Int32 cacheSize)
{
    mCache->Resize(cacheSize);
}

} // namespace Model
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos