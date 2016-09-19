
#include "CMGF1ParameterSpec.h"

namespace Elastos {
namespace Security {
namespace Spec {

const AutoPtr<IMGF1ParameterSpec> CMGF1ParameterSpec::SHA1 = CMGF1ParameterSpec::InitStatic(String("SHA-1"));

const AutoPtr<IMGF1ParameterSpec> CMGF1ParameterSpec::SHA256 = CMGF1ParameterSpec::InitStatic(String("SHA-256"));

const AutoPtr<IMGF1ParameterSpec> CMGF1ParameterSpec::SHA384 = CMGF1ParameterSpec::InitStatic(String("SHA-384"));

const AutoPtr<IMGF1ParameterSpec> CMGF1ParameterSpec::SHA512 = CMGF1ParameterSpec::InitStatic(String("SHA-512"));

AutoPtr<IMGF1ParameterSpec> CMGF1ParameterSpec::InitStatic(
    /* [in] */ const String& type)
{
    AutoPtr<CMGF1ParameterSpec> tmp, obj;
    CMGF1ParameterSpec::NewByFriend(type, (CMGF1ParameterSpec**)&tmp);
    return tmp;
}

CAR_OBJECT_IMPL(CMGF1ParameterSpec)
CAR_INTERFACE_IMPL_2(CMGF1ParameterSpec, Object, IMGF1ParameterSpec, IAlgorithmParameterSpec)
ECode CMGF1ParameterSpec::GetDigestAlgorithm(
    /* [out] */ String *algorithm)
{
    VALIDATE_NOT_NULL(algorithm)
    *algorithm = mMdName;
    return NOERROR;
}

ECode CMGF1ParameterSpec::constructor(
    /* [in] */ const String& mdName)
{
    mMdName = mdName;
    if (mMdName.IsNull()) {
        return E_NULL_POINTER_EXCEPTION;
    }
    return NOERROR;
}

}
}
}

