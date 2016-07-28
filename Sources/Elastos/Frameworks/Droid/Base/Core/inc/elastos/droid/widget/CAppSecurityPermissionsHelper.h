#ifndef __ELASTOS_DROID_WIDGET_CAPPSECURITYPERMISSIONSHELPER_H__
#define __ELASTOS_DROID_WIDGET_CAPPSECURITYPERMISSIONSHELPER_H__

#include "_Elastos_Droid_Widget_CAppSecurityPermissionsHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::View::IView;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CAppSecurityPermissionsHelper)
    , public Singleton
    , public IAppSecurityPermissionsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI GetPermissionItemView(
        /* [in] */ IContext* context,
        /* [in] */ ICharSequence* grpName,
        /* [in] */ ICharSequence* description,
        /* [in] */ Boolean dangerous,
        /* [out] */ IView** view);
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_CAPPSECURITYPERMISSIONSHELPER_H__
