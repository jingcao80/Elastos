#include "elastos/droid/text/method/CTimeKeyListenerHelper.h"
#include "elastos/droid/text/method/TimeKeyListener.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CTimeKeyListenerHelper)

CAR_INTERFACE_IMPL(CTimeKeyListenerHelper, Singleton, ITimeKeyListenerHelper)

ECode CTimeKeyListenerHelper::GetInstance(
    /* [out] */ ITimeKeyListener** ret)
{
    return TimeKeyListener::GetInstance(ret);
}

ECode CTimeKeyListenerHelper::GetCHARACTERS(
    /* [out, callee] */ ArrayOf<Char32>** ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = TimeKeyListener::CHARACTERS;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

