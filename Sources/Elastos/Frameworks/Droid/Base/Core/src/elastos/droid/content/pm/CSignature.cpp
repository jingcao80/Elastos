
#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Security.h>
#include "elastos/droid/content/pm/CSignature.h"
#include "elastos/droid/internal/utility/ArrayUtils.h"
#include <elastos/utility/Arrays.h>
#include <elastos/core/Math.h>

using Elastos::Droid::Internal::Utility::ArrayUtils;
using Elastos::Core::Math;
using Elastos::Utility::Arrays;
using Elastos::IO::IInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::Security::Cert::ICertificateFactoryHelper;
using Elastos::Security::Cert::CCertificateFactoryHelper;
using Elastos::Security::Cert::IX509Certificate;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(CSignature, Object, ISignature, IParcelable)

CAR_OBJECT_IMPL(CSignature)

CSignature::CSignature()
    : mHashCode(0)
    , mHaveHashCode(FALSE)
{}

ECode CSignature::constructor()
{
    return NOERROR;
}

ECode CSignature::constructor(
    /* [in] */ ArrayOf<Byte>* signature)
{
    VALIDATE_NOT_NULL(signature)
    mSignature = signature->Clone();
    return NOERROR;
}

ECode CSignature::constructor(
    /* [in] */ const String& text)
{
    AutoPtr<ArrayOf<Byte> > input = text.GetBytes();
    const Int32 N = input->GetLength();

    if (N % 2 != 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<ArrayOf<Byte> > sig = ArrayOf<Byte>::Alloc(N / 2);
    Int32 sigIndex = 0;
    Int32 hi, lo;
    for (Int32 i = 0; i < N;) {
        FAIL_RETURN(ParseHexDigit((*input)[i++], &hi));
        FAIL_RETURN(ParseHexDigit((*input)[i++], &lo));
        (*sig)[sigIndex++] = (Byte) ((hi << 4) | lo);
    }

    mSignature = sig->Clone();
    return NOERROR;
}


ECode CSignature::constructor(
    /* [in] */ ArrayOf<ICertificate*>* certificateChain)
{
    (*certificateChain)[0]->GetEncoded((ArrayOf<Byte>**)&mSignature);
    if (certificateChain->GetLength() > 1) {
        Arrays::CopyOfRange(certificateChain, 1, certificateChain->GetLength(), (ArrayOf<ICertificate*>**)&mCertificateChain);
    }
    return NOERROR;
}

ECode CSignature::ParseHexDigit(
    /* [in] */ Int32 nibble,
    /* [out] */ Int32* digit)
{
    if ('0' <= nibble && nibble <= '9') {
        *digit = nibble - '0';
        return NOERROR;
    }
    else if ('a' <= nibble && nibble <= 'f') {
        *digit =  nibble - 'a' + 10;
        return NOERROR;
    }
    else if ('A' <= nibble && nibble <= 'F') {
        *digit =  nibble - 'A' + 10;
        return NOERROR;
    }
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
}

ECode CSignature::ToChars(
    /* [out, callee] */ ArrayOf<Char32>** text)
{
    return ToChars(NULL, NULL, text);
}

ECode CSignature::ToChars(
    /* [in] */ ArrayOf<Char32>* existingArray,
    /* [in] */ ArrayOf<Int32>* outLen,
    /* [out, callee] */ ArrayOf<Char32>** text)
{
    VALIDATE_NOT_NULL(text);

    AutoPtr<ArrayOf<Byte> > sig = mSignature;
    Int32 N = sig->GetLength();
    Int32 N2 = N*2;
    AutoPtr<ArrayOf<Char32> > textArray = existingArray == NULL || N2 > existingArray->GetLength()
                ? ArrayOf<Char32>::Alloc(N2) : existingArray;
    for (Int32 j = 0; j < N; j++) {
        Byte v = (*sig)[j];
        Int32 d = (v >> 4) & 0xf;
        (*textArray)[j*2] = (Char32)(d >= 10 ? ('a' + d - 10) : ('0' + d));
        d = v&0xf;
        (*textArray)[j*2+1] = (Char32)(d >= 10 ? ('a' + d - 10) : ('0' + d));
    }
    if (outLen != NULL) (*outLen)[0] = N;
    *text = textArray;
    return NOERROR;
}

ECode CSignature::ToCharsString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

   String str = mStringRef.IsNull() ? String(NULL) : mStringRef;
   if (!str.IsNull()) {
       *result = str;
       return NOERROR;
   }

   AutoPtr<ArrayOf<Char32> > chars;
   ToChars((ArrayOf<Char32>**)&chars);

   *result = String(*chars.Get());
   mStringRef = *result;
   return NOERROR;
}

ECode CSignature::ToByteArray(
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes);

    *bytes = mSignature->Clone();
    REFCOUNT_ADD(*bytes);
    return NOERROR;
}

ECode CSignature::GetPublicKey(
    /* [out] */ IPublicKey** publicKey)
{
    VALIDATE_NOT_NULL(publicKey)
    *publicKey = NULL;

    AutoPtr<ICertificateFactory> certFactory;
    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&certFactory);

    AutoPtr<IInputStream> bais;
    CByteArrayInputStream::New(mSignature, (IInputStream**)&bais);
    AutoPtr<ICertificate> cert;
    certFactory->GenerateCertificate(bais, (ICertificate**)&cert);
    return cert->GetPublicKey(publicKey);
}

