#ifndef _ELASTOS_DROID_KEYSTORE_SECURITY_CREDENTIALS_H__
#define _ELASTOS_DROID_KEYSTORE_SECURITY_CREDENTIALS_H__

#include "Elastos.Droid.KeyStore.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Security::Cert::ICertificate;
using Elastos::Security::IKeyPair;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace KeyStore {
namespace Security {

/**
 * {@hide}
 */
class Credentials
    : public Object
    , public ICredentials
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor();
    /**
     * Convert objects to a PEM format which is used for
     * CA_CERTIFICATE and USER_CERTIFICATE entries.
     */
   static CARAPI ConvertToPem(
       /* [in] */ ArrayOf<ICertificate*>* objects,
       /* [out] */ ArrayOf<Byte>** result);

   /**
     * Convert objects from PEM format, which is used for
     * CA_CERTIFICATE and USER_CERTIFICATE entries.
     */
   static CARAPI ConvertFromPem(
       /* [in] */ ArrayOf<Byte>* bytes,
       /* [out] */ IList** result); // List<X509Certificate>


   static CARAPI GetInstance(
       /* [out] */ ICredentials** instance);

    /**
     * Delete all types (private key, certificate, CA certificate) for a
     * particular {@code alias}. All three can exist for any given alias.
     * Returns {@code true} if there was at least one of those types.
     */
   static CARAPI DeleteCertificateTypesForAlias(
       /* [in] */ IKeyStore* keystore,
       /* [in] */ const String& alias,
       /* [out] */ Boolean* result);

    /**
     * Delete all types (private key, certificate, CA certificate) for a
     * particular {@code alias}. All three can exist for any given alias.
     * Returns {@code true} if there was at least one of those types.
     */
   static CARAPI DeleteAllTypesForAlias(
       /* [in] */ IKeyStore* keystore,
       /* [in] */ const String& alias,
       /* [out] */ Boolean* ret);

   CARAPI Unlock(
        /* [in] */ IContext* context);

   CARAPI Install(
        /* [in] */ IContext* context);

   CARAPI Install(
        /* [in] */ IContext* context,
        /* [in] */ IKeyPair* pair);

   CARAPI Install(
        /* [in] */ IContext* context,
        /* [in] */ const String& type,
        /* [in] */ ArrayOf<Byte>* value);

private:
    static const String LOGTAG;// = "Credentials";
    static AutoPtr<ICredentials> singleton;

};

}// namespace Elastos
}// namespace Droid
}// namespace KeyStore
}// namespace Security

#endif // _ELASTOS_DROID_KEYSTORE_SECURITY_CREDENTIALS_H__
