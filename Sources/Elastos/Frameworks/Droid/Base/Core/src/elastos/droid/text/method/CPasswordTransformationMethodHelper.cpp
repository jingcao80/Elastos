#include "elastos/droid/text/method/CPasswordTransformationMethodHelper.h"
#include "elastos/droid/text/method/CPasswordTransformationMethod.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CPasswordTransformationMethodHelper)

CAR_INTERFACE_IMPL(CPasswordTransformationMethodHelper, Singleton, IPasswordTransformationMethodHelper)

ECode CPasswordTransformationMethodHelper::GetInstance(
    /* [out] */ IPasswordTransformationMethod** ret)
{
    return CPasswordTransformationMethod::GetInstance(ret);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

