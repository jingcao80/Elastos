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

#include "Elastos.CoreLibrary.IO.h"
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.Apache.h"
#include "OpenSSLX509Certificate.h"
#include "NativeCrypto.h"
#include "org/conscrypt/COpenSSLX509Certificate.h"
#include "org/conscrypt/COpenSSLBIOInputStream.h"
#include "org/conscrypt/COpenSSLKey.h"
#include "org/conscrypt/CX509PublicKey.h"
#include <elastos/core/Math.h>
#include <elastos/utility/Arrays.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::IO::IByteArrayOutputStream;
using Elastos::IO::CByteArrayOutputStream;
using Elastos::Math::CBigInteger;
using Elastos::Security::IKeyFactory;
using Elastos::Security::CKeyFactoryHelper;
using Elastos::Security::IKeyFactoryHelper;
using Elastos::Security::ISignature;
using Elastos::Security::ISignatureHelper;
using Elastos::Security::CSignatureHelper;
using Elastos::Security::Spec::IKeySpec;
using Elastos::Security::Spec::IX509EncodedKeySpec;
using Elastos::Security::Spec::CX509EncodedKeySpec;
using Elastos::Security::Cert::IX509Certificate;
using Elastos::Utility::Arrays;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ITimeZone;
using Elastos::Utility::ITimeZoneHelper;
using Elastos::Utility::CTimeZoneHelper;
using Elastos::Utility::IHashSet;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CDate;
using Elastosx::Security::Auth::X500::CX500Principal;
using Org::Apache::Harmony::Security::Utils::IAlgNameMapperHelper;
using Org::Apache::Harmony::Security::Utils::CAlgNameMapperHelper;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLX509Certificate::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLX509Certificate, X509Certificate, IOpenSSLX509Certificate)

ECode OpenSSLX509Certificate::constructor(
    /* [in] */ Int64 ctx)
{
    X509Certificate::constructor();
    mContext = ctx;
    return NOERROR;
}

ECode OpenSSLX509Certificate::FromX509DerInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IOpenSSLX509Certificate** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLBIOInputStream> bis;
    COpenSSLBIOInputStream::New(is, (IOpenSSLBIOInputStream**)&bis);

    // try {
        Int64 context = 0;
        bis->GetBioContext(&context);
        Int64 certCtx = 0;
        NativeCrypto::D2i_X509_bio(context, &certCtx);
        if (certCtx == 0) {
            *result = NULL;
            return NOERROR;
        }
        AutoPtr<IOpenSSLX509Certificate> res;
        COpenSSLX509Certificate::New(certCtx, (IOpenSSLX509Certificate**)&res);
        *result = res;
        REFCOUNT_ADD(*result)
    // } catch (Exception e) {
    //     throw new ParsingException(e);
    // } finally {
        bis->Release();
        return NOERROR;
    // }
}

