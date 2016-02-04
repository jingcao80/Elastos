
#include "elastos/droid/server/notification/PackagePriorityExtractor.h"
#include "elastos/droid/server/notification/NotificationRecord.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

const String PackagePriorityExtractor::TAG("ImportantPackageExtractor");
const Boolean PackagePriorityExtractor::DBG = FALSE;


CAR_INTERFACE_IMPL_2(PackagePriorityExtractor, Object, IPackagePriorityExtractor, INotificationSignalExtractor);

PackagePriorityExtractor::PackagePriorityExtractor()
{
}

PackagePriorityExtractor::~PackagePriorityExtractor()
{}

ECode PackagePriorityExtractor::Initialize(
    /* [in] */ IContext* context)
{
    if (DBG) Slogger::D(TAG, "Initializing PackagePriorityExtractor.");
    return NOERROR;
}

ECode PackagePriorityExtractor::Process(
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

    if (mConfig == NULL) {
        if (DBG) Slogger::D(TAG, "missing config");
        return NOERROR;
    }

    String pkg;
    r->mSbn->GetPackageName(&pkg);
    Int32 uid;
    r->mSbn->GetUid(&uid);

    Int32 packagePriority;
    mConfig->GetPackagePriority(pkg, uid, &packagePriority);
    r->SetPackagePriority(packagePriority);

    return NOERROR;
}

ECode PackagePriorityExtractor::SetConfig(
    /* [in] */ IRankingConfig* config)
{
    mConfig = config;
    return NOERROR;
}

} // Notification
} // Server
} // Droid
} // Elastos
