
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTEDCERTIFICATEKEYSTORESPI_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTEDCERTIFICATEKEYSTORESPI_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_CTrustedCertificateKeyStoreSpi.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(CTrustedCertificateKeyStoreSpi)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI EngineGetKey(
        /* [in] */ const String& alias,
        /* [in] */ ArrayOf<Char32> * pPassword,
        /* [out] */ Elastos::Security::IKey ** ppKey);

    CARAPI EngineGetCertificateChain(
        /* [in] */ const String& alias,
        /* [out, callee] */ ArrayOf<Elastos::Security::Cert::ICertificate *> ** ppCertChain);

    CARAPI EngineGetCertificate(
        /* [in] */ const String& alias,
        /* [out] */ Elastos::Security::Cert::ICertificate ** ppCert);

    CARAPI EngineGetCreationDate(
        /* [in] */ const String& alias,
        /* [out] */ Elastos::Utility::IDate ** ppCreationDate);

    CARAPI EngineSetKeyEntry(
        /* [in] */ const String& alias,
        /* [in] */ Elastos::Security::IKey * pKey,
        /* [in] */ ArrayOf<Char32> * pPassword,
        /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate *> * pChain);

    CARAPI EngineSetKeyEntryEx(
        /* [in] */ const String& alias,
        /* [in] */ ArrayOf<Byte> * pKey,
        /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate *> * pChain);

    CARAPI EngineSetCertificateEntry(
        /* [in] */ const String& alias,
        /* [in] */ Elastos::Security::Cert::ICertificate * pCert);

    CARAPI EngineDeleteEntry(
        /* [in] */ const String& alias);

    CARAPI EngineAliases(
        /* [out] */ Elastos::Utility::IEnumeration ** ppAliases);

    CARAPI EngineContainsAlias(
        /* [in] */ const String& alias,
        /* [out] */ Boolean * pResult);

    CARAPI EngineSize(
        /* [out] */ Int32 * pSize);

    CARAPI EngineIsKeyEntry(
        /* [in] */ const String& alias,
        /* [out] */ Boolean * pResult);

    CARAPI EngineIsCertificateEntry(
        /* [in] */ const String& alias,
        /* [out] */ Boolean * pResult);

    CARAPI EngineGetCertificateAlias(
        /* [in] */ Elastos::Security::Cert::ICertificate * pCert,
        /* [out] */ String * pAlias);

    CARAPI EngineStore(
        /* [in] */ Elastos::IO::IOutputStream * pStream,
        /* [in] */ ArrayOf<Char32> * pPassword);

    CARAPI EngineStoreEx(
        /* [in] */ Elastos::Security::IKeyStoreLoadStoreParameter * pParam);

    CARAPI EngineLoad(
        /* [in] */ Elastos::IO::IInputStream * pStream,
        /* [in] */ ArrayOf<Char32> * pPassword);

    CARAPI EngineLoadEx(
        /* [in] */ Elastos::Security::IKeyStoreLoadStoreParameter * pParam);

    CARAPI EngineGetEntry(
        /* [in] */ const String& alias,
        /* [in] */ Elastos::Security::IKeyStoreProtectionParameter * pProtParam,
        /* [out] */ Elastos::Security::IKeyStoreEntry ** ppEntry);

    CARAPI EngineSetEntry(
        /* [in] */ const String& alias,
        /* [in] */ Elastos::Security::IKeyStoreEntry * pEntry,
        /* [in] */ Elastos::Security::IKeyStoreProtectionParameter * pProtParam);

    CARAPI EngineEntryInstanceOf(
        /* [in] */ const String& alias,
        /* [in] */ const ClassID & entryClass,
        /* [out] */ Boolean * pResult);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_CTRUSTEDCERTIFICATEKEYSTORESPI_H__
