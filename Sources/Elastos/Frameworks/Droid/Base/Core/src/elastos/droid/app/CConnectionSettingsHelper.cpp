
#include "elastos/droid/app/CConnectionSettings.h"
#include "elastos/droid/app/CConnectionSettingsHelper.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_SINGLETON_IMPL(CConnectionSettingsHelper)

CAR_INTERFACE_IMPL(CConnectionSettingsHelper, Singleton, IConnectionSettingsHelper)

ECode CConnectionSettingsHelper::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IConnectionSettings** obj)
{
    return CConnectionSettings::FromXml(xpp, context, obj);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
