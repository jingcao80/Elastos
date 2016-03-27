
#include "elastos/droid/app/CProfileTriggerHelper.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CProfileTriggerHelper, Singleton, IProfileTriggerHelper)

CAR_SINGLETON_IMPL(CProfileTriggerHelper)

ECode CProfileTriggerHelper::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IProfileTrigger** result)
{
    VALIDATE_NOT_NULL(result);
    *result = Profile::ProfileTrigger::FromXml(xpp, context);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
