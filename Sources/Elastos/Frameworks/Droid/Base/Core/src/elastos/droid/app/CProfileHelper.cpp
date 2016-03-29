
#include "elastos/droid/app/CProfileHelper.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CProfileHelper, Singleton, IProfileHelper)

CAR_SINGLETON_IMPL(CProfileHelper)

ECode CProfileHelper::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IProfile** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IProfile> p = Profile::FromXml(xpp, context);
    *result = p;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
