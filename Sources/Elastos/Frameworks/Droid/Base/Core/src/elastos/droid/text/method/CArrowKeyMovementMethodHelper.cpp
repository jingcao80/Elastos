#include "elastos/droid/text/method/CArrowKeyMovementMethodHelper.h"
#include "elastos/droid/text/method/ArrowKeyMovementMethod.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CArrowKeyMovementMethodHelper)

CAR_INTERFACE_IMPL(CArrowKeyMovementMethodHelper, Singleton, IArrowKeyMovementMethodHelper)

ECode CArrowKeyMovementMethodHelper::GetInstance(
    /* [out] */ IMovementMethod** ret)
{
    return ArrowKeyMovementMethod::GetInstance(ret);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

