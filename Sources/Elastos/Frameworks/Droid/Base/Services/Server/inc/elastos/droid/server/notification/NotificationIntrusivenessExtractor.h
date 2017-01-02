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

#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONINTRUSIVENESSEXTRACTOR_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONINTRUSIVENESSEXTRACTOR_H__

#include "elastos/droid/server/notification/RankingReconsideration.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

/**
 * This {@link com.android.server.notification.NotificationSignalExtractor} noticies noisy
 * notifications and marks them to get a temporary ranking bump.
 */
class NotificationIntrusivenessExtractor
    : public Object
    , public INotificationIntrusivenessExtractor
    , public INotificationSignalExtractor
{
private:
    class MyRankingReconsideration
        : public RankingReconsideration
    {
    public:
        MyRankingReconsideration(
            /* [in] */ const String& key);

        MyRankingReconsideration(
            /* [in] */ const String& key,
            /* [in] */ Int64 delay);

        ~MyRankingReconsideration();

        // @Override
        CARAPI Work();

        // @Override
        CARAPI ApplyChangesLocked(
            /* [in] */ NotificationRecord* record);
    };

public:
    CAR_INTERFACE_DECL()

    NotificationIntrusivenessExtractor();

    ~NotificationIntrusivenessExtractor();

    CARAPI Initialize(
        /* [in] */ IContext* context);

    CARAPI Process(
        /* [in] */ INotificationRecord* record,
        /* [out] */ IRankingReconsideration** consideration);

    CARAPI SetConfig(
        /* [in] */ IRankingConfig* config);

private:
    static const String TAG;
    static const Boolean DBG;

    /** Length of time (in milliseconds) that an intrusive or noisy notification will stay at
    the top of the ranking order, before it falls back to its natural position. */
    static const Int64 HANG_TIME_MS;
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_NOTIFICATIONINTRUSIVENESSEXTRACTOR_H__
