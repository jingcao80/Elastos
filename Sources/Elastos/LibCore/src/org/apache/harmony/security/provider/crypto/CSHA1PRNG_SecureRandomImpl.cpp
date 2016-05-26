
#include "CSHA1PRNG_SecureRandomImpl.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Provider {
namespace Crypto {

CAR_OBJECT_IMPL(CSHA1PRNG_SecureRandomImpl)

ECode CSHA1PRNG_SecureRandomImpl::EngineSetSeed(
    /* [in] */ ArrayOf<Byte> * pSeed)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1PRNG_SecureRandomImpl::EngineNextBytes(
    /* [in] */ ArrayOf<Byte> * pBytes)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CSHA1PRNG_SecureRandomImpl::EngineGenerateSeed(
    /* [in] */ Int32 numBytes,
    /* [out, callee] */ ArrayOf<Byte> ** ppSeed)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

}
}
}
}
}
}

