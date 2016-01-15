
#include "elastos/droid/internal/utility/CNotificationColorUtilHelper.h"
#include "elastos/droid/internal/utility/NotificationColorUtil.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

CAR_INTERFACE_IMPL(CNotificationColorUtilHelper, Singleton, INotificationColorUtilHelper)

CAR_SINGLETON_IMPL(CNotificationColorUtilHelper)

ECode CNotificationColorUtilHelper::GetInstance(
    /* [in] */ IContext* context,
    /* [out] */ INotificationColorUtil** instance)
{
    VALIDATE_NOT_NULL(instance)
    AutoPtr<INotificationColorUtil> temp = NotificationColorUtil::GetInstance(context);
    *instance = temp.Get();
    REFCOUNT_ADD(*instance)
    return NOERROR;
}

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos
