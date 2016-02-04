
#include <Elastos.Droid.App.h>
#include "elastos/droid/server/notification/NotificationIntrusivenessExtractor.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::App::INotification;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

const String NotificationIntrusivenessExtractor::TAG("NotificationNoiseExtractor");
const Boolean NotificationIntrusivenessExtractor::DBG = FALSE;

const Int64 NotificationIntrusivenessExtractor::HANG_TIME_MS = 10000;

//===============================================================================
//                  NotificationIntrusivenessExtractor::MyRankingReconsideration
//===============================================================================

NotificationIntrusivenessExtractor::MyRankingReconsideration::MyRankingReconsideration(
    /* [in] */ const String& key)
    : RankingReconsideration(key)
{
}

NotificationIntrusivenessExtractor::MyRankingReconsideration::MyRankingReconsideration(
    /* [in] */ const String& key,
    /* [in] */ Int64 delay)
    : RankingReconsideration(key, delay)
{
}

NotificationIntrusivenessExtractor::MyRankingReconsideration::~MyRankingReconsideration()
{}

ECode NotificationIntrusivenessExtractor::MyRankingReconsideration::Work()
{
    // pass
    return NOERROR;
}

ECode NotificationIntrusivenessExtractor::MyRankingReconsideration::ApplyChangesLocked(
    /* [in] */ NotificationRecord* record)
{
    record->SetRecentlyIntusive(FALSE);
    return NOERROR;
}

//===============================================================================
//                  NotificationIntrusivenessExtractor
//===============================================================================

CAR_INTERFACE_IMPL_2(NotificationIntrusivenessExtractor, Object, INotificationIntrusivenessExtractor, INotificationSignalExtractor);

NotificationIntrusivenessExtractor::NotificationIntrusivenessExtractor()
{
}

NotificationIntrusivenessExtractor::~NotificationIntrusivenessExtractor()
{}

ECode NotificationIntrusivenessExtractor::Initialize(
    /* [in] */ IContext* context)
{
    if (DBG) Slogger::D(TAG, "Initializing NotificationIntrusivenessExtractor.");
    return NOERROR;
}

ECode NotificationIntrusivenessExtractor::Process(
    /* [in] */ INotificationRecord* record,
    /* [out] */ IRankingReconsideration** consideration)
{
    VALIDATE_NOT_NULL(consideration);
    *consideration = NULL;

    AutoPtr<NotificationRecord> r = (NotificationRecord*)record;

    if (r == NULL || r->GetNotification() == NULL) {
        if (DBG) Slogger::D(TAG, "skipping empty notification");
        return NOERROR;
    }

    AutoPtr<INotification> notification = r->GetNotification();
    Int32 defaults;
    notification->GetDefaults(&defaults);

    AutoPtr< ArrayOf<Int64> > vibrate;
    notification->GetVibrate((ArrayOf<Int64>**)&vibrate);

    AutoPtr<IUri> sound;
    notification->GetSound((IUri**)&sound);

    AutoPtr<IPendingIntent> fullScreenIntent;
    notification->GetFullScreenIntent((IPendingIntent**)&fullScreenIntent);

    if ((defaults & INotification::DEFAULT_VIBRATE) != 0 ||
            vibrate != NULL ||
            (defaults & INotification::DEFAULT_SOUND) != 0 ||
            sound != NULL ||
            fullScreenIntent != NULL) {
        r->SetRecentlyIntusive(TRUE);
    }

    AutoPtr<MyRankingReconsideration> ation = new MyRankingReconsideration(
            r->GetKey(), HANG_TIME_MS);
    *consideration = (IRankingReconsideration*)ation.Get();
    REFCOUNT_ADD(*consideration);
    return NOERROR;
}

ECode NotificationIntrusivenessExtractor::SetConfig(
    /* [in] */ IRankingConfig* config)
{
    // ignore: config has no relevant information yet.
    return NOERROR;
}

} // Notification
} // Server
} // Droid
} // Elastos
