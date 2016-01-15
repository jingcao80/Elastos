
#include "CTrustedCertificateKeyStoreSpi.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {


ECode CTrustedCertificateKeyStoreSpi::EngineGetKey(
    /* [in] */ const String& alias,
    /* [in] */ ArrayOf<Char32> * pPassword,
    /* [out] */ Elastos::Security::IKey ** ppKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineGetCertificateChain(
    /* [in] */ const String& alias,
    /* [out, callee] */ ArrayOf<Elastos::Security::Cert::ICertificate *> ** ppCertChain)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineGetCertificate(
    /* [in] */ const String& alias,
    /* [out] */ Elastos::Security::Cert::ICertificate ** ppCert)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineGetCreationDate(
    /* [in] */ const String& alias,
    /* [out] */ Elastos::Utility::IDate ** ppCreationDate)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineSetKeyEntry(
    /* [in] */ const String& alias,
    /* [in] */ Elastos::Security::IKey * pKey,
    /* [in] */ ArrayOf<Char32> * pPassword,
    /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate *> * pChain)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineSetKeyEntryEx(
    /* [in] */ const String& alias,
    /* [in] */ ArrayOf<Byte> * pKey,
    /* [in] */ ArrayOf<Elastos::Security::Cert::ICertificate *> * pChain)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineSetCertificateEntry(
    /* [in] */ const String& alias,
    /* [in] */ Elastos::Security::Cert::ICertificate * pCert)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineDeleteEntry(
    /* [in] */ const String& alias)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineAliases(
    /* [out] */ Elastos::Utility::IEnumeration ** ppAliases)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineContainsAlias(
    /* [in] */ const String& alias,
    /* [out] */ Boolean * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineSize(
    /* [out] */ Int32 * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineIsKeyEntry(
    /* [in] */ const String& alias,
    /* [out] */ Boolean * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineIsCertificateEntry(
    /* [in] */ const String& alias,
    /* [out] */ Boolean * pResult)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineGetCertificateAlias(
    /* [in] */ Elastos::Security::Cert::ICertificate * pCert,
    /* [out] */ String * pAlias)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineStore(
    /* [in] */ Elastos::IO::IOutputStream * pStream,
    /* [in] */ ArrayOf<Char32> * pPassword)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineStoreEx(
    /* [in] */ Elastos::Security::IKeyStoreLoadStoreParameter * pParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineLoad(
    /* [in] */ Elastos::IO::IInputStream * pStream,
    /* [in] */ ArrayOf<Char32> * pPassword)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineLoadEx(
    /* [in] */ Elastos::Security::IKeyStoreLoadStoreParameter * pParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineGetEntry(
    /* [in] */ const String& alias,
    /* [in] */ Elastos::Security::IKeyStoreProtectionParameter * pProtParam,
    /* [out] */ Elastos::Security::IKeyStoreEntry ** ppEntry)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineSetEntry(
    /* [in] */ const String& alias,
    /* [in] */ Elastos::Security::IKeyStoreEntry * pEntry,
    /* [in] */ Elastos::Security::IKeyStoreProtectionParameter * pProtParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode CTrustedCertificateKeyStoreSpi::EngineEntryInstanceOf(
    /* [in] */ const String& alias,
    /* [in] */ const ClassID & entryClass,
    /* [out] */ Boolean * pResult)
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

