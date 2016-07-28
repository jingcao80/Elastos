
#ifndef __ELASTOS_DROID_WIDGET_CAPPSECURITYPERMISSIONSPERMISSIONITEMVIEW_H__
#define __ELASTOS_DROID_WIDGET_CAPPSECURITYPERMISSIONSPERMISSIONITEMVIEW_H__

#include "_Elastos_Droid_Widget_CAppSecurityPermissionsPermissionItemView.h"
#include "elastos/droid/widget/CAppSecurityPermissions.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CarClass(CAppSecurityPermissionsPermissionItemView)
    , public CAppSecurityPermissions::PermissionItemView
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_WIDGET_CAPPSECURITYPERMISSIONSPERMISSIONITEMVIEW_H__
