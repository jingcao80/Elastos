
#ifndef __ELASTOS_SECURITY_CCODESOURCE_H__
#define __ELASTOS_SECURITY_CCODESOURCE_H__

#include "_Elastos_Security_CCodeSource.h"
#include "Permission.h"

using Elastos::IO::ISerializable;
using Elastos::Net::IURL;

namespace Elastos {
namespace Security {

CarClass(CCodeSource)
    , public Object
    , public ICodeSource
    , public ISerializable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IURL* location,
        /* [in] */ ArrayOf<ICertificate*>* certs);

    CARAPI constructor(
        /* [in] */ IURL* location,
        /* [in] */ ArrayOf<ICodeSigner*>* signers);

    CARAPI GetCertificates(
        /* [out] */ ArrayOf<ICertificate*>** result);

    CARAPI GetCodeSigners(
        /* [out] */ ArrayOf<ICodeSigner*>** result);

    CARAPI GetLocation(
        /* [out] */ IURL** result);

    CARAPI Implies(
        /* [in] */ ICodeSource* cs,
        /* [out] */ Boolean* result);
};

}
}

#endif // __ELASTOS_SECURITY_CCODESOURCE_H__
