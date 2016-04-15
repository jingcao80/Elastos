
#ifndef __ELASTOS_APPS_DIALERBINDER_ANALYTICSACTIVITY_H__
#define __ELASTOS_APPS_DIALERBINDER_ANALYTICSACTIVITY_H__

#include <Activity.h>

using Elastos::Droid::App::Activity;

namespace Elastos{
namespace Apps{
namespace DialerBinder {

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

} // DialerBinder
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALERBINDER_ANALYTICSACTIVITY_H__
