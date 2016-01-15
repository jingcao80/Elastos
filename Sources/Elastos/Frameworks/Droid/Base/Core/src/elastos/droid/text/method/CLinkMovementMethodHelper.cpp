#include "elastos/droid/text/method/CLinkMovementMethodHelper.h"
#include "elastos/droid/text/method/LinkMovementMethod.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CLinkMovementMethodHelper)

CAR_INTERFACE_IMPL(CLinkMovementMethodHelper, Singleton, ILinkMovementMethodHelper)

ECode CLinkMovementMethodHelper::GetInstance(
    /* [out] */ IMovementMethod** ret)
{
    return LinkMovementMethod::GetInstance(ret);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

