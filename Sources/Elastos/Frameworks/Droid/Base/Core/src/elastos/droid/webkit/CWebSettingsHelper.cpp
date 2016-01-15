
#include "elastos/droid/webkit/CWebSettingsHelper.h"
#include "elastos/droid/webkit/WebSettings.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CAR_INTERFACE_IMPL(CWebSettingsHelper, Object, IWebSettingsHelper);

CAR_SINGLETON_IMPL(CWebSettingsHelper);

ECode CWebSettingsHelper::GetDefaultUserAgent(
    /* [in] */ IContext* context,
    /* [out] */ String* ua)
{
    VALIDATE_NOT_NULL(ua);
    *ua = WebSettings::GetDefaultUserAgent(context);
    return NOERROR;
}

} // namespace Webkit
} // namespace Droid
} // namespace Elastos
