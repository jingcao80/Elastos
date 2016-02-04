#include "elastos/droid/app/CNotificationHelper.h"
#include "elastos/droid/app/CNotification.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CNotificationHelper, Object, INotificationHelper)

CAR_SINGLETON_IMPL(CNotificationHelper)

ECode CNotificationHelper::GetAUDIO_ATTRIBUTES_DEFAULT(
    /* [out] */ IAudioAttributes** attr)
{
    VALIDATE_NOT_NULL(attr);
    *attr = CNotification::AUDIO_ATTRIBUTES_DEFAULT;
    REFCOUNT_ADD(*attr);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
