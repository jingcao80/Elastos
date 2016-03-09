
#include "elastos/droid/app/CStreamSettings.h"
#include "elastos/droid/app/CStreamSettingsHelper.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_SINGLETON_IMPL(CStreamSettingsHelper)

CAR_INTERFACE_IMPL(CStreamSettingsHelper, Singleton, IStreamSettingsHelper)

ECode CStreamSettingsHelper::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IStreamSettings** obj)
{
    return CStreamSettings::FromXml(xpp, context, obj);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
