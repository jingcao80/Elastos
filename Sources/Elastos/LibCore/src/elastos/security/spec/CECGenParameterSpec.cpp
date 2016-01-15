
#include "CECGenParameterSpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

CAR_OBJECT_IMPL(CECGenParameterSpec);
CAR_INTERFACE_IMPL_2(CECGenParameterSpec, Object, IECGenParameterSpec, IAlgorithmParameterSpec);
ECode CECGenParameterSpec::GetName(
    /* [out] */ String *name)
{
    VALIDATE_NOT_NULL(name)
    *name = mName;
    return NOERROR;
}

ECode CECGenParameterSpec::constructor(
    /* [in] */ const String& name)
{
    mName = name;
    if (mName.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

}
}
}
