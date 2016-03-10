
#include "elastos/droid/app/CRingModeSettings.h"
#include "elastos/droid/app/CRingModeSettingsHelper.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_SINGLETON_IMPL(CRingModeSettingsHelper)

CAR_INTERFACE_IMPL(CRingModeSettingsHelper, Singleton, IRingModeSettingsHelper)

ECode CRingModeSettingsHelper::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IRingModeSettings** obj)
{
    return CRingModeSettings::FromXml(xpp, context, obj);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
