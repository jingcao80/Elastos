
#include "elastos/droid/app/CNotificationGroup.h"
#include "elastos/droid/app/CNotificationGroupHelper.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_SINGLETON_IMPL(CNotificationGroupHelper)

CAR_INTERFACE_IMPL(CNotificationGroupHelper, Singleton, INotificationGroupHelper)

ECode CNotificationGroupHelper::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ INotificationGroup** group)
{
    return CNotificationGroup::FromXml(xpp, context, group);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
