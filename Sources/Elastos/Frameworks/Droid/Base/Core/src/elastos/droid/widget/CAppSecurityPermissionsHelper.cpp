
#include "elastos/droid/widget/CAppSecurityPermissionsHelper.h"
#include "elastos/droid/widget/CAppSecurityPermissions.h"

namespace Elastos {
namespace Droid {
namespace Widget {

CAR_SINGLETON_IMPL(CAppSecurityPermissionsHelper)

CAR_INTERFACE_IMPL(CAppSecurityPermissionsHelper, Singleton, IAppSecurityPermissionsHelper)

ECode CAppSecurityPermissionsHelper::GetPermissionItemView(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* grpName,
    /* [in] */ ICharSequence* description,
    /* [in] */ Boolean dangerous,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view)
    AutoPtr<IView> temp = CAppSecurityPermissions::GetPermissionItemView(
        context, grpName, description, dangerous);
    *view = temp;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
