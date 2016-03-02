
#include "elastos/droid/app/CBrightnessSettingsHelper.h"
#include "elastos/droid/app/CBrightnessSettings.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_SINGLETON_IMPL(CBrightnessSettingsHelper)

CAR_INTERFACE_IMPL(CBrightnessSettingsHelper, Singleton, IBrightnessSettingsHelper)

ECode CBrightnessSettingsHelper::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IBrightnessSettings** obj)
{
    return CBrightnessSettings::FromXml(xpp, context, obj);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
