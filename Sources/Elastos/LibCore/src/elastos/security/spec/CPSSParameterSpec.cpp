
#include "CPSSParameterSpec.h"
#include "CMGF1ParameterSpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

const AutoPtr<IPSSParameterSpec> CPSSParameterSpec::DEFAULT = CPSSParameterSpec::InitStatic();

const AutoPtr<IPSSParameterSpec> CPSSParameterSpec::InitStatic()
{
    AutoPtr<CPSSParameterSpec> tmp;
    CPSSParameterSpec::NewByFriend(20, (CPSSParameterSpec**)&tmp);
    return tmp;
}

CAR_OBJECT_IMPL(CPSSParameterSpec);
CAR_INTERFACE_IMPL_2(CPSSParameterSpec, Object, IPSSParameterSpec, IAlgorithmParameterSpec);
ECode CPSSParameterSpec::GetSaltLength(
    /* [out] */ Int32 *saltLength)
{
    VALIDATE_NOT_NULL(saltLength)
    *saltLength = mSaltLen;
    return NOERROR;
}

ECode CPSSParameterSpec::GetDigestAlgorithm(
    /* [out] */ String *digestAlgo)
{
    VALIDATE_NOT_NULL(digestAlgo)
    *digestAlgo = mMdName;
    return NOERROR;
}

ECode CPSSParameterSpec::GetMGFAlgorithm(
    /* [out] */ String *mgfaAlgo)
{
    VALIDATE_NOT_NULL(mgfaAlgo)
    *mgfaAlgo = mMgfName;
    return NOERROR;
}

ECode CPSSParameterSpec::GetMGFParameters(
    /* [out] */ IAlgorithmParameterSpec **mgfParams)
{
    VALIDATE_NOT_NULL(mgfParams)
    *mgfParams = mMgfSpec;
    REFCOUNT_ADD(*mgfParams)
    return NOERROR;
}

ECode CPSSParameterSpec::GetTrailerField(
    /* [out] */ Int32 *trailerField)
{
    VALIDATE_NOT_NULL(trailerField)
    *trailerField = mTrailerField;
    return NOERROR;
}

ECode CPSSParameterSpec::constructor(
    /* [in] */ Int32 saltLen)
{
    if (saltLen < 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mSaltLen = saltLen;
    mMdName = String("SHA-1");
    mMgfName = String("MGF1");
    mMgfSpec = CMGF1ParameterSpec::SHA1.Get();
    mTrailerField = 1;
    return NOERROR;
}

ECode CPSSParameterSpec::constructor(
    /* [in] */ const String& mdName,
    /* [in] */ const String& mgfName,
    /* [in] */ IAlgorithmParameterSpec *mgfSpec,
    /* [in] */ Int32 saltLen,
    /* [in] */ Int32 trailerField)
{
    if (mdName.IsNull()) {
        //throw new NullPointerException("mdName == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (mgfName.IsNull()) {
        //throw new NullPointerException("mgfName == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (saltLen < 0) {
        //throw new IllegalArgumentException("saltLen < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (trailerField < 0) {
        //throw new IllegalArgumentException("trailerField < 0");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mMdName = mdName;
    mMgfName = mgfName;
    mMgfSpec = mgfSpec;
    mSaltLen = saltLen;
    mTrailerField = trailerField;
    return NOERROR;
}

}
}
}
