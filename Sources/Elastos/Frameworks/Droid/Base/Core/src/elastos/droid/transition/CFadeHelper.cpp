
#include "elastos/droid/transition/CFadeHelper.h"
#include "elastos/droid/transition/CFade.h"

namespace Elastos {
namespace Droid {
namespace Transition {

CAR_SINGLETON_IMPL(CFadeHelper)

CAR_INTERFACE_IMPL(CFadeHelper, Singleton, IFadeHelper)

ECode CFadeHelper::GetIN(
    /* [out] */ Int32* _in)
{
    VALIDATE_NOT_NULL(_in)

    *_in = CFade::IN;
    return NOERROR;
}

ECode CFadeHelper::GetOUT(
    /* [out] */ Int32* _out)
{
    VALIDATE_NOT_NULL(_out)

    *_out = CFade::OUT;
    return NOERROR;
}

} // namespace Transition
} // namepsace Droid
} // namespace Elastos
