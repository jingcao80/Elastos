
#ifndef __ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSACTIVITY_H__
#define __ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSACTIVITY_H__

#include <Activity.h>

using Elastos::Droid::App::Activity;

namespace Elastos{
namespace Apps{
namespace DailerBind {
namespace Analytics {

class ECO_PUBLIC AnalyticsActivity
    : public Activity
    , public IAnalyticsInterface
{
public:
    CARAPI SendHitEvent(
        /* [in] */ const String& categoryId,
        /* [in] */ const String& actionId,
        /* [in] */ const String& labelId)
    {
        return NOERROR;
    }

    CARAPI SendScreenView()
    {
        return NOERROR;
    }

};

} // Analytics
} // DailerBind
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSACTIVITY_H__
