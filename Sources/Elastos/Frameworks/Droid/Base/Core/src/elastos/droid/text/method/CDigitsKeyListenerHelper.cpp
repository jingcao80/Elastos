#include "elastos/droid/text/method/CDigitsKeyListenerHelper.h"
#include "elastos/droid/text/method/DigitsKeyListener.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CDigitsKeyListenerHelper)

CAR_INTERFACE_IMPL(CDigitsKeyListenerHelper, Singleton, IDigitsKeyListenerHelper)

ECode CDigitsKeyListenerHelper::GetInstance(
    /* [out] */ IDigitsKeyListener** ret)
{
    return DigitsKeyListener::GetInstance(ret);
}

ECode CDigitsKeyListenerHelper::GetInstance(
    /* [in] */ Boolean sign,
    /* [in] */ Boolean decimal,
    /* [out] */ IDigitsKeyListener** ret)
{
    return DigitsKeyListener::GetInstance(sign, decimal, ret);
}

ECode CDigitsKeyListenerHelper::GetInstance(
    /* [in] */ const String& accepted,
    /* [out] */ IDigitsKeyListener** ret)
{
    return DigitsKeyListener::GetInstance(accepted, ret);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

