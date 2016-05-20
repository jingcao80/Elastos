
#ifndef __ELASTOS_SECURITY_CKEYSTOREPRIVATEKEYENTRY_H__
#define __ELASTOS_SECURITY_CKEYSTOREPRIVATEKEYENTRY_H__

#include "_Elastos_Security_CKeyStorePrivateKeyEntry.h"

namespace Elastos {
namespace Security {

CarClass(CKeyStorePrivateKeyEntry)
    , public Object
    , public IKeyStorePrivateKeyEntry
    , public IKeyStoreEntry
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetPrivateKey(
        /* [out] */ IPrivateKey **privateKey);

    CARAPI GetCertificateChain(
        /* [out, callee] */ ArrayOf<Elastos::Security::Cert::ICertificate*> **cc);

    CARAPI GetCertificate(
        /* [out] */ Elastos::Security::Cert::ICertificate **cert);

    CARAPI ToString(
        /* [out] */ String *str);

    CARAPI constructor(
        /* [in] */ IPrivateKey *privateKey,
        /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate*> *chain);

private:
    // Store Certificate chain
    AutoPtr<ArrayOf<Elastos::Security::Cert::ICertificate*> > mChain;

    // Store PrivateKey
    AutoPtr<IPrivateKey> mPrivateKey;

};

}
}

#endif // __ELASTOS_SECURITY_CKEYSTOREPRIVATEKEYENTRY_H__
