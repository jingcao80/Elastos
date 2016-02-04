
#ifndef __ELASTOS_DROID_SERVER_NOTIFICATION_PACKAGEVISIBILITYEXTRACTOR_H__
#define __ELASTOS_DROID_SERVER_NOTIFICATION_PACKAGEVISIBILITYEXTRACTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include <_Elastos.Droid.Server.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Notification {

class PackageVisibilityExtractor
    : public Object
    , public IPackageVisibilityExtractor
    , public INotificationSignalExtractor
{
public:
    CAR_INTERFACE_DECL();

    PackageVisibilityExtractor();

    ~PackageVisibilityExtractor();

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

    AutoPtr<IRankingConfig> mConfig;
};

} // Notification
} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_NOTIFICATION_PACKAGEVISIBILITYEXTRACTOR_H__
