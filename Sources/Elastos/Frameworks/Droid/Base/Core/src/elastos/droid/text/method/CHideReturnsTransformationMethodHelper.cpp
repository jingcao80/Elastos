#include "elastos/droid/text/method/CHideReturnsTransformationMethodHelper.h"
#include "elastos/droid/text/method/HideReturnsTransformationMethod.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_SINGLETON_IMPL(CHideReturnsTransformationMethodHelper)

CAR_INTERFACE_IMPL(CHideReturnsTransformationMethodHelper, Singleton, IHideReturnsTransformationMethodHelper)

ECode CHideReturnsTransformationMethodHelper::GetInstance(
    /* [out] */ IHideReturnsTransformationMethod** ret)
{
   return HideReturnsTransformationMethod::GetInstance(ret);
}


} //namespace Method
} //namespace Text
} //namespace Droid
} //namespace Elastos
