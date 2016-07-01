
#ifndef __ELASTOS_DROID_DIALERBIND_ANALYTICS_ANALYTICSACTIVITY_H__
#define __ELASTOS_DROID_DIALERBIND_ANALYTICS_ANALYTICSACTIVITY_H__

#include "_Elastos.Droid.ContactsCommon.h"
#include <elastos/droid/app/Activity.h>

using Elastos::Droid::App::Activity;

namespace Elastos {
namespace Droid {
namespace DialerBind {
namespace Analytics {

class ECO_PUBLIC AnalyticsActivity
    : public Activity
    , public IAnalyticsInterface
{
public:
    CAR_INTERFACE_DECL();

    CARAPI SendHitEvent(
        /* [in] */ const String& categoryId,
        /* [in] */ const String& actionId,
        /* [in] */ const String& labelId);

    CARAPI SendScreenView();
};

} // Analytics
} // DialerBind
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALERBIND_ANALYTICS_ANALYTICSACTIVITY_H__
