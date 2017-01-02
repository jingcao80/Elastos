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

#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_RANKINGRECONSIDERATION_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_RANKINGRECONSIDERATION_H__

#include "elastos/droid/server/notification/NotificationRecord.h"

using Elastos::Core::Runnable;
using Elastos::Core::IComparator;
using Elastos::Utility::Concurrent::ITimeUnit;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

/**
 * Sorts notifications individually into attention-relelvant order.
 */
class RankingReconsideration
    : public Runnable
    , public IRankingReconsideration
{
public:
    CAR_INTERFACE_DECL()

    RankingReconsideration(
        /* [in] */ const String& key);

    RankingReconsideration(
        /* [in] */ const String& key,
        /* [in] */ Int64 delay);

    virtual ~RankingReconsideration();

    CARAPI_(String) GetKey();

    // @Override
    CARAPI Run();

    CARAPI_(Int64) GetDelay(
        /* [in] */ ITimeUnit* unit);

    CARAPI_(Boolean) Cancel(
        /* [in] */ Boolean mayInterruptIfRunning);

    CARAPI_(Boolean) IsCancelled();

    CARAPI_(Boolean) IsDone();

    /**
     * Analyse the notification.  This will be called on a worker thread. To
     * avoid concurrency issues, do not use held references to modify the
     * {@link NotificationRecord}.
     */
    virtual CARAPI Work() = 0;

    /**
     * Apply any computed changes to the notification record.  This method will be
     * called on the main service thread, synchronized on he mNotificationList.
     * @param record The locked record to be updated.
     */
    virtual CARAPI ApplyChangesLocked(
        /* [in] */ NotificationRecord* record) = 0;

protected:
    String mKey;

private:
    static const Int64 IMMEDIATE;

    static const Int32 START;
    static const Int32 RUNNING;
    static const Int32 DONE;
    static const Int32 CANCELLED;

    Int32 mState;
    Int64 mDelay;
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_RANKINGRECONSIDERATION_H__
