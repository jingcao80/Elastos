
#include "elastos/droid/view/CMagnificationSpec.h"
#include "elastos/droid/view/CMagnificationSpecHelper.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(CMagnificationSpecHelper, Singleton, IMagnificationSpecHelper)

CAR_SINGLETON_IMPL(CMagnificationSpecHelper)

ECode CMagnificationSpecHelper::Obtain(
    /* [in] */ IMagnificationSpec* other,
    /* [out] */ IMagnificationSpec** spec)
{
    VALIDATE_NOT_NULL(spec)
    *spec = CMagnificationSpec::Obtain(other);
    REFCOUNT_ADD(*spec)
    return NOERROR;
}

ECode CMagnificationSpecHelper::Obtain(
    /* [out] */ IMagnificationSpec** spec)
{
    VALIDATE_NOT_NULL(spec)
    *spec = CMagnificationSpec::Obtain();
    REFCOUNT_ADD(*spec)
    return NOERROR;
}

}   //namespace View
}   //namespace Droid
}   //namespace Elastos
