
#include "elastos/droid/internal/os/CSomeArgsHelper.h"
#include "elastos/droid/internal/os/SomeArgs.h"
#include "elastos/droid/ext/frameworkdef.h"

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

CAR_INTERFACE_IMPL(CSomeArgsHelper, Singleton, ISomeArgsHelper)

CAR_SINGLETON_IMPL(CSomeArgsHelper)

ECode CSomeArgsHelper::Obtain(
    /* [out] */ ISomeArgs** args)
{
    VALIDATE_NOT_NULL(args)
    AutoPtr<ISomeArgs> temp = SomeArgs::Obtain();
    *args = temp;
    REFCOUNT_ADD(*args)
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Internal
} // namespace Elastos
