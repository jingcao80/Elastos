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
    VALIDATE_NOT_NULL(spec);
    return COAEPParameterSpec::New((IOAEPParameterSpec**)&spec);
}

} // Spec
} // Crypto
} // Elastosx