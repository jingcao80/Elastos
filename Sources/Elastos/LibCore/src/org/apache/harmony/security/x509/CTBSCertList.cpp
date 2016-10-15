
#include "org/apache/harmony/security/x509/CTBSCertList.h"
#include "Time.h"
#include "CExtensionsHelper.h"
#include "CName.h"
#include "Arrays.h"
#include "AlgorithmIdentifier.h"
#include "ASN1SequenceOf.h"
#include "CASN1Explicit.h"
#include "CASN1Integer.h"
#include "CBerInputStream.h"
#include "CBigInteger.h"
#include <CoreUtils.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::IArrayOf;
using Elastos::Core::IByte;
using Elastos::Math::CBigInteger;
using Elastos::Utility::Arrays;
using Org::Apache::Harmony::Security::Asn1::ASN1SequenceOf;
using Org::Apache::Harmony::Security::Asn1::CASN1Integer;
using Org::Apache::Harmony::Security::Asn1::CASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::CBerInputStream;
using Org::Apache::Harmony::Security::Asn1::EIID_IASN1Type;
using Org::Apache::Harmony::Security::Asn1::IASN1Explicit;
using Org::Apache::Harmony::Security::Asn1::IASN1Integer;
using Org::Apache::Harmony::Security::X501::CName;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Security {
namespace X509 {

CTBSCertList::ASN1SequenceWrapper::ASN1SequenceWrapper()
{}

ECode CTBSCertList::ASN1SequenceWrapper::constructor(
    /* [in] */ ArrayOf<IASN1Type*>* types)
{
    ASN1Sequence::constructor(types);
    SetOptional(0);
    SetOptional(4);
    SetOptional(5);
    SetOptional(6);
    return NOERROR;
}

ECode CTBSCertList::ASN1SequenceWrapper::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(((CBerInputStream*)in)->mContent);

    AutoPtr<IInterface> v0;
    values->Get(0, (IInterface**)&v0);

    AutoPtr<IInterface> v1;
    values->Get(1, (IInterface**)&v1);

    AutoPtr<IInterface> v2;
    values->Get(2, (IInterface**)&v2);

    AutoPtr<IInterface> v3;
    values->Get(3, (IInterface**)&v3);

    AutoPtr<IInterface> v4;
    values->Get(4, (IInterface**)&v4);

    AutoPtr<IInterface> v5;
    values->Get(5, (IInterface**)&v5);

    AutoPtr<IInterface> v6;
    values->Get(6, (IInterface**)&v6);

    Int32 v = 1;
    if (v0 != NULL) {
        CASN1Integer::ToIntValue(v0, &v);
        v += 1;
    }

    AutoPtr<ArrayOf<Byte> > e;
    in->GetEncoded((ArrayOf<Byte>**)&e);
    AutoPtr<ITBSCertList> t;
    CTBSCertList::New(v, IAlgorithmIdentifier::Probe(v1),
                IName::Probe(v2),
                IDate::Probe(v3),
                IDate::Probe(v4),
                IList::Probe(v5),
                IExtensions::Probe(v6),
                e, (ITBSCertList**)&t);
    *result = IInterface::Probe(t);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertList::ASN1SequenceWrapper::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    CTBSCertList* tbs = (CTBSCertList*) ITBSCertList::Probe(object);
    AutoPtr<IInterface> fv;
    if (tbs->mVersion > 1) {
        CASN1Integer::FromIntValue(tbs->mVersion - 1, (IInterface**)&fv);
    }
    values->Set(0, fv);
    values->Set(1, tbs->mSignature);
    values->Set(2, tbs->mIssuer);
    values->Set(3, tbs->mThisUpdate);
    values->Set(4, tbs->mNextUpdate);
    values->Set(5, tbs->mRevokedCertificates);
    values->Set(6, tbs->mCrlExtensions);
    return NOERROR;
}

CTBSCertList::RevokedCertificate::ASN1SequenceWrapper2::ASN1SequenceWrapper2()
{}

ECode CTBSCertList::RevokedCertificate::ASN1SequenceWrapper2::constructor(
    /* [in] */ ArrayOf<IASN1Type*>* types)
{
    ASN1Sequence::constructor(types);
    SetOptional(2);
    return NOERROR;
}

ECode CTBSCertList::RevokedCertificate::ASN1SequenceWrapper2::GetDecodedObject(
    /* [in] */ IBerInputStream* in,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IArrayOf> values = IArrayOf::Probe(((CBerInputStream*)in)->mContent);

    AutoPtr<IInterface> v0;
    values->Get(0, (IInterface**)&v0);
    AutoPtr<IArrayOf> array = IArrayOf::Probe(v0);
    Int32 len = 0;
    array->GetLength(&len);
    AutoPtr<ArrayOf<Byte> > bs = ArrayOf<Byte>::Alloc(len);
    for (Int32 i = 0; i < len; i++) {
        AutoPtr<IInterface> o;
        array->Get(i, (IInterface**)&o);
        Byte v;
        IByte::Probe(o)->GetValue(&v);
        (*bs)[i] = v;
    }
    AutoPtr<IBigInteger> bt;
    CBigInteger::New(*bs, (IBigInteger**)&bt);

    AutoPtr<IInterface> v1;
    values->Get(1, (IInterface**)&v1);

    AutoPtr<IInterface> v2;
    values->Get(2, (IInterface**)&v2);

    AutoPtr<RevokedCertificate> r = new RevokedCertificate(bt, IDate::Probe(v1), IExtensions::Probe(v2));
    *result = r->Probe(EIID_IInterface);
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertList::RevokedCertificate::ASN1SequenceWrapper2::GetValues(
    /* [in] */ IInterface* object,
    /* [in] */ ArrayOf<IInterface*>* values)
{
    RevokedCertificate* rcert = (RevokedCertificate*) IRevokedCertificate::Probe(object);
    AutoPtr<ArrayOf<Byte> > ba;
    rcert->mUserCertificate->ToByteArray((ArrayOf<Byte>**)&ba);
    values->Set(0, CoreUtils::ConvertByteArray(ba));
    values->Set(1, rcert->mRevocationDate);
    values->Set(2, rcert->mCrlEntryExtensions);
    return NOERROR;
}

AutoPtr<IASN1Sequence> CTBSCertList::RevokedCertificate::ASN1 = CTBSCertList::RevokedCertificate::InitASN1();
AutoPtr<IASN1Sequence> CTBSCertList::RevokedCertificate::InitASN1()
{
    AutoPtr<ASN1SequenceWrapper2> a = new ASN1SequenceWrapper2();
    AutoPtr<ArrayOf<IASN1Type*> > at = ArrayOf<IASN1Type*>::Alloc(3);
    AutoPtr<IASN1Integer> ais = CASN1Integer::GetInstance();
    at->Set(0, IASN1Type::Probe(ais));
    at->Set(1, IASN1Type::Probe(Time::ASN1));

    AutoPtr<IExtensionsHelper> helper;
    CExtensionsHelper::AcquireSingleton((IExtensionsHelper**)&helper);
    AutoPtr<IASN1Type> tmp;
    helper->GetASN1((IASN1Type**)&tmp);
    at->Set(2, tmp);

    a->constructor(at);
    return a;
}

CAR_INTERFACE_IMPL(CTBSCertList::RevokedCertificate, Object, IRevokedCertificate)
CTBSCertList::RevokedCertificate::RevokedCertificate(
    /* [in] */ IBigInteger* userCertificate,
    /* [in] */ IDate* revocationDate,
    /* [in] */ IExtensions* crlEntryExtensions)
    : mIssuerRetrieved(FALSE)
{
    mUserCertificate = userCertificate;
    mRevocationDate = revocationDate;
    mCrlEntryExtensions = crlEntryExtensions;
}

AutoPtr<IExtensions> CTBSCertList::RevokedCertificate::GetCrlEntryExtensions()
{
    return mCrlEntryExtensions;
}

AutoPtr<IBigInteger> CTBSCertList::RevokedCertificate::GetUserCertificate()
{
    return mUserCertificate;
}

AutoPtr<IDate> CTBSCertList::RevokedCertificate::GetRevocationDate()
{
    return mRevocationDate;
}

AutoPtr<IX500Principal> CTBSCertList::RevokedCertificate::GetIssuer()
{
    if (mCrlEntryExtensions == NULL) {
        return NULL;
    }
    if (!mIssuerRetrieved) {
        // try {
        mCrlEntryExtensions->ValueOfCertificateIssuerExtension((IX500Principal**)&mIssuer);
        // } catch (IOException e) {
        //     e.printStackTrace();
        // }
        mIssuerRetrieved = TRUE;
    }
    return mIssuer;
}

AutoPtr<ArrayOf<Byte> > CTBSCertList::RevokedCertificate::GetEncoded()
{
    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(this->Probe(EIID_IInterface), (ArrayOf<Byte>**)&mEncoding);
    }
    return mEncoding;
}

ECode CTBSCertList::RevokedCertificate::Equals(
    /* [in] */ IInterface* rc,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    if (IRevokedCertificate::Probe(rc) == NULL) {
        *result = FALSE;
        return NOERROR;
    }
    RevokedCertificate* rcert = (RevokedCertificate*) IRevokedCertificate::Probe(rc);
    Int64 t1 = 0, t2 = 0;
    Boolean e1 = FALSE, e2 = FALSE;
    *result = (mUserCertificate->Equals(rcert->mUserCertificate, &e1), e1)
        && (((mRevocationDate->GetTime(&t1), t1) / 1000)
                == ((rcert->mRevocationDate->GetTime(&t2), t2) / 1000))
        && ((mCrlEntryExtensions == NULL)
            ? rcert->mCrlEntryExtensions == NULL
            : (IObject::Probe(mCrlEntryExtensions)->Equals(rcert->mCrlEntryExtensions, &e2), e2));
    return NOERROR;
}

ECode CTBSCertList::RevokedCertificate::GetHashCode(
    /* [out] */ Int32* hash)
{
    VALIDATE_NOT_NULL(hash);
    Int32 c1 = 0, c2 = 0;
    Int64 t1 = 0;
    mRevocationDate->GetTime(&t1);
    *hash = (IObject::Probe(mUserCertificate)->GetHashCode(&c1), c1) * 37 + (Int32)t1 / 1000
            + (mCrlEntryExtensions == NULL ? 0 : (IObject::Probe(mCrlEntryExtensions)->GetHashCode(&c2), c2));
    return NOERROR;
}

void CTBSCertList::RevokedCertificate::DumpValue(
    /* [in] */ IStringBuilder* sb,
    /* [in] */ const String& prefix)
{
    assert(0 && "TODO");
    // sb.append(prefix).append("Certificate Serial Number: ").append(userCertificate).append('\n');
    // sb.append(prefix).append("Revocation Date: ").append(revocationDate);
    // if (crlEntryExtensions != NULL) {
    //     sb.append('\n').append(prefix).append("CRL Entry Extensions: [");
    //     crlEntryExtensions.dumpValue(sb, prefix + "  ");
    //     sb.append(prefix).append(']');
    // }
}

AutoPtr<IASN1Sequence> CTBSCertList::ASN1 = InitASN1();

CAR_OBJECT_IMPL(CTBSCertList)
CAR_INTERFACE_IMPL(CTBSCertList, Object, ITBSCertList)
CTBSCertList::CTBSCertList()
    : mVersion(0)
{}

AutoPtr<IASN1Sequence> CTBSCertList::InitASN1()
{
    AutoPtr<ASN1SequenceWrapper> a = new ASN1SequenceWrapper();
    AutoPtr<ArrayOf<IASN1Type*> > at = ArrayOf<IASN1Type*>::Alloc(7);
    AutoPtr<IASN1Integer> ais = CASN1Integer::GetInstance();
    at->Set(0, IASN1Type::Probe(ais)); // version
    at->Set(1, IASN1Type::Probe(AlgorithmIdentifier::ASN1));  // signature
    at->Set(2, IASN1Type::Probe(CName::ASN1)); // issuer
    at->Set(3, IASN1Type::Probe(Time::ASN1)); // thisUpdate
    at->Set(4, IASN1Type::Probe(Time::ASN1)); // nextUpdate
    AutoPtr<ASN1SequenceOf> as = new ASN1SequenceOf(); // revokedCertificates
    as->constructor(IASN1Type::Probe(RevokedCertificate::ASN1));
    at->Set(5, (IASN1Type*)as->Probe(EIID_IASN1Type));

    AutoPtr<IExtensionsHelper> helper;
    CExtensionsHelper::AcquireSingleton((IExtensionsHelper**)&helper);
    AutoPtr<IASN1Type> tmp;
    helper->GetASN1((IASN1Type**)&tmp);
    AutoPtr<IASN1Explicit> ae;
    CASN1Explicit::New(0, tmp, (IASN1Explicit**)&ae); // crlExtensions
    at->Set(6, IASN1Type::Probe(ae));
    a->constructor(at);
    return a;
}

ECode CTBSCertList::constructor(
    /* [in] */ Int32 version,
    /* [in] */ IAlgorithmIdentifier* signature,
    /* [in] */ IName* issuer,
    /* [in] */ IDate* thisUpdate,
    /* [in] */ IDate* nextUpdate,
    /* [in] */ IList* revokedCertificates,
    /* [in] */ IExtensions* crlExtensions,
    /* [in] */ ArrayOf<Byte>* encoding)
{
    mVersion = version;
    mSignature = signature;
    mIssuer = issuer;
    mThisUpdate = thisUpdate;
    mNextUpdate = nextUpdate;
    mRevokedCertificates = revokedCertificates;
    mCrlExtensions = crlExtensions;
    mEncoding = encoding;
    return NOERROR;
}

ECode CTBSCertList::GetVersion(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mVersion;
    return NOERROR;
}

ECode CTBSCertList::GetSignature(
    /* [out] */ IAlgorithmIdentifier** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mSignature;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertList::GetIssuer(
    /* [out] */ IName** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIssuer;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertList::GetThisUpdate(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mThisUpdate;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertList::GetNextUpdate(
    /* [out] */ IDate** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNextUpdate;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertList::GetRevokedCertificates(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRevokedCertificates;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertList::GetCrlExtensions(
    /* [out] */ IExtensions** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mCrlExtensions;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CTBSCertList::GetEncoded(
    /* [out, callee] */ ArrayOf<Byte>** ppEncoded)
{
    VALIDATE_NOT_NULL(ppEncoded)
    if (mEncoding == NULL) {
        IASN1Type::Probe(ASN1)->Encode(this->Probe(EIID_IInterface), (ArrayOf<Byte>**)&mEncoding);
    }
    *ppEncoded = mEncoding;
    REFCOUNT_ADD(*ppEncoded);
    return NOERROR;
}

ECode CTBSCertList::Equals(
    /* [in] */ IInterface* other,
    /* [out] */ Boolean* result)
{
    if (ITBSCertList::Probe(other) == NULL) {
        *result = FALSE;
        return NOERROR;
    }

    Boolean e1 = FALSE;
    AutoPtr<ArrayOf<Byte> > a1;
    AutoPtr<ArrayOf<Byte> > a2;
    Int64 t1 = 0, t2 = 0;
    CTBSCertList* that = (CTBSCertList*) ITBSCertList::Probe(other);
    *result = mVersion == that->mVersion
        && (IObject::Probe(mSignature)->Equals(that->mSignature, &e1), e1)
        && Arrays::Equals((mIssuer->GetEncoded((ArrayOf<Byte>**)&a1), a1)
                  , (that->mIssuer->GetEncoded((ArrayOf<Byte>**)&a2), a2))
        && (mThisUpdate->GetTime(&t1), t1) / 1000
                == (that->mThisUpdate->GetTime(&t2), t2) / 1000
        && (mNextUpdate == NULL
                ? that->mNextUpdate == NULL
                : (mNextUpdate->GetTime(&t1), t1) / 1000
                    == (that->mNextUpdate->GetTime(&t2), t2) / 1000)
        && (((mRevokedCertificates == NULL || that->mRevokedCertificates == NULL)
            && mRevokedCertificates == that->mRevokedCertificates)
            || (mRevokedCertificates->Equals(that->mRevokedCertificates, &e1), e1))
        && (mCrlExtensions == NULL
                ? that->mCrlExtensions == NULL
                : (IObject::Probe(mCrlExtensions)->Equals(that->mCrlExtensions, &e1), e1));
    return NOERROR;
}

ECode CTBSCertList::GetHashCode(
    /* [out] */ Int32* pHashCode)
{
    VALIDATE_NOT_NULL(pHashCode)
    Int32 hc = 0;
    Int64 t = 0;
    AutoPtr<ArrayOf<Byte> > a1;
    *pHashCode = ((mVersion * 37 + (IObject::Probe(mSignature)->GetHashCode(&hc), hc)) * 37
            + Arrays::GetHashCode((mIssuer->GetEncoded((ArrayOf<Byte>**)&a1), a1))) * 37
            + (Int32)(mThisUpdate->GetTime(&t)) / 1000;
    return NOERROR;
}

ECode CTBSCertList::DumpValue(
    /* [in] */ IStringBuilder* pSb)
{
    assert(0 && "TODO");
    // sb.append("X.509 CRL v").append(mVersion);
    // sb.append("\nSignature Algorithm: [");
    // mSignature.dumpValue(sb);
    // sb.append(']');
    // sb.append("\nIssuer: ").append(mIssuer.getName(X500Principal.RFC2253));
    // sb.append("\n\nThis Update: ").append(mThisUpdate);
    // sb.append("\nNext Update: ").append(mNextUpdate).append('\n');
    // if (mRevokedCertificates != NULL) {
    //     sb.append("\nRevoked Certificates: ").append(mRevokedCertificates.size()).append(" [");
    //     int number = 1;
    //     for (RevokedCertificate revokedCertificate : mRevokedCertificates) {
    //         sb.append("\n  [").append(number++).append(']');
    //         revokedCertificate.dumpValue(sb, "  ");
    //         sb.append('\n');
    //     }
    //     sb.append("]\n");
    // }
    // if (mCrlExtensions != NULL) {
    //     sb.append("\nCRL Extensions: ").append(mCrlExtensions.size()).append(" [");
    //     mCrlExtensions.dumpValue(sb, "  ");
    //     sb.append("]\n");
    // }
    return NOERROR;
}

} // namespace X509
} // namespace Security
} // namespace Harmony
} // namespace Apache
} // namespace Org
