
#include "CCodeSource.h"

using Elastos::IO::EIID_ISerializable;

namespace Elastos {
namespace Security {

CAR_OBJECT_IMPL(CCodeSource)
CAR_INTERFACE_IMPL_2(CCodeSource, Object, ICodeSource, ISerializable)
ECode CCodeSource::constructor(
    /* [in] */ IURL* location,
    /* [in] */ ArrayOf<ICertificate*>* certs)
{
    return NOERROR;
}

ECode CCodeSource::constructor(
    /* [in] */ IURL* location,
    /* [in] */ ArrayOf<ICodeSigner*>* signers)
{
    return NOERROR;
}

ECode CCodeSource::GetCertificates(
    /* [out] */ ArrayOf<ICertificate*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode CCodeSource::GetCodeSigners(
    /* [out] */ ArrayOf<ICodeSigner*>** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode CCodeSource::GetLocation(
    /* [out] */ IURL** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    return NOERROR;
}

ECode CCodeSource::Implies(
    /* [in] */ ICodeSource* cs,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = TRUE;
    return NOERROR;
}

}
}
