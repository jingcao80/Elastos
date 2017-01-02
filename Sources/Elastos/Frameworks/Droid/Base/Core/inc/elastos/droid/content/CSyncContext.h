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

#ifndef __ELASTOS_DROID_CONTENT_CSYNCCONTEXT_H__
#define __ELASTOS_DROID_CONTENT_CSYNCCONTEXT_H__

#include "_Elastos_Droid_Content_CSyncContext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::IBinder;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CSyncContext)
    , public Object
    , public ISyncContext
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSyncContext();

    ~CSyncContext();

    /**
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IISyncContext* syncContextInterface);

    /**
     * Call to update the status text for this sync. This internally invokes
     * {@link #updateHeartbeat}, so it also takes the place of a call to that.
     *
     * @param message the current status message for this sync
     *
     * @hide
     */
    CARAPI SetStatusText(
        /* [in] */ const String& message);

    CARAPI OnFinished(
        /* [in] */ ISyncResult* result);

    CARAPI GetSyncContextBinder(
        /* [out] */ IBinder** binder);

private:
    /**
     * Call to indicate that the SyncAdapter is making progress. E.g., if this SyncAdapter
     * downloads or sends records to/from the server, this may be called after each record
     * is downloaded or uploaded.
     */
    CARAPI UpdateHeartbeat();

private:
    static const Int64 HEARTBEAT_SEND_INTERVAL_IN_MS = 1000;

private:
    AutoPtr<IISyncContext> mSyncContext;
    Int64 mLastHeartbeatSendTime;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CSYNCCONTEXT_H__
