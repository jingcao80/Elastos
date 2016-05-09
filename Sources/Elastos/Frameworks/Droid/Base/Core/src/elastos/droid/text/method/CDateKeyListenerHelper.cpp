#include "elastos/droid/text/method/CDateKeyListenerHelper.h"
#include "elastos/droid/text/method/CDateKeyListener.h"
#include "elastos/droid/text/method/DateKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CDateKeyListenerHelper)

CAR_INTERFACE_IMPL(CDateKeyListenerHelper, Singleton, IDateKeyListenerHelper)

AutoPtr<IDateKeyListener> CDateKeyListenerHelper::sInstance;

ECode CDateKeyListenerHelper::GetCHARACTERS(
    /* [out, callee] */ ArrayOf<Char32>** ret)
{
    return DateKeyListener::GetCHARACTERS(ret);
}

ECode CDateKeyListenerHelper::GetInstance(
    /* [out] */ IDateKeyListener** ret)
{
    if (sInstance == NULL) {
        CDateKeyListener::New((IDateKeyListener**)&sInstance);
    }

    *ret = sInstance;
    REFCOUNT_ADD(*ret)
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

