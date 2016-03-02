
#include "elastos/droid/app/CAirplaneModeSettings.h"
#include "elastos/droid/app/CAirplaneModeSettingsHelper.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_SINGLETON_IMPL(CAirplaneModeSettingsHelper)

CAR_INTERFACE_IMPL(CAirplaneModeSettingsHelper, Singleton, IAirplaneModeSettingsHelper)

ECode CAirplaneModeSettingsHelper::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IAirplaneModeSettings** obj)
{
    return CAirplaneModeSettings::FromXml(xpp, context, obj);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
