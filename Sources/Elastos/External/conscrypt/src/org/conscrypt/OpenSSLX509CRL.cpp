//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "OpenSSLX509CRL.h"
#include "NativeCrypto.h"
#include "OpenSSLX509Certificate.h"
#include "elastos/utility/Arrays.h"

using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::Security::ISignature;
using Elastos::Utility::Arrays;
using Elastos::Utility::CHashSet;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastosx::Security::Auth::X500::CX500Principal;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLX509CRL::
//=========================================
OpenSSLX509CRL::OpenSSLX509CRL(
    /* [in] */ Int64 ctx)
{
    mContext = ctx;
}

ECode OpenSSLX509CRL::FromX509DerInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IOpenSSLX509CRL** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLBIOInputStream> bis;
    assert(0 && "TODO");
    // COpenSSLBIOInputStream::New(is, (IOpenSSLBIOInputStream**)&bis);

    Int64 bioCtx = 0;
    bis->GetBioContext(&bioCtx);
    Int64 crlCtx = 0;
    NativeCrypto::D2i_X509_CRL_bio(bioCtx, &crlCtx);
    if (crlCtx == 0) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IOpenSSLX509CRL> res;
    // new OpenSSLX509CRL(crlCtx);
    *result = res;
    REFCOUNT_ADD(*result)
    bis->Release();
    return NOERROR;

}

ECode OpenSSLX509CRL::FromPkcs7DerInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLBIOInputStream> bis;
    assert(0 && "TODO");
    // COpenSSLBIOInputStream::New(is, (IOpenSSLBIOInputStream**)&bis);

    Int64 context = 0;
    bis->GetBioContext(&context);
    AutoPtr<ArrayOf<Int64> > certRefs;
    NativeCrypto::D2i_PKCS7_bio(context, INativeCrypto::PKCS7_CRLS, (ArrayOf<Int64>**)&certRefs);
    bis->Release();

    AutoPtr<IList> certs;
    CArrayList::New(certRefs->GetLength(), (IList**)&certs);
    for (Int32 i = 0; i < certRefs->GetLength(); i++) {
        if ((*certRefs)[i] == 0) {
            continue;
        }
        AutoPtr<IOpenSSLX509CRL> p;
        assert(0 && "TODO");
        // COpenSSLX509CRL::New((*certRefs)[i], (IOpenSSLX509CRL**)&p);
        certs->Add(p);
    }
    *result = certs;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CRL::FromX509PemInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IOpenSSLX509CRL** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLBIOInputStream> bis;
    assert(0 && "TODO");
    // COpenSSLBIOInputStream::New(is, (IOpenSSLBIOInputStream**)&bis);

    Int64 context = 0;
    bis->GetBioContext(&context);
    Int64 crlCtx = 0;
    NativeCrypto::PEM_read_bio_X509_CRL(context, &crlCtx);
    if (crlCtx == 0) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IOpenSSLX509CRL> res;
    assert(0 && "TODO");
    // COpenSSLX509CRL::New(crlCtx, (IOpenSSLX509CRL**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    bis->Release();
    return NOERROR;
}

