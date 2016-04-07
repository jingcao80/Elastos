#include "COAEPParameterSpecHelper.h"
#include "COAEPParameterSpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_INTERFACE_IMPL(COAEPParameterSpecHelper, Singleton, IOAEPParameterSpecHelper)

CAR_SINGLETON_IMPL(COAEPParameterSpecHelper)

ECode COAEPParameterSpecHelper::GetDEFAULT(
    /* [out] */ IOAEPParameterSpec ** spec)
{
    return COAEPParameterSpec::New(spec);
}

} // Spec
} // Crypto
} // Elastosx