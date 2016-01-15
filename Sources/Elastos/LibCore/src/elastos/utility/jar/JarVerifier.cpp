#include "JarVerifier.h"
#include "core/AutoLock.h"
#include "CAttributes.h"
#include "CName.h"
#include "ManifestReader.h"
#include "io/CByteArrayInputStream.h"
#include "io/CBase64.h"
#include "security/CMessageDigestHelper.h"
#include "utility/etl/List.h"
#include "org/apache/harmony/security/utils/CJarUtils.h"

using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Libcore::IO::IBase64;
using Libcore::IO::CBase64;
using Elastos::Security::IMessageDigestHelper;
using Elastos::Security::CMessageDigestHelper;
using Elastos::Utility::Etl::List;
using Org::Apache::Harmony::Security::Utils::IJarUtils;
using Org::Apache::Harmony::Security::Utils::CJarUtils;

namespace Elastos {
namespace Utility {
namespace Jar {

AutoPtr<ArrayOf<String> > InitDIGEST_ALGORITHMS()
{
    AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(4);
    array->Set(0, String("SHA-512"));
    array->Set(1, String("SHA-384"));
    array->Set(2, String("SHA-256"));
    array->Set(3, String("SHA1"));
    return array;
}

const AutoPtr<ArrayOf<String> > JarVerifier::DIGEST_ALGORITHMS = InitDIGEST_ALGORITHMS();

//===========================================================
// JarVerifier::VerifierEntry
//===========================================================
JarVerifier::VerifierEntry::VerifierEntry(
    /* [in] */ const String& name,
    /* [in] */ IMessageDigest* digest,
    /* [in] */ ArrayOf<Byte>* hash,
    /* [in] */ ArrayOf<AutoPtr<CertificateArray > >* certificates,
    /* [in] */ HashMap<String, AutoPtr<ArrayOf<AutoPtr<CertificateArray> > > > * verifiedEntries,
    /* [in] */ JarVerifier* host)
    : mName(name)
    , mDigest(digest)
    , mHash(hash)
    , mCertificates(certificates)
    , mVerifiedEntries(verifiedEntries)
    , mHost(host)
{
}

ECode JarVerifier::VerifierEntry::Write(
    /* in */ Int32 value)
{
    return mDigest->Update((Byte)value);
}

ECode JarVerifier::VerifierEntry::Write(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    VALIDATE_NOT_NULL(buffer)
    return mDigest->Update(buffer, 0, buffer->GetLength());
}

ECode JarVerifier::VerifierEntry::Write(
    /* in */ ArrayOf<Byte>* buf,
    /* in */ Int32 off,
    /* in */ Int32 nbytes)
{
    return mDigest->Update(buf, off, nbytes);
}

ECode JarVerifier::VerifierEntry::Verify()
{
    AutoPtr<ArrayOf<Byte> > d;
    mDigest->Digest((ArrayOf<Byte>**)&d);
    AutoPtr<IMessageDigestHelper> helper;
    CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&helper);
    Boolean isEqual;
    AutoPtr<IBase64> bs64;
    CBase64::AcquireSingleton((IBase64**)&bs64);
    AutoPtr<ArrayOf<Byte> > rst;
    bs64->Decode(mHash, (ArrayOf<Byte>**)&rst);
    helper->IsEqual(d, rst, &isEqual);
    if (!isEqual) {
        ALOGE("%s has invalid digest for %s in %s",
            IJarFile::MANIFEST_NAME.string(), mName.string(), mName.string());
        return E_SECURITY_EXCEPTION;
    }
    (*mVerifiedEntries)[mName] = mCertificates;
    return NOERROR;
}

//===========================================================
// JarVerifier
//===========================================================
JarVerifier::JarVerifier(
    /* [in] */ const String& name,
    /* [in] */ IManifest* manifest,
    /* [in] */ HashMap<String, AutoPtr<ArrayOf<Byte> > >* metaEntries)
    : mMainAttributesEnd(0)
    , mJarName(name)
    , mManifest(manifest)
    , mMetaEntries(metaEntries)
{
    mSignatures = new HashMap<String, AutoPtr<StringAttributesMap> >(5);
    mCertificates = new StringCertificateMap(5);
    mVerifiedEntries = new HashMap<String, AutoPtr<ArrayOf<AutoPtr<CertificateArray> > > >();
}

JarVerifier::~JarVerifier()
{
    mVerifiedEntries = NULL;
    mCertificates = NULL;
    mMetaEntries = NULL;
    mSignatures = NULL;
}

ECode JarVerifier::InitEntry(
    /* in */ const String& name,
    /* out */ VerifierEntry** entry)
{
    VALIDATE_NOT_NULL(entry)
    *entry = NULL;

    // If no manifest is present by the time an entry is found,
    // verification cannot occur. If no signature files have
    // been found, do not verify.
    if (mManifest == NULL || mSignatures->IsEmpty()) {
        return NOERROR;
    }

    AutoPtr<IAttributes> attributes;
    mManifest->GetAttributes(name, (IAttributes**)&attributes);
    // entry has no digest
    if (attributes == NULL) {
        return NOERROR;
    }


    Etl::List<AutoPtr<CertificateArray> > certArrList;
    StringAttributesMapIterator sait;
    StringCertificateMapIterator scit;
    HashMap<String, AutoPtr<StringAttributesMap> >::Iterator it;
    for (it = mSignatures->Begin(); it != mSignatures->End(); ++it) {
        StringAttributesMap* hm = it->mSecond;
        sait = hm->Find(name);
        if (sait != hm->End()) {
            IAttributes * attr = sait->mSecond;
            if (attr != NULL) {
                // Found an entry for entry name in .SF file
                String signatureFile = it->mFirst;
                scit = mCertificates->Find(signatureFile);
                if (scit != mCertificates->End()) {
                    AutoPtr<ArrayOf<ICertificate*> > certArr = scit->mSecond;
                    if (certArr) {
                        certArrList.PushBack(certArr);
                    }
                }
            }
        }
    }

    // entry is not signed
    if (certArrList.IsEmpty()) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<AutoPtr<CertificateArray> > > certChainsArray;
    certChainsArray = ArrayOf<AutoPtr<CertificateArray> >::Alloc(certArrList.GetSize());
    Etl::List<AutoPtr<CertificateArray> >::Iterator cit;
    Int32 i = 0;
    for (cit = certArrList.Begin(); cit != certArrList.End(); ++cit) {
        certChainsArray->Set(i++, *cit);
    }

    for (i = 0; i < DIGEST_ALGORITHMS->GetLength(); ++i) {
        String algorithm = (*DIGEST_ALGORITHMS)[i];
        String hash;
        attributes->GetValue(algorithm + "-Digest", &hash);
        if (hash.IsNull()) {
            continue;
        }

        AutoPtr<IMessageDigestHelper> helper;
        CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&helper);
        AutoPtr<IMessageDigest> md;
        ECode ec = helper->GetInstance(algorithm, (IMessageDigest**)&md);
        if (ec == (ECode)E_NO_SUCH_ALGORITHM_EXCEPTION) {
            //ignore
        }
        if (FAILED(ec)) {
            return ec;
        }

        AutoPtr<ArrayOf<Byte> > hashBytes = hash.GetBytes();//hash.getBytes(StandardCharsets.ISO_8859_1);
        AutoPtr<VerifierEntry> ret = new VerifierEntry(name, md, hashBytes, certChainsArray, mVerifiedEntries, this);
        *entry = ret;
        REFCOUNT_ADD(*entry)
        return NOERROR;
    }

