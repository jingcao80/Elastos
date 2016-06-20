
#include "CPhoneCallDetailsViewsHelper.h"
#include "PhoneCallDetailsViews.h"

namespace Elastos {
namespace Apps {
namespace Dialer {

CAR_INTERFACE_IMPL(CPhoneCallDetailsViewsHelper, Singleton, IPhoneCallDetailsViewsHelper);

CAR_SINGLETON_IMPL(CPhoneCallDetailsViewsHelper);

ECode CPhoneCallDetailsViewsHelper::FromView(
    /* [in] */ IView* view,
    /* [out] */  IPhoneCallDetailsViews** detailsViews)
{
    VALIDATE_NOT_NULL(detailsViews);
    AutoPtr<IPhoneCallDetailsViews> views = PhoneCallDetailsViews::FromView(view);
    *detailsViews = views;
    REFCOUNT_ADD(*detailsViews);
    return NOERROR;
}

ECode CPhoneCallDetailsViewsHelper::CreateForTest(
    /* [in] */ IContext* context,
    /* [out] */  IPhoneCallDetailsViews** detailsViews)
{
    VALIDATE_NOT_NULL(detailsViews);
    AutoPtr<IPhoneCallDetailsViews> views = PhoneCallDetailsViews::CreateForTest(context);
    *detailsViews = views;
    REFCOUNT_ADD(*detailsViews);
    return NOERROR;
}

} // Dialer
} // Apps
} // Elastos