ECode OpenSSLX509Certificate::FromX509Der(
    /* [in] */ ArrayOf<Byte>* encoded,
    /* [out] */ IOpenSSLX509Certificate** result)
{
    VALIDATE_NOT_NULL(result)
    Int64 certCtx = 0;
    NativeCrypto::D2i_X509(encoded, &certCtx);
    if (certCtx == 0) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IOpenSSLX509Certificate> res;
    COpenSSLX509Certificate::New(certCtx, (IOpenSSLX509Certificate**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509Certificate::FromPkcs7DerInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLBIOInputStream> bis;
    COpenSSLBIOInputStream::New(is, (IOpenSSLBIOInputStream**)&bis);

    Int64 context = 0;
    bis->GetBioContext(&context);
    AutoPtr<ArrayOf<Int64> > certRefs;
    // try {
        NativeCrypto::D2i_PKCS7_bio(context, INativeCrypto::PKCS7_CERTS, (ArrayOf<Int64>**)&certRefs);
    // } catch (Exception e) {
    //     throw new ParsingException(e);
    // } finally {
        bis->Release();
    // }

    if (certRefs == NULL) {
        AutoPtr<ICollections> cls;
        CCollections::AcquireSingleton((ICollections**)&cls);
        return cls->GetEmptyList(result);
    }

    AutoPtr<IList> certs;
    CArrayList::New(certRefs->GetLength(), (IList**)&certs);
    for (Int32 i = 0; i < certRefs->GetLength(); i++) {
        if ((*certRefs)[i] == 0) {
            continue;
        }
        AutoPtr<IOpenSSLX509Certificate> p;
        COpenSSLX509Certificate::New((*certRefs)[i], (IOpenSSLX509Certificate**)&p);
        certs->Add(p);
    }
    *result = certs;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509Certificate::FromX509PemInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IOpenSSLX509Certificate** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLBIOInputStream> bis;
    COpenSSLBIOInputStream::New(is, (IOpenSSLBIOInputStream**)&bis);

    // try {
        Int64 context = 0;
        bis->GetBioContext(&context);
        Int64 certCtx = 0;
        NativeCrypto::PEM_read_bio_X509(context, &certCtx);
        if (certCtx == 0l) {
            *result = NULL;
            return NOERROR;
        }
        AutoPtr<IOpenSSLX509Certificate> res;
        COpenSSLX509Certificate::New(certCtx, (IOpenSSLX509Certificate**)&res);
        *result = res;
        REFCOUNT_ADD(*result)
    // } catch (Exception e) {
    //     throw new ParsingException(e);
    // } finally {
        bis->Release();
        return NOERROR;
    // }
}

ECode OpenSSLX509Certificate::FromPkcs7PemInputStream(
    /* [in] */ IInputStream* is,
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IOpenSSLBIOInputStream> bis;
    COpenSSLBIOInputStream::New(is, (IOpenSSLBIOInputStream**)&bis);

    AutoPtr<ArrayOf<Int64> > certRefs;
    // try {
        Int64 context = 0;
        bis->GetBioContext(&context);
        NativeCrypto::PEM_read_bio_PKCS7(context,
                INativeCrypto::PKCS7_CERTS, (ArrayOf<Int64>**)&certRefs);
    // } catch (Exception e) {
    //     throw new ParsingException(e);
    // } finally {
        bis->Release();
    // }

    AutoPtr<IList> certs;
    CArrayList::New(
            certRefs->GetLength(), (IList**)&certs);
    for (Int32 i = 0; i < certRefs->GetLength(); i++) {
        if ((*certRefs)[i] == 0) {
            continue;
        }
        AutoPtr<IOpenSSLX509Certificate> p;
        COpenSSLX509Certificate::New((*certRefs)[i], (IOpenSSLX509Certificate**)&p);
        certs->Add(p);
    }
    *result = certs;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509Certificate::FromCertificate(
    /* [in] */ ICertificate* cert,
    /* [out] */ IOpenSSLX509Certificate** result)
{
    VALIDATE_NOT_NULL(result)
    if (IOpenSSLX509Certificate::Probe(cert) != NULL) {
        *result = IOpenSSLX509Certificate::Probe(cert);
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    else if (IX509Certificate::Probe(cert) != NULL) {
        AutoPtr<ArrayOf<Byte> > encoded;
        cert->GetEncoded((ArrayOf<Byte>**)&encoded);
        return FromX509Der(encoded, result);
    }
    else {
        // throw new CertificateEncodingException("Only X.509 certificates are supported");
        return E_CERTIFICATE_ENCODING_EXCEPTION;
    }
}

ECode OpenSSLX509Certificate::GetCriticalExtensionOIDs(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > critOids;
    NativeCrypto::Get_X509_ext_oids(mContext, INativeCrypto::EXTENSION_TYPE_CRITICAL,
            (ArrayOf<String>**)&critOids);

    /*
     * This API has a special case that if there are no extensions, we
     * should return NULL. So if we have no critical extensions, we'll check
     * non-critical extensions.
     */
    AutoPtr<ArrayOf<String> > oids;
    NativeCrypto::Get_X509_ext_oids(mContext, INativeCrypto::EXTENSION_TYPE_NON_CRITICAL,
            (ArrayOf<String>**)&oids);
    if ((critOids->GetLength() == 0)
            && (oids->GetLength() == 0)) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IList> l;
    Arrays::AsList(critOids, (IList**)&l);
    AutoPtr<IHashSet> res;
    CHashSet::New(ICollection::Probe(l), (IHashSet**)&res);
    *result = ISet::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetExtensionValue(
    /* [in] */ const String& oid,
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::X509_get_ext_oid(mContext, oid, result);
}

ECode OpenSSLX509Certificate::GetNonCriticalExtensionOIDs(
    /* [out] */ ISet** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > nonCritOids;
    NativeCrypto::Get_X509_ext_oids(mContext,
            INativeCrypto::EXTENSION_TYPE_NON_CRITICAL,
            (ArrayOf<String>**)&nonCritOids);

    /*
     * This API has a special case that if there are no extensions, we
     * should return NULL. So if we have no non-critical extensions, we'll
     * check critical extensions.
     */
    AutoPtr<ArrayOf<String> > critOids;
    NativeCrypto::Get_X509_ext_oids(mContext,
            INativeCrypto::EXTENSION_TYPE_CRITICAL,
            (ArrayOf<String>**)&critOids);
    if ((nonCritOids->GetLength() == 0)
            && (critOids->GetLength() == 0)) {
        *result = NULL;
        return NOERROR;
    }

    AutoPtr<IList> l;
    Arrays::AsList(nonCritOids, (IList**)&l);
    AutoPtr<IHashSet> res;
    CHashSet::New(ICollection::Probe(l), (IHashSet**)&res);
    *result = ISet::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509Certificate::HasUnsupportedCriticalExtension(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 flags = 0;
    NativeCrypto::Get_X509_ex_flags(mContext, &flags);
    *result = (flags & INativeCrypto::EXFLAG_CRITICAL) != 0;
    return NOERROR;
}

ECode OpenSSLX509Certificate::CheckValidity()
{
    AutoPtr<IDate> d;
    CDate::New((IDate**)&d);
    CheckValidity(d);
    return NOERROR;
}

ECode OpenSSLX509Certificate::CheckValidity(
    /* [in] */ IDate* date)
{
    AutoPtr<IDate> bf;
    GetNotBefore((IDate**)&bf);
    assert(0 && "TODO");
    // if (bf->CompareTo(date) > 0) {
    //     // throw new CertificateNotYetValidException("Certificate not valid until "
    //     //         + getNotBefore().toString() + " (compared to " + date.toString() + ")");
    //     return E_CERTIFICATE_NOT_YET_VALID_EXCEPTION;
    // }

    AutoPtr<IDate> af;
    GetNotAfter((IDate**)&af);
    assert(0 && "TODO");
    // if (af->CompareTo(date) < 0) {
    //     // throw new CertificateExpiredException("Certificate expired at "
    //     //         + getNotAfter().toString() + " (compared to " + date.toString() + ")");
    //     return E_CERTIFICATE_EXPIRED_EXCEPTION;
    // }
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetVersion(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int64 version = 0;
    NativeCrypto::X509_get_version(mContext, &version);
    *result = (Int32)version + 1;
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetSerialNumber(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Byte> > sn;
    NativeCrypto::X509_get_serialNumber(mContext, (ArrayOf<Byte>**)&sn);
    AutoPtr<IBigInteger> res;
    CBigInteger::New(*sn, (IBigInteger**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetIssuerDN(
    /* [out] */ IPrincipal** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IX500Principal> res;
    GetIssuerX500Principal((IX500Principal**)&res);
    *result = IPrincipal::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetSubjectDN(
    /* [out] */ IPrincipal** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IX500Principal> res;
    GetSubjectX500Principal((IX500Principal**)&res);
    *result = IPrincipal::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetNotBefore(
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
    Int64 context = 0;
    NativeCrypto::X509_get_notBefore(mContext, &context);
    NativeCrypto::ASN1_TIME_to_Calendar(context, calendar);
    return calendar->GetTime(result);
}

ECode OpenSSLX509Certificate::GetNotAfter(
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
    Int64 notafter = 0;
    NativeCrypto::X509_get_notAfter(mContext, &notafter);
    NativeCrypto::ASN1_TIME_to_Calendar(notafter, calendar);
    return calendar->GetTime(result);
}

ECode OpenSSLX509Certificate::GetTBSCertificate(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::Get_X509_cert_info_enc(mContext, result);
}

ECode OpenSSLX509Certificate::GetSignature(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::Get_X509_signature(mContext, result);
}

ECode OpenSSLX509Certificate::GetSigAlgName(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String oid;
    GetSigAlgOID(&oid);
    AutoPtr<IAlgNameMapperHelper> anmHelper;
    CAlgNameMapperHelper::AcquireSingleton((IAlgNameMapperHelper**)&anmHelper);
    return anmHelper->Map2AlgName(oid, result);
}

ECode OpenSSLX509Certificate::GetSigAlgOID(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::Get_X509_sig_alg_oid(mContext, result);
}

ECode OpenSSLX509Certificate::GetSigAlgParams(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::Get_X509_sig_alg_parameter(mContext, result);
}

ECode OpenSSLX509Certificate::GetIssuerUniqueID(
    /* [out] */ ArrayOf<Boolean>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::Get_X509_issuerUID(mContext, result);
}

ECode OpenSSLX509Certificate::GetSubjectUniqueID(
    /* [out] */ ArrayOf<Boolean>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::Get_X509_subjectUID(mContext, result);
}

ECode OpenSSLX509Certificate::GetKeyUsage(
    /* [out] */ ArrayOf<Boolean>** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Boolean> > kusage;
    NativeCrypto::Get_X509_ex_kusage(mContext, (ArrayOf<Boolean>**)&kusage);
    if (kusage == NULL) {
        *result = NULL;
        return NOERROR;
    }

    if (kusage->GetLength() >= 9) {
        *result = kusage;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Boolean> > resized = ArrayOf<Boolean>::Alloc(9);
    resized->Copy(0, kusage, 0, kusage->GetLength());
    *result = resized;
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetBasicConstraints(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    Int32 flags = 0;
    NativeCrypto::Get_X509_ex_flags(mContext, &flags);
    if ((flags & INativeCrypto::EXFLAG_CA) == 0) {
        *result = -1;
        return NOERROR;
    }

    Int32 pathLen = 0;
    NativeCrypto::Get_X509_ex_pathlen(mContext, &pathLen);
    if (pathLen == -1) {
        *result = Elastos::Core::Math::INT32_MAX_VALUE;
        return NOERROR;
    }

    *result = pathLen;
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetEncoded(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    return NativeCrypto::I2d_X509(mContext, result);
}

void OpenSSLX509Certificate::VerifyOpenSSL(
    /* [in] */ IOpenSSLKey* pkey)
{
    // try {
        Int64 ctx = 0;
        pkey->GetPkeyContext(&ctx);
        NativeCrypto::X509_verify(mContext, ctx);
    // } catch (RuntimeException e) {
    //     throw new CertificateException(e);
    // } catch (BadPaddingException e) {
    //     throw new SignatureException();
    // }
}

void OpenSSLX509Certificate::VerifyInternal(
    /* [in] */ IPublicKey* key,
    /* [in] */ String sigProvider)
{
    String sigAlg;
    GetSigAlgName(&sigAlg);
    if (sigAlg.IsNull()) {
        GetSigAlgOID(&sigAlg);
    }

    AutoPtr<ISignatureHelper> hlp;
    CSignatureHelper::AcquireSingleton((ISignatureHelper**)&hlp);
    AutoPtr<ISignature> sig;
    if (sigProvider.IsNull()) {
        hlp->GetInstance(sigAlg, (ISignature**)&sig);
    }
    else {
        hlp->GetInstance(sigAlg, sigProvider, (ISignature**)&sig);
    }

    sig->InitVerify(key);
    AutoPtr<ArrayOf<Byte> > ct;
    GetTBSCertificate((ArrayOf<Byte>**)&ct);
    sig->Update(ct);
    AutoPtr<ArrayOf<Byte> > sg;
    GetSignature((ArrayOf<Byte>**)&sg);
    Boolean bVerify = FALSE;
    sig->Verify(sg, &bVerify);
    if (!bVerify) {
        // throw new SignatureException("signature did not verify");
        return;
    }
}

ECode OpenSSLX509Certificate::Verify(
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

ECode OpenSSLX509Certificate::Verify(
    /* [in] */ IPublicKey* key,
    /* [in] */ const String& sigProvider)
{
    VerifyInternal(key, sigProvider);
    return NOERROR;
}

ECode OpenSSLX509Certificate::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IByteArrayOutputStream> os;
    CByteArrayOutputStream::New((IByteArrayOutputStream**)&os);
    Int64 bioCtx = 0;
    NativeCrypto::Create_BIO_OutputStream(IOutputStream::Probe(os), &bioCtx);
    // try {
        NativeCrypto::X509_print_ex(bioCtx, mContext, 0, 0);
        os->ToString(result);
    // } finally {
        NativeCrypto::BIO_free_all(bioCtx);
    // }
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetPublicKey(
    /* [out] */ IPublicKey** result)
{
    VALIDATE_NOT_NULL(result)
    /* First try to generate the key from supported OpenSSL key types. */
    // try {
        Int64 pubkey = 0;
        NativeCrypto::X509_get_pubkey(mContext, &pubkey);
        AutoPtr<IOpenSSLKey> pkey;
        COpenSSLKey::New(pubkey, (IOpenSSLKey**)&pkey);
        return pkey->GetPublicKey(result);
    // } catch (NoSuchAlgorithmException ignored) {
    // }

    /* Try generating the key using other Java providers. */
    String oid;
    NativeCrypto::Get_X509_pubkey_oid(mContext, &oid);
    AutoPtr<ArrayOf<Byte> > encoded;
    NativeCrypto::I2d_X509_PUBKEY(mContext, (ArrayOf<Byte>**)&encoded);
    // try {
        AutoPtr<IKeyFactoryHelper> kfHelper;
        CKeyFactoryHelper::AcquireSingleton((IKeyFactoryHelper**)&kfHelper);
        AutoPtr<IKeyFactory> kf;
        kfHelper->GetInstance(oid, (IKeyFactory**)&kf);
        AutoPtr<IX509EncodedKeySpec> spec;
        CX509EncodedKeySpec::New(encoded, (IX509EncodedKeySpec**)&spec);
        return kf->GeneratePublic(IKeySpec::Probe(spec), result);
    // } catch (NoSuchAlgorithmException ignored) {
    // } catch (InvalidKeySpecException ignored) {
    // }

    /*
     * We couldn't find anything else, so just return a nearly-unusable
     * X.509-encoded key.
     */
    AutoPtr<IX509PublicKey> res;
    CX509PublicKey::New(oid, encoded, (IX509PublicKey**)&res);
    *result = IPublicKey::Probe(res);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetIssuerX500Principal(
    /* [out] */ IX500Principal** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Byte> > issuer;
    NativeCrypto::X509_get_issuer_name(mContext, (ArrayOf<Byte>**)&issuer);
    AutoPtr<IX500Principal> res;
    CX500Principal::New(issuer, (IX500Principal**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetSubjectX500Principal(
    /* [out] */ IX500Principal** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<Byte> > subject;
    NativeCrypto::X509_get_subject_name(mContext, (ArrayOf<Byte>**)&subject);
    AutoPtr<IX500Principal> res;
    CX500Principal::New(subject, (IX500Principal**)&res);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetExtendedKeyUsage(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<String> > extUsage;
    NativeCrypto::Get_X509_ex_xkusage(mContext, (ArrayOf<String>**)&extUsage);
    if (extUsage == NULL) {
        *result = NULL;
        return NOERROR;
    }

    return Arrays::AsList(extUsage, result);
}

AutoPtr<ICollection> OpenSSLX509Certificate::AlternativeNameArrayToList(
    /* [in] */ ArrayOf<AutoPtr<ArrayOf<IInterface*> > >* altNameArray)
{
    if (altNameArray == NULL) {
        return NULL;
    }

    AutoPtr<ICollections> cls;
    CCollections::AcquireSingleton((ICollections**)&cls);
    AutoPtr<ICollection> coll;
    CArrayList::New(altNameArray->GetLength(), (ICollection**)&coll);
    for (Int32 i = 0; i < altNameArray->GetLength(); i++) {
        AutoPtr<IList> arr;
        Arrays::AsList((*altNameArray)[i], (IList**)&arr);
        AutoPtr<IList> l;
        cls->UnmodifiableList(arr, (IList**)&l);
        coll->Add(l);
    }

    AutoPtr<ICollection> res;
    cls->UnmodifiableCollection(coll, (ICollection**)&res);
    return res;
}

ECode OpenSSLX509Certificate::GetSubjectAlternativeNames(
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<IArrayOf*> > stack;
    NativeCrypto::Get_X509_GENERAL_NAME_stack(mContext,
            INativeCrypto::GN_STACK_SUBJECT_ALT_NAME,
            (ArrayOf<IArrayOf*>**)&stack);
    assert(0 && "TODO");
    AutoPtr<ICollection> res;// = AlternativeNameArrayToList(stack);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetIssuerAlternativeNames(
    /* [out] */ ICollection** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<ArrayOf<IArrayOf*> > stack;
    NativeCrypto::Get_X509_GENERAL_NAME_stack(mContext,
            INativeCrypto::GN_STACK_ISSUER_ALT_NAME,
            (ArrayOf<IArrayOf*>**)&stack);
    assert(0 && "TODO");
    AutoPtr<ICollection> res;// = AlternativeNameArrayToList(stack);
    *result = res;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLX509Certificate::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (IOpenSSLX509Certificate::Probe(other) != NULL) {
        AutoPtr<IOpenSSLX509Certificate> o = IOpenSSLX509Certificate::Probe(other);
        Int32 cmp = 0;
        Int64 context = 0;
        o->GetContext(&context);
        NativeCrypto::X509_cmp(mContext, context, &cmp);
        *result = cmp == 0;
        return NOERROR;
    }

    assert(0 && "TODO");
    // return X509Certificate::Equals(other, result);
    return NOERROR;
}

ECode OpenSSLX509Certificate::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    /* Make this faster since we might be in hash-based structures. */
    return NativeCrypto::Get_X509_hashCode(mContext, result);
}

ECode OpenSSLX509Certificate::GetContext(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mContext;
    return NOERROR;
}

ECode OpenSSLX509Certificate::Finalize()
{
    if (mContext != 0) {
        NativeCrypto::X509_free(mContext);
    }
    assert(0 && "TODO");
    // X509Certificate::Finalize();
    return NOERROR;
}

} // namespace Conscrypt
} // namespace Org
