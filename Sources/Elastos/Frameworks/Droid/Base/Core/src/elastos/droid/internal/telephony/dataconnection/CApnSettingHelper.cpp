
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/internal/telephony/dataconnection/CApnSetting.h"
#include "elastos/droid/internal/telephony/dataconnection/CApnSettingHelper.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace DataConnection {

CAR_INTERFACE_IMPL(CApnSettingHelper, Singleton, IApnSettingHelper)

CAR_SINGLETON_IMPL(CApnSettingHelper)

ECode CApnSettingHelper::FromString(
    /* [in] */ const String& data,
    /* [out] */ IApnSetting** result)
{
    return CApnSetting::FromString(data, result);
}

ECode CApnSettingHelper::ArrayFromString(
    /* [in] */ const String& data,
    /* [out] */ IList** result)
{
    return CApnSetting::ArrayFromString(data, result);
}

} // namespace DataConnection
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos
