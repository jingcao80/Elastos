
#include "elastos/droid/server/notification/PackageVisibilityExtractor.h"
#include "elastos/droid/server/notification/NotificationRecord.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

const String PackageVisibilityExtractor::TAG("PackageVisibilityExtractor");
const Boolean PackageVisibilityExtractor::DBG = FALSE;

CAR_INTERFACE_IMPL_2(PackageVisibilityExtractor, Object, IPackageVisibilityExtractor, INotificationSignalExtractor);

PackageVisibilityExtractor::PackageVisibilityExtractor()
{
}

PackageVisibilityExtractor::~PackageVisibilityExtractor()
{}

ECode PackageVisibilityExtractor::Initialize(
    /* [in] */ IContext* context)
{
    if (DBG) Slogger::D(TAG, "Initializing PackageVisibilityExtractor.");
    return NOERROR;
}

ECode PackageVisibilityExtractor::Process(
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

    Int32 packageVisibility;
    mConfig->GetPackageVisibilityOverride(pkg, uid, &packageVisibility);
    r->SetPackageVisibilityOverride(packageVisibility);

    return NOERROR;
}

ECode PackageVisibilityExtractor::SetConfig(
    /* [in] */ IRankingConfig* config)
{
    mConfig = config;
    return NOERROR;
}

} // Notification
} // Server
} // Droid
} // Elastos
