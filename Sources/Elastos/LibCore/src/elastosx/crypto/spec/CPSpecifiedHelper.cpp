#include "CPSpecifiedHelper.h"
#include "CPSpecified.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_INTERFACE_IMPL(CPSpecifiedHelper, Singleton, IPSpecifiedHelper)

CAR_SINGLETON_IMPL(CPSpecifiedHelper)

ECode CPSpecifiedHelper::GetDEFAULT(
    /* [out] */ IPSpecified** result)
{
    VALIDATE_NOT_NULL(result);
    return CPSpecified::New(result);
}

} // Spec
} // Crypto
} // Elastosx