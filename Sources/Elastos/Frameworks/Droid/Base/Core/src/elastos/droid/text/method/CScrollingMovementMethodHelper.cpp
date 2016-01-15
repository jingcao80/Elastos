#include "elastos/droid/text/method/CScrollingMovementMethodHelper.h"
#include "elastos/droid/text/method/CScrollingMovementMethod.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CScrollingMovementMethodHelper)

CAR_INTERFACE_IMPL(CScrollingMovementMethodHelper, Singleton, IScrollingMovementMethodHelper)

ECode CScrollingMovementMethodHelper::GetInstance(
    /* [out] */ IMovementMethod** ret)
{
    return ScrollingMovementMethod::GetInstance(ret);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

