#ifndef __ELASTOS_APPS_DIALERBINDER_ANALYTICSLISTFRAGMENT_H__
#define __ELASTOS_APPS_DIALERBINDER_ANALYTICSLISTFRAGMENT_H__

namespace Elastos{
namespace Apps{
namespace DialerBinder {

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

} // DialerBinder
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALERBINDER_ANALYTICSLISTFRAGMENT_H__