ECode CSignature::GetChainSignatures(
    /* [out, callee] */ ArrayOf<ISignature*>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    if (mCertificateChain == NULL) {
        AutoPtr<ArrayOf<ISignature*> > sigs = ArrayOf<ISignature*>::Alloc(1);
        sigs->Set(0, THIS_PROBE(ISignature));
        *result = sigs;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }

    AutoPtr<ArrayOf<ISignature*> > chain = ArrayOf<ISignature*>::Alloc(1 + mCertificateChain->GetLength());
    chain->Set(0, THIS_PROBE(ISignature));

    Int32 i = 1;
    for (; i < mCertificateChain->GetLength(); ++i) {
        AutoPtr<ArrayOf<Byte> > encoded;
        (*mCertificateChain)[i]->GetEncoded((ArrayOf<Byte>**)&encoded);
        AutoPtr<CSignature> sig;
        CSignature::NewByFriend(encoded, (CSignature**)&sig);
        chain->Set(i + 1, (ISignature*)sig.Get());
    }

    *result = chain;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CSignature::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);
    *isEqual = FALSE;

    ISignature* sg = ISignature::Probe(obj);

//    try {
    if (sg != NULL) {
        CSignature* other = (CSignature*)sg;
        if (this == other) {
            *isEqual = TRUE;
            return NOERROR;
        }
        if (mSignature->GetLength() == other->mSignature->GetLength()) {
            *isEqual = memcmp(mSignature->GetPayload(),
                    other->mSignature->GetPayload(), mSignature->GetLength()) == 0;
            return NOERROR;
        }
    }
//    } catch (ClassCastException e) {
//    }
    return NOERROR;
}

ECode CSignature::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    if (mHaveHashCode) {
        *hashCode = mHashCode;
        return NOERROR;
    }

    mHashCode = Arrays::GetHashCode(mSignature);
    mHaveHashCode = TRUE;
    *hashCode = mHashCode;
    return NOERROR;;
}

ECode CSignature::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return source->ReadArrayOf((Handle32*)&mSignature);
}

ECode CSignature::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteArrayOf((Handle32)mSignature.Get());
}

Boolean CSignature::AreExactMatch(
    /* [in] */ ArrayOf<ISignature*>* a,
    /* [in] */ ArrayOf<ISignature*>* b)
{
    return (a->GetLength() == b->GetLength()) && ArrayUtils::ContainsAll(a, b)
            && ArrayUtils::ContainsAll(b, a);
}

Boolean CSignature::AreEffectiveMatch(
    /* [in] */ ArrayOf<ISignature*>* a,
    /* [in] */ ArrayOf<ISignature*>* b)
{
    AutoPtr<ICertificateFactory> cf;
    AutoPtr<ICertificateFactoryHelper> helper;
    CCertificateFactoryHelper::AcquireSingleton((ICertificateFactoryHelper**)&helper);
    helper->GetInstance(String("X.509"), (ICertificateFactory**)&cf);

    AutoPtr<ArrayOf<ISignature*> > aPrime = ArrayOf<ISignature*>::Alloc(a->GetLength());
    for (Int32 i = 0; i < a->GetLength(); i++) {
        AutoPtr<ISignature> sig;
        Bounce(cf, (*a)[i], (ISignature**)&sig);
        aPrime->Set(i, sig);
    }

    AutoPtr<ArrayOf<ISignature*> > bPrime = ArrayOf<ISignature*>::Alloc(b->GetLength());
    for (Int32 i = 0; i < b->GetLength(); i++) {
        AutoPtr<ISignature> sig;
        Bounce(cf, (*b)[i], (ISignature**)&sig);
        bPrime->Set(i, sig);
    }

    return AreExactMatch(aPrime, bPrime);
}

ECode CSignature::Bounce(
    /* [in] */ ICertificateFactory* cf,
    /* [in] */ ISignature* s,
    /* [out] */ ISignature** sig)
{
    VALIDATE_NOT_NULL(sig)
    *sig = NULL;

    AutoPtr<IInputStream> is;
    CSignature* csg = (CSignature*)s;
    CByteArrayInputStream::New(csg->mSignature, (IInputStream**)&is);

    AutoPtr<ICertificate> ce;
    cf->GenerateCertificate(is, (ICertificate**)&ce);
    AutoPtr<IX509Certificate> cert = IX509Certificate::Probe(ce);
    VALIDATE_NOT_NULL(cert)

    AutoPtr<ArrayOf<Byte> > encoded;
    ce->GetEncoded((ArrayOf<Byte>**)&encoded);

    AutoPtr<CSignature> sPrime;
    CSignature::NewByFriend(encoded, (CSignature**)&sPrime);

    if (Elastos::Core::Math::Abs(sPrime->mSignature->GetLength() - csg->mSignature->GetLength()) > 2) {
        // throw new CertificateException("Bounced cert length looks fishy; before "
        //         + s.mSignature.length + ", after " + sPrime.mSignature.length);
        return E_CERTIFICATE_EXCEPTION;
    }

    *sig = (ISignature*)sPrime.Get();
    REFCOUNT_ADD(*sig);
    return NOERROR;
}


} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
