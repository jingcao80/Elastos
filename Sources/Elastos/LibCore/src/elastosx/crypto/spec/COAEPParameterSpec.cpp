#include "COAEPParameterSpec.h"
#include "CPSpecifiedHelper.h"
//TODO: Need CMGF1ParameterSpec.h
// #include "CMGF1ParameterSpec.h"

namespace Elastosx {
namespace Crypto {
namespace Spec {

CAR_OBJECT_IMPL(COAEPParameterSpec)
CAR_INTERFACE_IMPL_2(COAEPParameterSpec, Object, IOAEPParameterSpec, IAlgorithmParameterSpec)

COAEPParameterSpec::COAEPParameterSpec()
    : mMdName(String(NULL))
    , mMgfName(String(NULL))
{
}

ECode COAEPParameterSpec::constructor()
{
    mMdName = String("SHA-1");
    mMgfName = String("MGF1");
//TODO: Need CMGF1ParameterSpec.h
    // mMgfSpec = Elastos::Security::Spec::CMGF1ParameterSpec::SHA1;
    AutoPtr<IPSpecifiedHelper> helper;
    CPSpecifiedHelper::AcquireSingleton((IPSpecifiedHelper**)&helper);
    AutoPtr<IPSpecified> result;
    helper->GetDEFAULT((IPSpecified**)&result);
    mPSrc = IPSource::Probe(result);
    return NOERROR;
}

ECode COAEPParameterSpec::constructor(
    /* [in] */ const String& mdName,
    /* [in] */ const String& mgfName,
    /* [in] */ IAlgorithmParameterSpec * mgfSpec,
    /* [in] */ IPSource * pSrc)
{
    if (mdName == NULL) {
        // throw new NullPointerException("mdName == NULL");
        return E_NULL_POINTER_EXCEPTION;
    } else if (mgfName == NULL) {
        // throw new NullPointerException("mgfName == NULL");
        return E_NULL_POINTER_EXCEPTION;
    } else if (pSrc == NULL) {
        // throw new NullPointerException("pSrc == NULL");
        return E_NULL_POINTER_EXCEPTION;
    }
    mMdName = mdName;
    mMgfName = mgfName;
    mMgfSpec = mgfSpec;
    mPSrc = pSrc;
    return NOERROR;
}

ECode COAEPParameterSpec::GetDigestAlgorithm(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMdName;
    return NOERROR;
}

ECode COAEPParameterSpec::GetMGFAlgorithm(
    /* [out] */ String * result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMgfName;
    return NOERROR;
}

ECode COAEPParameterSpec::GetMGFParameters(
    /* [out] */ IAlgorithmParameterSpec ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMgfSpec;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode COAEPParameterSpec::GetPSource(
    /* [out] */ IPSource ** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPSrc;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

} // Spec
} // Crypto
} // Elastosx