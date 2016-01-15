
#include "elastos/droid/internal/net/CVpnConfigHelper.h"
#include "elastos/droid/internal/net/CVpnConfig.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

CAR_INTERFACE_IMPL(CVpnConfigHelper, Singleton, IVpnConfigHelper)

CAR_SINGLETON_IMPL(CVpnConfigHelper)

ECode CVpnConfigHelper::GetIntentForConfirmation(
    /* [out] */ IIntent** intent)
{
    return CVpnConfig::GetIntentForConfirmation(intent);
}

ECode CVpnConfigHelper::GetIntentForStatusPanel(
    /* [in] */ IContext* context,
    /* [out] */ IPendingIntent** intent)
{
    return CVpnConfig::GetIntentForStatusPanel(context, intent);
}

ECode CVpnConfigHelper::GetVpnLabel(
    /* [in] */ IContext* context,
    /* [in] */ const String& packageName,
    /* [out] */ ICharSequence** label)
{
    return CVpnConfig::GetVpnLabel(context, packageName, label);
}

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos
