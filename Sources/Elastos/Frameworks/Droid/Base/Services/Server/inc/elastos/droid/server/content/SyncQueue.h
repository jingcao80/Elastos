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

#ifndef __ELASTOS_DROID_SERVER_CONTENT_SYNC_QUEUE_H__
#define __ELASTOS_DROID_SERVER_CONTENT_SYNC_QUEUE_H__

#include <elastos/droid/ext/frameworkext.h>
#include <Elastos.Droid.Content.h>
#include "_Elastos.Droid.Server.h"
#include "elastos/droid/server/content/SyncOperation.h"
#include <elastos/droid/server/content/SyncStorageEngine.h>

using Elastos::Droid::Content::ISyncAdaptersCache;
using Elastos::Droid::Content::Pm::IPackageManager;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Content {

/**
 * Queue of pending sync operations. Not inherently thread safe, external
 * callers are responsible for locking.
 *
 * @hide
 */
class SyncQueue
    : public Object
{
public:
    SyncQueue(
        /* [in] */ IPackageManager* packageManager,
        /* [in] */ SyncStorageEngine* syncStorageEngine,
        /* [in] */ ISyncAdaptersCache* syncAdapters);

    void AddPendingOperations(
        /* [in] */ Int32 userId);

    Boolean Add(
        /* [in] */ SyncOperation* operation);

    /**
     * Adds a SyncOperation to the queue and creates a PendingOperation object to track that sync.
     * If an operation is added that already exists, the existing operation is updated if the newly
     * added operation occurs before (or the Int32erval overlaps).
     */
    Boolean Add(
        /* [in] */ SyncOperation* operation,
        /* [in] */ PendingOperation* pop);

    void RemoveUserLocked(
        /* [in] */ Int32 userId);

    /**
     * Remove the specified operation if it is in the queue.
     * @param operation the operation to remove
     */
    void Remove(
        /* [in] */ SyncOperation* operation);

    /** Reset backoffs for all operations in the queue. */
    void ClearBackoffs();

    void OnBackoffChanged(
        /* [in] */ EndPoint* target,
        /* [in] */ Int64 backoff);

    void OnDelayUntilTimeChanged(
        /* [in] */ EndPoint* target,
        /* [in] */ Int64 delayUntil);

    /**
     * Remove all of the SyncOperations associated with a given target.
     *
     * @param info target object provided here can have null Account/provider. This is the case
     * where you want to remove all ops associated with a provider (null Account) or all ops
     * associated with an account (null provider).
     * @param extras option bundle to include to further specify which operation to remove. If this
     * bundle contains sync settings flags, they are ignored.
     */
    void Remove(
        /* [in] */ EndPoint* info,
        /* [in] */ IBundle* extras);

    AutoPtr< HashMap<String, AutoPtr<SyncOperation> > > GetOperations();

    void Dump(
        /* [in] */ StringBuilder* sb);

private:
    static const String TAG;
    AutoPtr<SyncStorageEngine> mSyncStorageEngine;
    AutoPtr<ISyncAdaptersCache> mSyncAdapters;
    AutoPtr<IPackageManager> mPackageManager;

    // A Map of SyncOperations operationKey -> SyncOperation that is designed for
    // quick lookup of an enqueued SyncOperation.
    AutoPtr< HashMap<String, AutoPtr<SyncOperation> > > mOperationsMap;
};

} // namespace Content
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_SERVER_CONTENT_SYNC_QUEUE_H__
