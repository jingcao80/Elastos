
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/net/CMailToHelper.h"
#include "elastos/droid/net/MailTo.h"

namespace Elastos {
namespace Droid {
namespace Net {

CAR_SINGLETON_IMPL(CMailToHelper)

CAR_INTERFACE_IMPL(CMailToHelper, Singleton, IMailToHelper)

ECode CMailToHelper::IsMailTo(
    /* [in] */ const String& url,
    /* [out] */ Boolean* result)
{
    return MailTo::IsMailTo(url, result);
}

ECode CMailToHelper::Parse(
    /* [in] */ const String& url,
    /* [out] */ IMailTo** result)
{
    return MailTo::Parse(url, result);
}

} // namespace Net
} // namepsace Droid
} // namespace Elastos
