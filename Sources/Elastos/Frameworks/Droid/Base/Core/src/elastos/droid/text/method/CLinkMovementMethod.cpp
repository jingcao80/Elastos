#include "elastos/droid/text/method/CLinkMovementMethod.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

AutoPtr<ILinkMovementMethod> CLinkMovementMethod::sInstance;

IBASEMOVEMENTMETHOD_METHODS_IMPL(CLinkMovementMethod, LinkMovementMethod, LinkMovementMethod)

AutoPtr<IMovementMethod> CLinkMovementMethod::GetInstance()
{
    if (sInstance == NULL) {
        AutoPtr<CLinkMovementMethod> lmm;
        CLinkMovementMethod::NewByFriend((CLinkMovementMethod**)&lmm);
        sInstance = (ILinkMovementMethod*)(lmm.Get());
    }
    AutoPtr<IMovementMethod> sInstanceR = IMovementMethod::Probe(sInstance);
    return sInstanceR;
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