ECode OpenSSLX509CRL::FromPkcs7PemInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLBIOInputStream> bis;
    assert(0 && "TODO");
    // COpenSSLBIOInputStream::New(is, (IOpenSSLBIOInputStream**)&bis);

    Int64 context = 0;
    bis->GetBioContext(&context);
    AutoPtr<ArrayOf<Int64> > certRefs;
    // try {
        NativeCrypto::PEM_read_bio_PKCS7(context,
                INativeCrypto::PKCS7_CRLS, (ArrayOf<Int64>**)&certRefs);
    // } catch (Exception e) {
    //     throw new ParsingException(e);
    // } finally {
        bis->Release();
    // }

    AutoPtr<IList> certs;
    CArrayList::New(certRefs->GetLength(), (IList**)&certs);
    for (Int32 i = 0; i < certRefs->GetLength(); i++) {
        if ((*certRefs)[i] == 0) {
            continue;
        }
        AutoPtr<IOpenSSLX509CRL> p;
        assert(0 && "TODO");
        // COpenSSLX509CRL::New((*certRefs)[i], (IOpenSSLX509CRL**)&p);
        certs->Add(p);
    }
    *result = certs;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CRL::GetCriticalExtensionOIDs(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > critOids;
    NativeCrypto::Get_X509_CRL_ext_oids(mContext, INativeCrypto::EXTENSION_TYPE_CRITICAL,
        (ArrayOf<String>**)&critOids);

    /*
     * This API has a special case that if there are no extensions, we
     * should return null. So if we have no critical extensions, we'll check
     * non-critical extensions.
     */
    AutoPtr<ArrayOf<String> > crit;
    NativeCrypto::Get_X509_CRL_ext_oids(mContext, INativeCrypto::EXTENSION_TYPE_NON_CRITICAL,
        (ArrayOf<String>**)&crit);
    if ((critOids->GetLength() == 0)
            && (crit->GetLength() == 0)) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IList> oidsList;
    Arrays::AsList(critOids, (IList**)&oidsList);
    AutoPtr<IHashSet> res;
    CHashSet::New(ICollection::Probe(oidsList), (IHashSet**)&res);
    *result = ISet::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CRL::GetExtensionValue(
    /* [in] */ String oid,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::X509_CRL_get_ext_oid(mContext, oid, result);
}

ECode OpenSSLX509CRL::GetNonCriticalExtensionOIDs(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > nonCritOids;
    NativeCrypto::Get_X509_CRL_ext_oids(mContext,
                INativeCrypto::EXTENSION_TYPE_NON_CRITICAL,
                (ArrayOf<String>**)&nonCritOids);

    /*
     * This API has a special case that if there are no extensions, we
     * should return null. So if we have no non-critical extensions, we'll
     * check critical extensions.
     */
    AutoPtr<ArrayOf<String> > crit;
    NativeCrypto::Get_X509_CRL_ext_oids(mContext,
                INativeCrypto::EXTENSION_TYPE_CRITICAL,
                (ArrayOf<String>**)&crit);
    if ((nonCritOids->GetLength() == 0)
            && (crit->GetLength() == 0)) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IList> nonList;
    Arrays::AsList(nonCritOids, (IList**)&nonList);
    AutoPtr<IHashSet> res;
    CHashSet::New(ICollection::Probe(nonList), (IHashSet**)&res);
    *result = ISet::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CRL::HasUnsupportedCriticalExtension(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > criticalOids;
    NativeCrypto::Get_X509_CRL_ext_oids(mContext, INativeCrypto::EXTENSION_TYPE_CRITICAL,
            (ArrayOf<String>**)&criticalOids);
    for (Int32 i = 0; i < criticalOids->GetLength(); i++) {
        String oid = (*criticalOids)[i];
        Int64 extensionRef = 0;
        NativeCrypto::X509_CRL_get_ext(mContext, oid, &extensionRef);
        Int32 extension = 0;
        NativeCrypto::X509_supported_extension(extensionRef, &extension);
        if (extension != 1) {
            *result = TRUE;
            return NOERROR;
        }
    }

    *result = FALSE;
    return NOERROR;
}

ECode OpenSSLX509CRL::GetEncoded(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::I2d_X509_CRL(mContext, result);
}

void OpenSSLX509CRL::VerifyOpenSSL(
    /* [in] */ IOpenSSLKey* pkey)
{
    Int64 context = 0;
    pkey->GetPkeyContext(&context);
    NativeCrypto::X509_CRL_verify(mContext, context);
}

void OpenSSLX509CRL::VerifyInternal(
    /* [in] */ IPublicKey* key,
    /* [in] */ String sigProvider)
{
    String sigAlg;
    GetSigAlgName(&sigAlg);
    if (sigAlg.IsNull()) {
        GetSigAlgOID(&sigAlg);
    }

    AutoPtr<ISignature> sig;
    if (sigProvider.IsNull()) {
        assert(0 && "TODO");
        // Signature::GetInstance(sigAlg, (ISignature**)&sig);
    }
    else {
        assert(0 && "TODO");
        // Signature::GetInstance(sigAlg, sigProvider, (ISignature**)&sig);
    }

    sig->InitVerify(key);
    AutoPtr<ArrayOf<Byte> > certList;
    GetTBSCertList((ArrayOf<Byte>**)&certList);
    sig->Update(certList);
    AutoPtr<ArrayOf<Byte> > sigArr;
    GetSignature((ArrayOf<Byte>**)&sigArr);
    Boolean bVerify = FALSE;
    sig->Verify(sigArr, &bVerify);
    if (!bVerify) {
        // throw new SignatureException("signature did not verify");
        return;
    }
}

ECode OpenSSLX509CRL::Verify(
    /* [in] */ IPublicKey* key)
{
    if (IOpenSSLKeyHolder::Probe(key) != NULL) {
        AutoPtr<IOpenSSLKey> pkey;
        IOpenSSLKeyHolder::Probe(key)->GetOpenSSLKey((IOpenSSLKey**)&pkey);
        VerifyOpenSSL(pkey);
        return NOERROR;
    }

    VerifyInternal(key, String(NULL));
    return NOERROR;
}

ECode OpenSSLX509CRL::Verify(
    /* [in] */ IPublicKey* key,
    /* [in] */ String sigProvider)
{
    VerifyInternal(key, sigProvider);
    return NOERROR;
}

ECode OpenSSLX509CRL::GetVersion(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 version = 0;
    NativeCrypto::X509_CRL_get_version(mContext, &version);
    *result = (Int32)version + 1;
    return NOERROR;
}

ECode OpenSSLX509CRL::GetIssuerDN(
    /* [out] */ IPrincipal** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IX500Principal> res;
    GetIssuerX500Principal((IX500Principal**)&res);
    *result = IPrincipal::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CRL::GetIssuerX500Principal(
    /* [out] */ IX500Principal** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Byte> > issuer;
    NativeCrypto::X509_CRL_get_issuer_name(mContext, (ArrayOf<Byte>**)&issuer);
    AutoPtr<IX500Principal> res;
    CX500Principal::New(issuer, (IX500Principal**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CRL::GetThisUpdate(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ITimeZoneHelper> tzhlp;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzhlp);
    AutoPtr<ITimeZone> tz;
    tzhlp->GetTimeZone(String("UTC"), (ITimeZone**)&tz);
    AutoPtr<ICalendarHelper> chlp;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&chlp);
    AutoPtr<ICalendar> calendar;
    chlp->GetInstance(tz, (ICalendar**)&calendar);
    calendar->Set(ICalendar::MILLISECOND, 0);
    Int64 update = 0;
    NativeCrypto::X509_CRL_get_lastUpdate(mContext, &update);
    NativeCrypto::ASN1_TIME_to_Calendar(update, calendar);
    return calendar->GetTime(result);
}

ECode OpenSSLX509CRL::GetNextUpdate(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ITimeZoneHelper> tzhlp;
    CTimeZoneHelper::AcquireSingleton((ITimeZoneHelper**)&tzhlp);
    AutoPtr<ITimeZone> tz;
    tzhlp->GetTimeZone(String("UTC"), (ITimeZone**)&tz);
    AutoPtr<ICalendarHelper> chlp;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&chlp);
    AutoPtr<ICalendar> calendar;
    chlp->GetInstance(tz, (ICalendar**)&calendar);
    calendar->Set(ICalendar::MILLISECOND, 0);
    Int64 update = 0;
    NativeCrypto::X509_CRL_get_nextUpdate(mContext, &update);
    NativeCrypto::ASN1_TIME_to_Calendar(update,
            calendar);
    return calendar->GetTime(result);
}

ECode OpenSSLX509CRL::GetRevokedCertificate(
    /* [in] */ IBigInteger* serialNumber,
    /* [out] */ IX509CRLEntry** result)
{
    VALIDATE_NOT_NULL(result)
    Int64 revokedRef = 0;
    AutoPtr<ArrayOf<Byte> > arr;
    serialNumber->ToByteArray((ArrayOf<Byte>**)&arr);
    NativeCrypto::X509_CRL_get0_by_serial(mContext,
            arr, &revokedRef);
    if (revokedRef == 0) {
        *result = NULL;
        return NOERROR;
    }

    Int64 dup = 0;
    NativeCrypto::X509_REVOKED_dup(revokedRef, &dup);
    AutoPtr<IOpenSSLX509CRLEntry> res;
    assert(0 && "TODO");
    // new OpenSSLX509CRLEntry(dup);
    *result = IX509CRLEntry::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CRL::GetRevokedCertificate(
    /* [in] */ IX509Certificate* certificate,
    /* [out] */ IX509CRLEntry** result)
{
    VALIDATE_NOT_NULL(result)
    if (IOpenSSLX509Certificate::Probe(certificate) != NULL) {
        AutoPtr<IOpenSSLX509Certificate> osslCert = IOpenSSLX509Certificate::Probe(certificate);
        Int64 context = 0;
        osslCert->GetContext(&context);
        Int64 x509RevokedRef = 0;
        NativeCrypto::X509_CRL_get0_by_cert(mContext,
                context, &x509RevokedRef);

        if (x509RevokedRef == 0) {
            *result = NULL;
            return NOERROR;
        }

        Int64 dup = 0;
        NativeCrypto::X509_REVOKED_dup(x509RevokedRef, &dup);
        AutoPtr<IOpenSSLX509CRLEntry> res;
        assert(0 && "TODO");
        // new OpenSSLX509CRLEntry(dup);
        *result = IX509CRLEntry::Probe(res);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    AutoPtr<IBigInteger> sn;
    certificate->GetSerialNumber((IBigInteger**)&sn);
    return GetRevokedCertificate(sn, result);
}

ECode OpenSSLX509CRL::GetRevokedCertificates(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Int64> > entryRefs;
    NativeCrypto::X509_CRL_get_REVOKED(mContext, (ArrayOf<Int64>**)&entryRefs);
    if (entryRefs == NULL || entryRefs->GetLength() == 0) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<ISet> crlSet;
    CHashSet::New((ISet**)&crlSet);
    for (Int32 i = 0; i < entryRefs->GetLength(); i++) {
        Int64 entryRef = (*entryRefs)[i];
        AutoPtr<IOpenSSLX509CRLEntry> p;
        assert(0 && "TODO");
        // COpenSSLX509CRLEntry::New(entryRef, (IOpenSSLX509CRLEntry**)&p);
        crlSet->Add(p);
    }

    *result = crlSet;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509CRL::GetTBSCertList(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::Get_X509_CRL_crl_enc(mContext, result);
}

ECode OpenSSLX509CRL::GetSignature(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::Get_X509_CRL_signature(mContext, result);
}

ECode OpenSSLX509CRL::GetSigAlgName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String oid;
    GetSigAlgOID(&oid);
    assert(0 && "TODO");
    // return AlgNameMapper::Map2AlgName(oid, result);
    return NOERROR;
}

ECode OpenSSLX509CRL::GetSigAlgOID(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::Get_X509_CRL_sig_alg_oid(mContext, result);
}

ECode OpenSSLX509CRL::GetSigAlgParams(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::Get_X509_CRL_sig_alg_parameter(mContext, result);
}

ECode OpenSSLX509CRL::IsRevoked(
    /* [in] */ ICertificate* cert,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (IX509Certificate::Probe(cert) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<IOpenSSLX509Certificate> osslCert;
    if (IOpenSSLX509Certificate::Probe(cert) != NULL) {
        osslCert = IOpenSSLX509Certificate::Probe(cert);
    }
    else {
        // try {
        AutoPtr<ArrayOf<Byte> > encoded;
        cert->GetEncoded((ArrayOf<Byte>**)&encoded);
        AutoPtr<IByteArrayInputStream> stream;
        CByteArrayInputStream::New(encoded, (IByteArrayInputStream**)&stream);
        OpenSSLX509Certificate::FromX509DerInputStream(IInputStream::Probe(stream), (IOpenSSLX509Certificate**)&osslCert);
        // } catch (Exception e) {
        //     throw new RuntimeException("cannot convert certificate", e);
        // }
    }

    Int64 context = 0;
    osslCert->GetContext(&context);
    Int64 x509RevokedRef = 0;
    NativeCrypto::X509_CRL_get0_by_cert(mContext,
            context, &x509RevokedRef);

    *result = x509RevokedRef != 0;
    return NOERROR;
}

ECode OpenSSLX509CRL::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IByteArrayOutputStream> os;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&os);
    Int64 bioCtx = 0;
    NativeCrypto::Create_BIO_OutputStream(IOutputStream::Probe(os), &bioCtx);
    // try {
        NativeCrypto::X509_CRL_print(bioCtx, mContext);
        NativeCrypto::BIO_free_all(bioCtx);
        return os->ToString(result);
    // } finally {
    // }
}

ECode OpenSSLX509CRL::Finalize()
{
    if (mContext != 0) {
        NativeCrypto::X509_CRL_free(mContext);
    }
    // X509CRL::Finalize();
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
