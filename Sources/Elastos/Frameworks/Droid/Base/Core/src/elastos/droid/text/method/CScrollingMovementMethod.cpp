#include "elastos/droid/text/method/CScrollingMovementMethod.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

AutoPtr<IScrollingMovementMethod> CScrollingMovementMethod::sInstance;

IBASEMOVEMENTMETHOD_METHODS_IMPL(CScrollingMovementMethod, ScrollingMovementMethod, ScrollingMovementMethod)

AutoPtr<IMovementMethod> CScrollingMovementMethod::GetInstance()
{
    if (sInstance == NULL) {
        AutoPtr<CScrollingMovementMethod> smm;
        CScrollingMovementMethod::NewByFriend((CScrollingMovementMethod**)&smm);
        sInstance = (IScrollingMovementMethod*)(smm.Get());
    }
    AutoPtr<IMovementMethod> mm = (IMovementMethod*)(sInstance.Get());
    return mm;
}


} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

