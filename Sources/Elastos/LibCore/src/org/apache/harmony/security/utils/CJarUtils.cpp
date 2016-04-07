
#include "Elastos.CoreLibrary.Extensions.h"
#include "CJarUtils.h"
#include "io/CByteArrayInputStream.h"
#include "security/MessageDigest.h"
#include "security/Signature.h"
#include "security/cert/CertificateFactory.h"
#include "utility/Arrays.h"
#include "utility/CArrayList.h"
#include "utility/logging/Logger.h"
#include "asn1/CASN1OctetString.h"
#include "asn1/CBerInputStream.h"
#include "pkcs7/CContentInfoHelper.h"

using Elastos::Core::IByte;
using Elastos::Math::IBigInteger;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Security::IMessageDigest;
using Elastos::Security::ISignature;
using Elastos::Security::MessageDigest;
using Elastos::Security::Signature;
using Elastos::Security::Cert::ICertificateFactory;
using Elastos::Security::Cert::CertificateFactory;
using Elastos::Security::Cert::IX509Extension;
using Elastos::Utility::Arrays;
using Elastos::Utility::ICollection;
using Elastos::Utility::IList;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Elastosx::Security::Auth::X500::IX500Principal;
using Org::Apache::Harmony::Security::Asn1::IASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::CASN1OctetString;
using Org::Apache::Harmony::Security::Asn1::IASN1Sequence;
using Org::Apache::Harmony::Security::Asn1::IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IBerInputStream;
using Org::Apache::Harmony::Security::Asn1::CBerInputStream;
using Org::Apache::Harmony::Security::Pkcs7::IContentInfo;
using Org::Apache::Harmony::Security::Pkcs7::IContentInfoHelper;
using Org::Apache::Harmony::Security::Pkcs7::CContentInfoHelper;
using Org::Apache::Harmony::Security::Pkcs7::ISignedData;
using Org::Apache::Harmony::Security::Pkcs7::ISignerInfo;
using Org::Apache::Harmony::Security::X501::IAttributeTypeAndValue;
using Org::Apache::Harmony::Security::X501::IAttributeValue;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace Utils {

//------------------------------------------------------------
//  CJarUtils
//------------------------------------------------------------
const AutoPtr< ArrayOf<Int32> > CJarUtils::MESSAGE_DIGEST_OID = CJarUtils::Init_MESSAGE_DIGEST_OID();

CAR_INTERFACE_IMPL(CJarUtils, Singleton, IJarUtils)

CAR_SINGLETON_IMPL(CJarUtils)

ECode CJarUtils::VerifySignature(
    /* [in] */ IInputStream* signature,
    /* [in] */ IInputStream* signatureBlock,
    /* [out, callee] */ ArrayOf<ICertificate*>** sign)
{
    VALIDATE_NOT_NULL(sign);
    *sign = NULL;
    AutoPtr<IBerInputStream> bis;
    FAIL_RETURN(CBerInputStream::New(signatureBlock, (IBerInputStream**)&bis));
    AutoPtr<IContentInfoHelper> helper;
    CContentInfoHelper::AcquireSingleton((IContentInfoHelper**)&helper);
    AutoPtr<IASN1Sequence> asn1;
    helper->GetASN1((IASN1Sequence**)&asn1);
    AutoPtr<IInterface> obj;
    IASN1Type::Probe(asn1)->Decode(bis, (IInterface**)&obj);
    IContentInfo* info = IContentInfo::Probe(obj);
    AutoPtr<ISignedData> signedData;
    info->GetSignedData((ISignedData**)&signedData);
    if (signedData == NULL) {
        Logger::E("CJarUtils", "No SignedData found");
        return E_IO_EXCEPTION;
    }
    AutoPtr<IList> certList;
    signedData->GetCertificates((IList**)&certList);
    ICollection* encCerts = ICollection::Probe(certList);
    Boolean isEmpty;
    if (encCerts->IsEmpty(&isEmpty), isEmpty) {
        return NOERROR;
    }
    Int32 size;
    encCerts->GetSize(&size);
    AutoPtr< ArrayOf<IX509Certificate*> > certs = ArrayOf<IX509Certificate*>::Alloc(size);
    AutoPtr<ICertificateFactory> cf;
    CertificateFactory::GetInstance(String("X.509"), (ICertificateFactory**)&cf);
    Int32 i = 0;
    AutoPtr<IIterator> it;
    encCerts->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> certObj;
        it->GetNext((IInterface**)&certObj);
        Org::Apache::Harmony::Security::X509::ICertificate* encCert =
                Org::Apache::Harmony::Security::X509::ICertificate::Probe(certObj);
        AutoPtr< ArrayOf<Byte> > encoded;
        encCert->GetEncoded((ArrayOf<Byte>**)&encoded);
        AutoPtr<IInputStream> is;
        CByteArrayInputStream::New(encoded, (IInputStream**)&is);
        AutoPtr<ICertificate> cert;
        cf->GenerateCertificate(is, (ICertificate**)&cert);
        certs->Set(i++, IX509Certificate::Probe(cert));
    }

    AutoPtr<IList> sigInfos;
    signedData->GetSignerInfos((IList**)&sigInfos);
    AutoPtr<ISignerInfo> sigInfo;
    if (sigInfos->IsEmpty(&isEmpty), !isEmpty) {
        AutoPtr<IInterface> obj;
        sigInfos->Get(0, (IInterface**)&obj);
        sigInfo = ISignerInfo::Probe(obj);
    }
    else {
        *sign = NULL;
        return NOERROR;
    }

    // Issuer
    AutoPtr<IX500Principal> issuer;
    sigInfo->GetIssuer((IX500Principal**)&issuer);

    // Certificate serial number
    AutoPtr<IBigInteger> snum;
    sigInfo->GetSerialNumber((IBigInteger**)&snum);

    // Locate the certificate
    Int32 issuerSertIndex = 0;
    for (i = 0; i < certs->GetLength(); i++) {
        AutoPtr<IPrincipal> issuerDN;
        (*certs)[i]->GetIssuerDN((IPrincipal**)&issuerDN);
        AutoPtr<IBigInteger> number;
        (*certs)[i]->GetSerialNumber((IBigInteger**)&number);
        Boolean isEqual;
        if ((IObject::Probe(issuer)->Equals(issuerDN, &isEqual), isEqual) &&
                (snum->Equals(number, &isEqual), isEqual)) {
            issuerSertIndex = i;
            break;
        }
    }
    if (i == certs->GetLength()) { // No issuer certificate found
        *sign = NULL;
        return NOERROR;
    }

    Boolean result;
    if (IX509Extension::Probe((*certs)[issuerSertIndex])->HasUnsupportedCriticalExtension(&result), result) {
        Logger::E("CJarUtils", "Can not recognize a critical extension");
        return E_SECURITY_EXCEPTION;
    }

    // Get Signature instance
    String daOid;
    sigInfo->GetDigestAlgorithm(&daOid);
    String daName;
    sigInfo->GetDigestAlgorithmName(&daName);
    String deaOid;
    sigInfo->GetDigestEncryptionAlgorithm(&deaOid);

    String alg;
    AutoPtr<ISignature> sig;

    if (!deaOid.IsNull()) {
        alg = deaOid;
        Signature::GetInstance(alg, (ISignature**)&sig);

        String deaName;
        sigInfo->GetDigestEncryptionAlgorithmName(&deaName);
        if (sig == NULL && !deaName.IsNull()) {
            alg = deaName;
            Signature::GetInstance(alg, (ISignature**)&sig);
        }
    }

    if (sig == NULL && !daOid.IsNull() && !deaOid.IsNull()) {
        alg = daOid + "with" + deaOid;
        Signature::GetInstance(alg, (ISignature**)&sig);

        // Try to convert to names instead of OID.
        if (sig == NULL) {
            String deaName;
            sigInfo->GetDigestEncryptionAlgorithmName(&deaName);
            alg = daName + "with" + deaName;
            Signature::GetInstance(alg, (ISignature**)&sig);
        }
    }

    // We couldn't find a valid Signature type.
    if (sig == NULL) {
        *sign = NULL;
        return NOERROR;
    }

    FAIL_RETURN(sig->InitVerify(ICertificate::Probe((*certs)[issuerSertIndex])));

    // If the authenticatedAttributes field of SignerInfo contains more than zero attributes,
    // compute the message digest on the ASN.1 DER encoding of the Attributes value.
    // Otherwise, compute the message digest on the data.
    AutoPtr<IList> atr;
    sigInfo->GetAuthenticatedAttributes((IList**)&atr);

    signature->Available(&size);
    AutoPtr< ArrayOf<Byte> > sfBytes = ArrayOf<Byte>::Alloc(size);
    signature->Read(sfBytes, &size);

    if (atr == NULL) {
        sig->Update(sfBytes);
    }
    else {
        AutoPtr< ArrayOf<Byte> > attributes;
        sigInfo->GetEncodedAuthenticatedAttributes((ArrayOf<Byte>**)&attributes);
        sig->Update(attributes);

        // If the authenticatedAttributes field contains the message-digest attribute,
        // verify that it equals the computed digest of the signature file
        AutoPtr< ArrayOf<Byte> > existingDigest;
        AutoPtr<IIterator> it;
        atr->GetIterator((IIterator**)&it);
        Boolean hasNext;
        while (it->HasNext(&hasNext), hasNext) {
            AutoPtr<IInterface> item;
            it->GetNext((IInterface**)&item);
            AutoPtr<IAttributeTypeAndValue> a = IAttributeTypeAndValue::Probe(item);
            AutoPtr<IObjectIdentifier> oi;
            a->GetType((IObjectIdentifier**)&oi);
            AutoPtr< ArrayOf<Int32> > oid;
            oi->GetOid((ArrayOf<Int32>**)&oid);
            if (Arrays::Equals(oid, MESSAGE_DIGEST_OID)) {
                if (existingDigest != NULL) {
                    Logger::E("CJarUtils", "Too many MessageDigest attributes");
                    return E_SECURITY_EXCEPTION;
                }
                AutoPtr<IAttributeValue> value;
                a->GetValue((IAttributeValue**)&value);
                AutoPtr<IASN1OctetString> octect;
                CASN1OctetString::GetInstance((IASN1OctetString**)&octect);
                AutoPtr<ICollection> entries;
                value->GetValues(IASN1Type::Probe(octect), (ICollection**)&entries);
                Int32 size;
                if (entries->GetSize(&size), size != 1) {
                    Logger::E("CJarUtils", "Too many values for MessageDigest attribute");
                    return E_SECURITY_EXCEPTION;
                }
                AutoPtr<IIterator> entriesIt;
                entries->GetIterator((IIterator**)&entriesIt);
                AutoPtr<IInterface> obj;
                entriesIt->GetNext((IInterface**)&obj);
                AutoPtr<IArrayOf> arrayObj = IArrayOf::Probe(obj);
                arrayObj->GetLength(&size);
                existingDigest = ArrayOf<Byte>::Alloc(size);
                for (Int32 i = 0; i < size; i++) {
                    AutoPtr<IInterface> byteObj;
                    arrayObj->Get(i, (IInterface**)&byteObj);
                    Byte value;
                    IByte::Probe(byteObj)->GetValue(&value);
                    (*existingDigest)[i] = value;
                }
            }
        }

        // RFC 3852 section 9.2: it authAttrs is present, it must have a
        // message digest entry.
        if (existingDigest == NULL) {
            Logger::E("CJarUtils", "Missing MessageDigest in Authenticated Attributes");
            return E_SECURITY_EXCEPTION;
        }

        AutoPtr<IMessageDigest> md;
        if (!daOid.IsNull()) {
            MessageDigest::GetInstance(daOid, (IMessageDigest**)&md);
        }
        if (md == NULL && !daName.IsNull()) {
            MessageDigest::GetInstance(daName, (IMessageDigest**)&md);
        }
        if (md == NULL) {
            *sign = NULL;
            return NOERROR;
        }

        AutoPtr< ArrayOf<Byte> > computedDigest;
        md->Digest(sfBytes, (ArrayOf<Byte>**)&computedDigest);
        if (!Arrays::Equals(existingDigest, computedDigest)) {
            Logger::E("CJarUtils", "Incorrect MD");
            return E_SECURITY_EXCEPTION;
        }
    }

    AutoPtr< ArrayOf<Byte> > encryptedDigest;
    sigInfo->GetEncryptedDigest((ArrayOf<Byte>**)&encryptedDigest);
    if (sig->Verify(encryptedDigest, &result), !result) {
        Logger::E("CJarUtils", "Incorrect signature");
        return E_SECURITY_EXCEPTION;
    }

    AutoPtr< ArrayOf<IX509Certificate*> > x509Certs = CreateChain((*certs)[issuerSertIndex], *certs);
    *sign = ArrayOf<ICertificate*>::Alloc(x509Certs->GetLength());
    REFCOUNT_ADD(*sign);
    for (Int32 i = 0; i < x509Certs->GetLength(); i++) {
        (*sign)->Set(i, ICertificate::Probe((*x509Certs)[i]));
    }
    return NOERROR;
}

