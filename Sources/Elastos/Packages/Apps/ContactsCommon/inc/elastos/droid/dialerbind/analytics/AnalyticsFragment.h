#ifndef __ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSLISTFRAGMENT_H__
#define __ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSLISTFRAGMENT_H__

namespace Elastos{
namespace Apps{
namespace DialerBind {
namespace Analytics {

class AnalyticsFragment
    : public Fragment
    , public IAnalyticsFragment
    , public IAnalyticsInterface
{
public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI SendHitEvent(
        /* [in] */ String categoryId,
        /* [in] */ String actionId,
        /* [in] */ String labelId);

    // @Override
    CARAPI SendScreenView();

};

} // Analytics
} // DialerBind
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSLISTFRAGMENT_H__
