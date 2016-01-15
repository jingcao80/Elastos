#include "elastos/droid/text/method/CMultiTapKeyListenerHelper.h"
#include "elastos/droid/text/method/MultiTapKeyListener.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CMultiTapKeyListenerHelper)

CAR_INTERFACE_IMPL(CMultiTapKeyListenerHelper, Singleton, IMultiTapKeyListenerHelper)

ECode CMultiTapKeyListenerHelper::GetInstance(
    /* [in] */ Boolean autotext,
    /* [in] */ Capitalize cap,
    /* [out] */ IMultiTapKeyListener** ret)
{
    return MultiTapKeyListener::GetInstance(autotext, cap, ret);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