AutoPtr< ArrayOf<IX509Certificate*> > CJarUtils::CreateChain(
    /* [in] */ IX509Certificate* signer,
    /* [in] */ const ArrayOf<IX509Certificate*>& candidates)
{
    AutoPtr<IPrincipal> issuer;
    signer->GetIssuerDN((IPrincipal**)&issuer);

    // Signer is self-signed
    AutoPtr<IPrincipal> subject;
    signer->GetSubjectDN((IPrincipal**)&subject);
    Boolean isEqual;
    if (subject->Equals(issuer, &isEqual), isEqual) {
        AutoPtr< ArrayOf<IX509Certificate*> > x509Certs = ArrayOf<IX509Certificate*>::Alloc(1);
        x509Certs->Set(1, signer);
        return x509Certs;
    }

    AutoPtr<IArrayList> chain;
    CArrayList::New(candidates.GetLength() + 1, (IArrayList**)&chain);
    chain->Add(0, signer);

    AutoPtr<IX509Certificate> issuerCert;
    Int32 count = 1;
    while (TRUE) {
        issuerCert = FindCert(issuer, candidates);
        if (issuerCert == NULL) {
            break;
        }
        Boolean result;
        chain->Add(issuerCert, &result);
        count++;
        issuer = NULL;
        issuerCert->GetIssuerDN((IPrincipal**)&issuer);
        subject = NULL;
        issuerCert->GetSubjectDN((IPrincipal**)&subject);
        if (subject->Equals(issuer, &isEqual), isEqual) {
            break;
        }
    }
    AutoPtr< ArrayOf<IX509Certificate*> > x509Certs;
    chain->ToArray((ArrayOf<IInterface*>**)&x509Certs);
    return x509Certs;
}

AutoPtr<IX509Certificate> CJarUtils::FindCert(
    /* [in] */ IPrincipal* issuer,
    /* [in] */ const ArrayOf<IX509Certificate*>& candidates)
{
    for (Int32 i = 0; i < candidates.GetLength(); i++) {
        AutoPtr<IPrincipal> subject;
        candidates[i]->GetSubjectDN((IPrincipal**)&subject);
        Boolean isEqual;
        if (issuer->Equals(subject, &isEqual), isEqual) {
            return candidates[i];
        }
    }
    return NULL;
}

AutoPtr< ArrayOf<Int32> > CJarUtils::Init_MESSAGE_DIGEST_OID()
{
    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(7);
    (*array)[0] = 1;
    (*array)[1] = 2;
    (*array)[2] = 840;
    (*array)[3] = 113549;
    (*array)[4] = 1;
    (*array)[5] = 9;
    (*array)[6] = 4;
    return array;
}

}
}
}
}
}