    return NOERROR;
}

ECode JarVerifier::AddMetaEntry(
    /* in */ const String& name,
    /* in */ ArrayOf<Byte>* buf)
{
    (*mMetaEntries)[name.ToUpperCase()] = buf;
    return NOERROR;
}

ECode JarVerifier::ReadCertificates(
    /* out */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    AutoLock lock(this);
    if (mMetaEntries == NULL || mMetaEntries->IsEmpty()) {
        return NOERROR;
    }

    String key;
    HashMap<String, AutoPtr<ArrayOf<Byte> > >::Iterator it = mMetaEntries->Begin();
    for (; it != mMetaEntries->End();) {
        key = it->mFirst;
        if (key.EndWith(".DSA") || key.EndWith(".RSA") || key.EndWith(".EC")) {
            FAIL_RETURN(VerifyCertificate(key))
            mMetaEntries->Erase(it++);
        }
        else {
            it++;
        }
    }

    *result = TRUE;
    return NOERROR;
}

ECode JarVerifier::VerifyCertificate(
    /* in */ const String& certFile)
{
    // Found Digital Sig, .SF should already have been read
    String signatureFile = certFile.Substring(0, certFile.LastIndexOf('.')) + ".SF";
    AutoPtr<ArrayOf<Byte> > manifestBytes;
    HashMap<String, AutoPtr<ArrayOf<Byte> > >::Iterator it = mMetaEntries->Find(signatureFile);
    if (it != mMetaEntries->End()) {
        manifestBytes = it->mSecond;
    }
    // Manifest entry is required for any verifications.
    if (manifestBytes == NULL) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > sBlockBytes;
    it = mMetaEntries->Find(certFile);
    if (it != mMetaEntries->End()) {
        sBlockBytes = it->mSecond;
    }

    AutoPtr<IJarUtils> jarUtils;
    CJarUtils::AcquireSingleton((IJarUtils**)&jarUtils);
    AutoPtr<IByteArrayInputStream> sfi, sbi;
    CByteArrayInputStream::New(manifestBytes, (IByteArrayInputStream**)&sfi);
    CByteArrayInputStream::New(sBlockBytes, (IByteArrayInputStream**)&sbi);
    AutoPtr<ArrayOf<ICertificate*> > signerCertChain;
    FAIL_RETURN(jarUtils->VerifySignature(
            IInputStream::Probe(sfi), IInputStream::Probe(sbi), (ArrayOf<ICertificate*>**)&signerCertChain))
    if (signerCertChain != NULL) {
        (*mCertificates)[signatureFile] = signerCertChain;
    }

    // Verify manifest hash in .sf file
    AutoPtr<IAttributes> attributes;
    FAIL_RETURN(CAttributes::New((IAttributes**)&attributes))

    AutoPtr<HashMap<String, AutoPtr<IAttributes> > > entries = new HashMap<String, AutoPtr<IAttributes> >();
    AutoPtr<ManifestReader> im = new ManifestReader(manifestBytes, attributes);
    FAIL_RETURN(im->ReadEntries(entries, NULL))

    // Do we actually have any signatures to look at?
    AutoPtr<IInterface> obj;
    IMap::Probe(attributes)->Get(CName::SIGNATURE_VERSION, (IInterface**)&obj);
    if (obj == NULL) {
        return NOERROR;
    }

    Boolean createdBySigntool = FALSE;
    String createdBy;
    attributes->GetValue(String("Created-By"), &createdBy);
    if (!createdBy.IsNull()) {
        createdBySigntool = createdBy.IndexOf("signtool") != -1;
    }

    // Use .SF to verify the mainAttributes of the manifest
    // If there is no -Digest-Manifest-Main-Attributes entry in .SF
    // file, such as those created before java 1.5, then we ignore
    // such verification.
    if (mMainAttributesEnd > 0 && !createdBySigntool) {
        String digestAttribute("-Digest-Manifest-Main-Attributes");
        Boolean bval;
        Verify(attributes, digestAttribute, manifestBytes, 0, mMainAttributesEnd, FALSE, TRUE, &bval);
        if (!bval) {
            ALOGE("%s failed verification of %s.", mJarName.string(), signatureFile.string());
            return E_SECURITY_EXCEPTION;
            // throw failedVerification(jarName, signatureFile);
        }
    }

    // Use .SF to verify the whole manifest.
    String digestAttribute = createdBySigntool ? String("-Digest") : String("-Digest-Manifest");
    Boolean bval;
    Verify(attributes, digestAttribute, manifestBytes, 0, manifestBytes->GetLength(), FALSE, FALSE, &bval);
    if (!bval) {
        CManifest* cm = (CManifest*)mManifest.Get();
        HashMap<String, AutoPtr<IAttributes> >::Iterator it = entries->Begin();
        for (; it != entries->End(); ++it) {
            AutoPtr<CManifest::Chunk> chunk = cm->GetChunk(it->mFirst);
            if (chunk == NULL) {
                return NOERROR;
            }

            Verify(it->mSecond, String("-Digest"), manifestBytes,
                    chunk->mStart, chunk->mEnd, createdBySigntool, FALSE, &bval);
            if (!bval) {
                ALOGE("%s has invalid digest for %s in %s.", signatureFile.string(),
                    it->mFirst.string(), mJarName.string());
                return E_SECURITY_EXCEPTION;
                // throw invalidDigest(signatureFile, entry.getKey(), jarName);
            }
        }
    }

    AutoPtr<ArrayOf<Byte> > barr;
    (*mMetaEntries)[signatureFile] = barr;
    (*mSignatures)[signatureFile] = entries;

    return NOERROR;
}

ECode JarVerifier::IsSignedJar(
    /* [out] */ Boolean* isSigned)
{
    VALIDATE_NOT_NULL(isSigned)
    *isSigned = mCertificates->GetSize() > 0;
    return NOERROR;
}

ECode JarVerifier::Verify(
    /* in */ IAttributes* attributes,
    /* in */ const String& entry,
    /* in */ ArrayOf<Byte>* data,
    /* in */ Int32 start,
    /* in */ Int32 end,
    /* in */ Boolean ignoreSecondEndline,
    /* in */ Boolean ignorable,
    /* out */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = ignorable;

    for (Int32 i = 0; i < DIGEST_ALGORITHMS->GetLength(); i++) {
        String algorithm = (*DIGEST_ALGORITHMS)[i];
        String hash;
        attributes->GetValue(algorithm + entry, &hash);
        if (hash.IsNull()) {
            continue;
        }

        AutoPtr<IMessageDigestHelper> helper;
        CMessageDigestHelper::AcquireSingleton((IMessageDigestHelper**)&helper);
        AutoPtr<IMessageDigest> md;
        ECode ec = helper->GetInstance(algorithm, (IMessageDigest**)&md);
        if (ec == (ECode)E_NO_SUCH_ALGORITHM_EXCEPTION) {
            //ignore
        }
        if (FAILED(ec)) {
            return ec;
        }

        if (ignoreSecondEndline && (*data)[end - 1] == '\n' && (*data)[end - 2] == '\n') {
            md->Update(data, start, end - 1 - start);
        }
        else {
            md->Update(data, start, end - start);
        }

        AutoPtr<ArrayOf<Byte> > b;
        md->Digest((ArrayOf<Byte>**)&b);

        AutoPtr<IBase64> bs64;
        CBase64::AcquireSingleton((IBase64**)&bs64);
        AutoPtr<ArrayOf<Byte> > hashBytes = hash.GetBytes();//hash.getBytes(StandardCharsets.ISO_8859_1);
        AutoPtr<ArrayOf<Byte> > rst;
        bs64->Decode(hashBytes, (ArrayOf<Byte>**)&rst);

        Boolean isEqual;
        helper->IsEqual(b, rst, &isEqual);
        *result = isEqual;
        return NOERROR;
    }

    *result = ignorable;
    return NOERROR;
}

AutoPtr<ArrayOf<AutoPtr<JarVerifier::CertificateArray> > > JarVerifier::GetCertificateChains(
    /* [in] */ const String& name)
{
    HashMap<String, AutoPtr<ArrayOf<AutoPtr<CertificateArray> > > >::Iterator it;
    it = mVerifiedEntries->Find(name);
    if (it != mVerifiedEntries->End()) {
        return it->mSecond;
    }
    return NULL;
}

ECode JarVerifier::RemoveMetaEntries()
{
    mMetaEntries->Clear();
    return NOERROR;
}

} // namespace Jar
} // namespace Utility
} // namespace Elastos
