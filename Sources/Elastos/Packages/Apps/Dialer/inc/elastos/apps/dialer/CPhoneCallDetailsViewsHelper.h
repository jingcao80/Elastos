#ifndef __ELASTOS_APPS_DIALER_CPHONECALLDETAILSVIEWSHELPER_H__
#define __ELASTOS_APPS_DIALER_CPHONECALLDETAILSVIEWSHELPER_H__

#include "_Elastos_Apps_Dialer_CPhoneCallDetailsViewsHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Dialer{

CarClass(CPhoneCallDetailsViewsHelper)
    , public Singleton
    , public IPhoneCallDetailsViewsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Create a new instance by extracting the elements from the given view.
     * <p>
     * The view should contain three text views with identifiers {@code R.id.name},
     * {@code R.id.date}, and {@code R.id.number}, and a linear layout with identifier
     * {@code R.id.call_types}.
     */
    CARAPI FromView(
        /* [in] */ IView* view,
        /* [out] */  IPhoneCallDetailsViews** detailsViews);

    CARAPI CreateForTest(
        /* [in] */ IContext* context,
        /* [out] */  IPhoneCallDetailsViews** detailsViews);
};

} // Dialer
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_DIALER_CPHONECALLDETAILSVIEWSHELPER_H__
