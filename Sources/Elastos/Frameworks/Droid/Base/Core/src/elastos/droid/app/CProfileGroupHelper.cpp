
#include "elastos/droid/app/CProfileGroup.h"
#include "elastos/droid/app/CProfileGroupHelper.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_SINGLETON_IMPL(CProfileGroupHelper)

CAR_INTERFACE_IMPL(CProfileGroupHelper, Singleton, IProfileGroupHelper)

ECode CProfileGroupHelper::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IProfileGroup** obj)
{
    return CProfileGroup::FromXml(xpp, context, obj);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
