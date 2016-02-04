
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
    CAR_INTERFACE_DECL();

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
