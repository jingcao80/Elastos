
#include "elastos/droid/content/CMutableContextWrapper.h"

namespace Elastos {
namespace Droid {
namespace Content {

CAR_INTERFACE_IMPL(CMutableContextWrapper, ContextWrapper, IMutableContextWrapper)

CAR_OBJECT_IMPL(CMutableContextWrapper)

ECode CMutableContextWrapper::GetBaseContext(
    /* [out] */ IContext** context)
{
    return ContextWrapper::GetBaseContext(context);
}

ECode CMutableContextWrapper::SetBaseContext(
    /* [in] */ IContext* base)
{
    mBase = base;
    return NOERROR;
}

ECode CMutableContextWrapper::constructor(
    /* [in] */ IContext* base)
{
    return ContextWrapper::constructor(base);
}

}
}
}

