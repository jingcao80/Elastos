#include "CMacHelper.h"
#include "CMac.h"

namespace Elastosx {
namespace Crypto {

CAR_INTERFACE_IMPL(CMacHelper, Singleton, IMacHelper)

CAR_SINGLETON_IMPL(CMacHelper)

ECode CMacHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ IMac ** mac)
{
    VALIDATE_NOT_NULL(mac);
    return CMac::GetInstance(algorithm, mac);
}

ECode CMacHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ IMac ** mac)
{
    VALIDATE_NOT_NULL(mac);
    return CMac::GetInstance(algorithm, provider, mac);
}

ECode CMacHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider * provider,
    /* [out] */ IMac ** mac)
{
    VALIDATE_NOT_NULL(mac);
    return CMac::GetInstance(algorithm, provider, mac);
}

} // Crypto
} // Elastosx