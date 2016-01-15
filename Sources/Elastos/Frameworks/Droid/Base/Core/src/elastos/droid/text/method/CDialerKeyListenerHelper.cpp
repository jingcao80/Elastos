#include "elastos/droid/text/method/CDialerKeyListenerHelper.h"
#include "elastos/droid/text/method/DialerKeyListener.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CDialerKeyListenerHelper)

CAR_INTERFACE_IMPL(CDialerKeyListenerHelper, Singleton, IDialerKeyListenerHelper)

ECode CDialerKeyListenerHelper::GetCHARACTERS(
    /* [out, callee] */ ArrayOf<Char32>** ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = DialerKeyListener::CHARACTERS;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode CDialerKeyListenerHelper::GetInstance(
    /* [out] */ IDialerKeyListener** ret)
{
    return DialerKeyListener::GetInstance(ret);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

