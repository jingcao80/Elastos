
#include "CSecureRandomHelper.h"
#include "CSecureRandom.h"

namespace Elastos {
namespace Security {

CAR_SINGLETON_IMPL(CSecureRandomHelper)
CAR_INTERFACE_IMPL(CSecureRandomHelper, Singleton, ISecureRandomHelper)
ECode CSecureRandomHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException*/
{
    return CSecureRandom::GetInstance(algorithm, result);
}

ECode CSecureRandomHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ const String& provider,
    /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException, NoSuchProviderException*/
{
    return CSecureRandom::GetInstance(algorithm, provider, result);
}

ECode CSecureRandomHelper::GetInstance(
    /* [in] */ const String& algorithm,
    /* [in] */ IProvider* provider,
    /* [out] */ ISecureRandom** result) /*throws NoSuchAlgorithmException*/
{
    return CSecureRandom::GetInstance(algorithm, provider, result);
}

ECode CSecureRandomHelper::GetSeed(
    /* [in] */ Int32 numBytes,
    /* [out] */ ArrayOf<Byte>** seed)
{
    return CSecureRandom::GetSeed(numBytes, seed);
}

}
}
