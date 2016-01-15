#include "PSource.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_INTERFACE_IMPL(PSource, Object, IPSource)

PSource::PSource()
    : mPSrcName(String(NULL))
{
}

ECode PSource::constructor(
    /* [in] */ const String& pSrcName)
{
    if (pSrcName == NULL) {
        // throw new NullPointerException("pSrcName == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    mPSrcName = pSrcName;
    return NOERROR;
}

ECode PSource::GetAlgorithm(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPSrcName;
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx