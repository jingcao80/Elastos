#ifndef __ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSPREFERENCEACTIVITY_H__
#define __ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSPREFERENCEACTIVITY_H__

namespace Elastos{
namespace Apps{
namespace DialerBind {
namespace Analytics {

class AnalyticsPreferenceActivity
    : public PreferenceActivity
    , public IAnalyticsPreferenceActivity
    , public IAnalyticsInterface
{
public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI SendHitEvent(
        /* [in] */ const String& categoryId,
        /* [in] */ const String& actionId,
        /* [in] */ const String& labelId);

    // @Override
    CARAPI SendScreenView();
};

} // Analytics
} // DialerBind
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSPREFERENCEACTIVITY_H__
