#include "elastos/droid/text/method/CDateKeyListenerHelper.h"
#include "elastos/droid/text/method/CDateKeyListener.h"
#include "elastos/droid/text/method/DateKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CDateKeyListenerHelper)

CAR_INTERFACE_IMPL(CDateKeyListenerHelper, Singleton, IDateKeyListenerHelper)

ECode CDateKeyListenerHelper::GetCHARACTERS(
    /* [out, callee] */ ArrayOf<Char32>** ret)
{
    return DateKeyListener::GetCHARACTERS(ret);
}

ECode CDateKeyListenerHelper::GetInstance(
    /* [out] */ IDateKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret)
    AutoPtr<IDateKeyListener> l = DateKeyListener::GetInstance();
    *ret = l;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

