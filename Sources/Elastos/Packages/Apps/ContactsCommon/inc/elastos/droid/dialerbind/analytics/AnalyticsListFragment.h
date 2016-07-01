#ifndef __ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSLISTFRAGMENT_H__
#define __ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSLISTFRAGMENT_H__

namespace Elastos{
namespace Apps{
namespace DialerBind {
namespace Analytics {

class ECO_PUBLIC AnalyticsListFragment
    /* : public ListFragment*/
    : public Object
    , public IAnalyticsInterface
{
public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI SendHitEvent(
        /* [in] */ const String& categoryId,
        /* [in] */ const String& actionId,
        /* [in] */ const String& labelId)
    {}

    // @Override
    CARAPI SendScreenView()
    {}

};

} // Analytics
} // DialerBind
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALERBIND_ANALYTICS_ANALYTICSLISTFRAGMENT_H__
