
#include "Elastos.CoreLibrary.Security.h"
#include "OpenSSLRSAPrivateCrtKey.h"
#include "OpenSSLRSAPrivateKey.h"

using Elastos::Security::Interfaces::IRSAPrivateKey;
using Elastos::Security::Interfaces::EIID_IRSAPrivateCrtKey;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLRSAPrivateCrtKey::
//=========================================
Int64 OpenSSLRSAPrivateCrtKey::mSerialVersionUID = 3785291944868707197L;

CAR_INTERFACE_IMPL_2(OpenSSLRSAPrivateCrtKey, OpenSSLRSAPrivateKey, IRSAPrivateCrtKey, IOpenSSLRSAPrivateCrtKey)

ECode OpenSSLRSAPrivateCrtKey::constructor(
    /* [in] */ IOpenSSLKey* key)
{
    OpenSSLRSAPrivateKey::constructor(key);
    return NOERROR;
}

ECode OpenSSLRSAPrivateCrtKey::constructor(
    /* [in] */ IOpenSSLKey* key,
    /* [in] */ ArrayOf<Handle32>* params)
{
    OpenSSLRSAPrivateKey::constructor(key, params);
    return NOERROR;
}

ECode OpenSSLRSAPrivateCrtKey::constructor(
    /* [in] */ IRSAPrivateCrtKeySpec* rsaKeySpec)
{
    OpenSSLRSAPrivateKey::constructor(Init(rsaKeySpec));
    return NOERROR;
}

AutoPtr<IOpenSSLKey> OpenSSLRSAPrivateCrtKey::Init(
    /* [in] */ IRSAPrivateCrtKeySpec* rsaKeySpec)
{
    // AutoPtr<IBigInteger> modulus;
    // IRSAPrivateKeySpec::Probe(rsaKeySpec)->GetModulus((IBigInteger**)&modulus);
    // AutoPtr<IBigInteger> privateExponent;
    // IRSAPrivateKeySpec::Probe(rsaKeySpec)->GetPrivateExponent((IBigInteger**)&privateExponent);

    // if (modulus == NULL) {
    //     // throw new InvalidKeySpecException("modulus == null");
    //     return NULL;
    // }
    // else if (privateExponent == NULL) {
    //     // throw new InvalidKeySpecException("privateExponent == null");
    //     return NULL;
    // }

    // // try {
    // /*
    //  * OpenSSL uses the public modulus to do RSA blinding. If
    //  * the public modulus is not available, the call to
    //  * EVP_PKEY_new_RSA will turn off blinding for this key
    //  * instance.
    //  */
    // AutoPtr<IBigInteger> publicExponent;
    // rsaKeySpec->GetPublicExponent((IBigInteger**)&publicExponent);
    // AutoPtr<IBigInteger> primeP;
    // rsaKeySpec->GetPrimeP((IBigInteger**)&primeP);
    // AutoPtr<IBigInteger> primeQ;
    // rsaKeySpec->GetPrimeQ((IBigInteger**)&primeQ);
    // AutoPtr<IBigInteger> primeExponentP;
    // rsaKeySpec->GetPrimeExponentP((IBigInteger**)&primeExponentP);
    // AutoPtr<IBigInteger> primeExponentQ;
    // rsaKeySpec->GetPrimeExponentQ((IBigInteger**)&primeExponentQ);
    // AutoPtr<IBigInteger> crtCoefficient;
    // rsaKeySpec->GetCrtCoefficient((IBigInteger**)&crtCoefficient);

    // AutoPtr<ArrayOf<Byte> > arrModulus;
    // modulus->ToByteArray((ArrayOf<Byte>**)&arrModulus);
    // AutoPtr<ArrayOf<Byte> > arrPublicExponent;
    // if (publicExponent != NULL) {
    //     publicExponent->ToByteArray((ArrayOf<Byte>**)&arrPublicExponent);
    // }
    // AutoPtr<ArrayOf<Byte> > arrPrivateExponent;
    // privateExponent->ToByteArray((ArrayOf<Byte>**)&arrPrivateExponent);
    // AutoPtr<ArrayOf<Byte> > arrPrimeP;
    // if (primeP != NULL) {
    //     primeP->ToByteArray((ArrayOf<Byte>**)&arrPrimeP);
    // }
    // AutoPtr<ArrayOf<Byte> > arrPrimeQ;
    // if (primeQ != NULL) {
    //     primeQ->ToByteArray((ArrayOf<Byte>**)&arrPrimeQ);
    // }
    // AutoPtr<ArrayOf<Byte> > arrPrimeExponentP;
    // if (primeExponentP != NULL) {
    //     primeExponentP->ToByteArray((ArrayOf<Byte>**)&arrPrimeExponentP);
    // }
    // AutoPtr<ArrayOf<Byte> > arrPrimeExponentQ;
    // if (primeExponentQ != NULL) {
    //     primeExponentQ->ToByteArray((ArrayOf<Byte>**)&arrPrimeExponentQ);
    // }
    // AutoPtr<ArrayOf<Byte> > arrCrtCoefficient;
    // if (crtCoefficient != NULL) {
    //     crtCoefficient->ToByteArray((ArrayOf<Byte>**)&arrCrtCoefficient);
    // }
    // Int64 res = 0;
    // NativeCrypto::EVP_PKEY_new_RSA(
    //         arrModulus,
    //         arrPublicExponent,
    //         arrPrivateExponent,
    //         arrPrimeP,
    //         arrPrimeQ,
    //         arrPrimeExponentP,
    //         arrPrimeExponentQ,
    //         arrCrtCoefficient,
    //         &res);
    // AutoPtr<OpenSSLKey> p = new OpenSSLKey(res);
    return NULL;
    // } catch (Exception e) {
    //     throw new InvalidKeySpecException(e);
    // }
}

ECode OpenSSLRSAPrivateCrtKey::GetInstance(
    /* [in] */ IRSAPrivateCrtKey* rsaPrivateKey,
    /* [out] */ IOpenSSLKey** result)
{
    assert(0 && "TODO");
    // VALIDATE_NOT_NULL(result)
    // /**
    //  * If the key is not encodable (PKCS11-like key), then wrap it and use
    //  * JNI upcalls to satisfy requests.
    //  */
    // if (rsaPrivateKey->GetFormat() == NULL) {
    //     return WrapPlatformKey(rsaPrivateKey, result);
    // }

    // AutoPtr<IBigInteger> modulus;
    // rsaPrivateKey->GetModulus((IBigInteger**)&modulus);
    // AutoPtr<IBigInteger> privateExponent;
    // rsaPrivateKey->GetPrivateExponent((IBigInteger**)&privateExponent);

    // if (modulus == NULL) {
    //     // throw new InvalidKeyException("modulus == NULL");
    //     return NOERROR;
    // }
    // else if (privateExponent == NULL) {
    //     // throw new InvalidKeyException("privateExponent == NULL");
    //     return NOERROR;
    // }

    // // try {
    // /*
    //  * OpenSSL uses the public modulus to do RSA blinding. If
    //  * the public modulus is not available, the call to
    //  * EVP_PKEY_new_RSA will turn off blinding for this key
    //  * instance.
    //  */
    // AutoPtr<IBigInteger> publicExponent;
    // rsaPrivateKey->GetPublicExponent((IBigInteger**)&publicExponent);
    // AutoPtr<IBigInteger> primeP;
    // rsaPrivateKey->GetPrimeP((IBigInteger**)&primeP);
    // AutoPtr<IBigInteger> primeQ;
    // rsaPrivateKey->GetPrimeQ((IBigInteger**)&primeQ);
    // AutoPtr<IBigInteger> primeExponentP;
    // rsaPrivateKey->GetPrimeExponentP((IBigInteger**)&primeExponentP);
    // AutoPtr<IBigInteger> primeExponentQ;
    // rsaPrivateKey->GetPrimeExponentQ((IBigInteger**)&primeExponentQ);
    // AutoPtr<IBigInteger> crtCoefficient;
    // rsaPrivateKey->GetCrtCoefficient((IBigInteger**)&crtCoefficient);

    // AutoPtr<ArrayOf<Byte> > arrModulus;
    // modulus->ToByteArray((ArrayOf<Byte>**)&arrModulus);
    // AutoPtr<ArrayOf<Byte> > arrPublicExponent;
    // if (publicExponent != NULL) {
    //     publicExponent->ToByteArray((ArrayOf<Byte>**)&arrPublicExponent);
    // }
    // AutoPtr<ArrayOf<Byte> > arrPrivateExponent;
    // privateExponent->ToByteArray((ArrayOf<Byte>**)&arrPrivateExponent);
    // AutoPtr<ArrayOf<Byte> > arrPrimeP;
    // if (primeP != NULL) {
    //     primeP->ToByteArray((ArrayOf<Byte>**)&arrPrimeP);
    // }
    // AutoPtr<ArrayOf<Byte> > arrPrimeQ;
    // if (primeQ != NULL) {
    //     primeQ->ToByteArray((ArrayOf<Byte>**)&arrPrimeQ);
    // }
    // AutoPtr<ArrayOf<Byte> > arrPrimeExponentP;
    // if (primeExponentP != NULL) {
    //     primeExponentP->ToByteArray((ArrayOf<Byte>**)&arrPrimeExponentP);
    // }
    // AutoPtr<ArrayOf<Byte> > arrPrimeExponentQ;
    // if (primeExponentQ != NULL) {
    //     primeExponentQ->ToByteArray((ArrayOf<Byte>**)&arrPrimeExponentQ);
    // }
    // AutoPtr<ArrayOf<Byte> > arrCrtCoefficient;
    // if (crtCoefficient != NULL) {
    //     crtCoefficient->ToByteArray((ArrayOf<Byte>**)&arrCrtCoefficient);
    // }
    // Int64 res = 0;
    // NativeCrypto::EVP_PKEY_new_RSA(
    //         arrModulus,
    //         arrPublicExponent,
    //         arrPrivateExponent,
    //         arrPrimeP,
    //         arrPrimeQ,
    //         arrPrimeExponentP,
    //         arrPrimeExponentQ,
    //         arrCrtCoefficient,
    //         &res);
    // AutoPtr<OpenSSLKey> p = new OpenSSLKey(res);
    // *result = p;
    // REFCOUNT_ADD(*result)
    return NOERROR;
    // } catch (Exception e) {
    //     throw new InvalidKeyException(e);
    // }
}

void OpenSSLRSAPrivateCrtKey::ReadParams(
    /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* params)
{
    assert(0 && "TODO");
    // OpenSSLRSAPrivateKey::ReadParams(params);
    // // params[0] read in super.readParams
    // if ((*params)[1] != NULL) {
    //     CBigInteger::New((*params)[1], (IBigInteger**)&mPublicExponent);
    // }
    // // params[2] read in super.readParams
    // if ((*params)[3] != NULL) {
    //     CBigInteger::New((*params)[3], (IBigInteger**)&mPrimeP);
    // }
    // if ((*params)[4] != NULL) {
    //     CBigInteger::New((*params)[4], (IBigInteger**)&mPrimeQ);
    // }
    // if ((*params)[5] != NULL) {
    //     CBigInteger::New((*params)[5], (IBigInteger**)&mPrimeExponentP);
    // }
    // if ((*params)[6] != NULL) {
    //     CBigInteger::New((*params)[6], (IBigInteger**)&mPrimeExponentQ);
    // }
    // if ((*params)[7] != NULL) {
    //     CBigInteger::New((*params)[7], (IBigInteger**)&mCrtCoefficient);
    // }
}

ECode OpenSSLRSAPrivateCrtKey::GetPublicExponent(
    /* [out] */ IBigInteger** result)
{
    assert(0 && "TODO");
    // VALIDATE_NOT_NULL(result)
    // EnsureReadParams();
    // *result = mPublicExponent;
    // REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAPrivateCrtKey::GetPrimeP(
    /* [out] */ IBigInteger** result)
{
    assert(0 && "TODO");
    // VALIDATE_NOT_NULL(result)
    // EnsureReadParams();
    // *result = mPrimeP;
    // REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAPrivateCrtKey::GetPrimeQ(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // EnsureReadParams();
    // *result = mPrimeQ;
    // REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAPrivateCrtKey::GetPrimeExponentP(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // EnsureReadParams();
    // *result = mPrimeExponentP;
    // REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAPrivateCrtKey::GetPrimeExponentQ(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // EnsureReadParams();
    // *result = mPrimeExponentQ;
    // REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAPrivateCrtKey::GetCrtCoefficient(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // EnsureReadParams();
    // *result = mCrtCoefficient;
    // REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAPrivateCrtKey::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    assert(0 && "TODO");
    // VALIDATE_NOT_NULL(result)
    // if (o == THIS_PROBE(IInterface)) {
    //     *result = TRUE;
    //     return NOERROR;
    // }

    // if (IOpenSSLRSAPrivateKey::Probe(o) != NULL) {
    //     AutoPtr<IOpenSSLRSAPrivateKey> other = IOpenSSLRSAPrivateKey::Probe(o);
    //     return GetOpenSSLKey()->Equals(other->GetOpenSSLKey());
    // }

    // if (IRSAPrivateCrtKey::Probe(o) != NULL) {
    //     EnsureReadParams();
    //     AutoPtr<IRSAPrivateCrtKey> other = IRSAPrivateCrtKey::Probe(o);

    //     if (GetOpenSSLKey()->IsEngineBased()) {
    //         Boolean bModulus = FALSE, bPublicExponent = FALSE;
    //         GetModulus()->Equals(other->GetModulus(), &bModulus);
    //         AutoPtr<IBigInteger> otherPublicExponent;
    //         other->GetPublicExponent((IBigInteger**)&otherPublicExponent);
    //         mPublicExponent->Equals(otherPublicExponent, &bPublicExponent);
    //         return bModulus && bPublicExponent;
    //     }
    //     else {
    //         Boolean bModulus = FALSE, bPublicExponent = FALSE;
    //         Boolean bPrivateExponent = FALSE, bPrimeP = FALSE;
    //         Boolean bPrimeQ = FALSE, bPrimeExponentP = FALSE;
    //         Boolean bPrimeExponentQ = FALSE, bCrtCoefficient = FALSE;
    //         GetModulus()->Equals(other->GetModulus(), &bModulus);
    //         AutoPtr<IBigInteger> otherPublicExponent;
    //         other->GetPublicExponent((IBigInteger**)&otherPublicExponent);
    //         mPublicExponent->Equals(otherPublicExponent, &bPublicExponent);
    //         AutoPtr<IBigInteger> otherPrivateExponent;
    //         other->GetPrivateExponent((IBigInteger**)&otherPrivateExponent);
    //         GetPrivateExponent()->Equals(otherPrivateExponent, &bPrivateExponent);
    //         AutoPtr<IBigInteger> otherPrimeP;
    //         other->GetPrimeP((IBigInteger**)&otherPrimeP);
    //         mPrimeP->Equals(otherPrimeP, &bPrimeP);
    //         AutoPtr<IBigInteger> otherPrimeQ;
    //         other->GetPrimeQ((IBigInteger**)&otherPrimeQ);
    //         mPrimeQ->Equals(otherPrimeQ, &bPrimeQ);
    //         AutoPtr<IBigInteger> otherPrimeExponentP;
    //         other->GetPrimeExponentP((IBigInteger**)&otherPrimeExponentP);
    //         mPrimeExponentP->Equals(otherPrimeExponentP, &bPrimeExponentP);
    //         AutoPtr<IBigInteger> otherPrimeExponentQ;
    //         other->GetPrimeExponentQ((IBigInteger**)&otherPrimeExponentQ);
    //         mPrimeExponentQ->Equals(otherPrimeExponentQ, &bPrimeExponentQ);
    //         AutoPtr<IBigInteger> otherCrtCoefficient;
    //         other->GetCrtCoefficient((IBigInteger**)&otherCrtCoefficient);
    //         mCrtCoefficient->Equals(otherCrtCoefficient, &bCrtCoefficient);
    //         return bModulus && bPublicExponent && bPrivateExponent &&
    //              bPrimeP && bPrimeQ && bPrimeExponentP && bPrimeExponentQ && bCrtCoefficient;
    //     }
    // }
    // else if (IRSAPrivateKey::Probe(o) != NULL) {
    //     EnsureReadParams();
    //     AutoPtr<IRSAPrivateKey> other = IRSAPrivateKey::Probe(o);

    //     assert(0 && "TODO");
    //     // if (GetOpenSSLKey()->IsEngineBased()) {
    //     //     return GetModulus()->Equals(other->GetModulus());
    //     // }
    //     // else {
    //     //     Boolean bPrivateExponent = FALSE;
    //     //     AutoPtr<IBigInteger> otherPrivateExponent;
    //     //     other->GetPrivateExponent((IBigInteger**)&otherPrivateExponent);
    //     //     GetPrivateExponent()->Equals(otherPrivateExponent, &bPrivateExponent);
    //     //     return GetModulus()->Equals(other->GetModulus())
    //     //             && bPrivateExponent;
    //     // }
    // }

    // *result = FALSE;
    return NOERROR;
}

ECode OpenSSLRSAPrivateCrtKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // Int32 hashCode = 0;
    // assert(0 && "TODO");
    // // OpenSSLRSAPrivateKey::GetHashCode(&hashCode);
    // if (mPublicExponent != NULL) {
    //     Int32 hc = 0;
    //     mPublicExponent->GetHashCode(&hc);
    //     hashCode ^= hc;
    // }
    // *result = hashCode;
    return NOERROR;
}

ECode OpenSSLRSAPrivateCrtKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    assert(0 && "TODO");
    // StringBuilder sb("OpenSSLRSAPrivateCrtKey{");

    // Boolean engineBased = GetOpenSSLKey()->IsEngineBased();
    // if (engineBased) {
    //     sb.Append("key=");
    //     sb.Append(GetOpenSSLKey());
    //     sb.Append('}');
    // }

    // EnsureReadParams();
    // sb.Append("modulus=");
    // String strModulus;
    // GetModulus()->ToString(16, &strModulus);
    // sb.Append(strModulus);
    // sb.Append(',');

    // if (mPublicExponent != NULL) {
    //     sb.Append("publicExponent=");
    //     String str;
    //     mPublicExponent->ToString(16, &str);
    //     sb.Append(str);
    //     sb.Append(',');
    // }

    // if (!engineBased) {
    //     sb.Append("privateExponent=");
    //     String str;
    //     GetPrivateExponent()->ToString(16, &str);
    //     sb.Append(str);
    //     sb.Append(',');
    // }

    // if (mPrimeP != NULL) {
    //     sb.Append("primeP=");
    //     String str;
    //     mPrimeP->ToString(16, &str);
    //     sb.Append(str);
    //     sb.Append(',');
    // }

    // if (mPrimeQ != NULL) {
    //     sb.Append("primeQ=");
    //     String str;
    //     mPrimeQ->ToString(16, &str);
    //     sb.Append(str);
    //     sb.Append(',');
    // }

    // if (mPrimeExponentP != NULL) {
    //     sb.Append("primeExponentP=");
    //     String str;
    //     mPrimeExponentP->ToString(16, &str);
    //     sb.Append(str);
    //     sb.Append(',');
    // }

    // if (mPrimeExponentQ != NULL) {
    //     sb.Append("primeExponentQ=");
    //     String str;
    //     mPrimeExponentQ->ToString(16, &str);
    //     sb.Append(str);
    //     sb.Append(',');
    // }

    // if (mCrtCoefficient != NULL) {
    //     sb.Append("crtCoefficient=");
    //     String str;
    //     mCrtCoefficient->ToString(16, &str);
    //     sb.Append(str);
    //     sb.Append(',');
    // }

    // return sb.ToString();
    return NOERROR;
}

void OpenSSLRSAPrivateCrtKey::ReadObject(
    /* [in] */ IObjectInputStream* stream)
{
    assert(0 && "TODO");
    // stream->DefaultReadObject();

    // AutoPtr<ArrayOf<Byte> > arrModulus;
    // mModulus->ToByteArray((ArrayOf<Byte>**)&arrModulus);
    // AutoPtr<ArrayOf<Byte> > arrPublicExponent;
    // if (mPublicExponent != NULL) {
    //     mPublicExponent->ToByteArray((ArrayOf<Byte>**)&arrPublicExponent);
    // }
    // AutoPtr<ArrayOf<Byte> > arrPrivateExponent;
    // mPrivateExponent->ToByteArray((ArrayOf<Byte>**)&arrPrivateExponent);
    // AutoPtr<ArrayOf<Byte> > arrPrimeP;
    // if (mPrimeP != NULL) {
    //     mPrimeP->ToByteArray((ArrayOf<Byte>**)&arrPrimeP);
    // }
    // AutoPtr<ArrayOf<Byte> > arrPrimeQ;
    // if (mPrimeQ != NULL) {
    //     mPrimeQ->ToByteArray((ArrayOf<Byte>**)&arrPrimeQ);
    // }
    // AutoPtr<ArrayOf<Byte> > arrPrimeExponentP;
    // if (mPrimeExponentP != NULL) {
    //     mPrimeExponentP->ToByteArray((ArrayOf<Byte>**)&arrPrimeExponentP);
    // }
    // AutoPtr<ArrayOf<Byte> > arrPrimeExponentQ;
    // if (mPrimeExponentQ != NULL) {
    //     mPrimeExponentQ->ToByteArray((ArrayOf<Byte>**)&arrPrimeExponentQ);
    // }
    // AutoPtr<ArrayOf<Byte> > arrCrtCoefficient;
    // if (mCrtCoefficient != NULL) {
    //     mCrtCoefficient->ToByteArray((ArrayOf<Byte>**)&arrCrtCoefficient);
    // }
    // Int64 res = 0;
    // NativeCrypto::EVP_PKEY_new_RSA(
    //         arrModulus,
    //         arrPublicExponent,
    //         arrPrivateExponent,
    //         arrPrimeP,
    //         arrPrimeQ,
    //         arrPrimeExponentP,
    //         arrPrimeExponentQ,
    //         arrCrtCoefficient,
    //         &res);
    // key = new OpenSSLKey(res);
    // fetchedParams = TRUE;
}

void OpenSSLRSAPrivateCrtKey::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    assert(0 && "TODO");
    // if (GetOpenSSLKey()->IsEngineBased()) {
    //     // throw new NotSerializableException("engine-based keys can not be serialized");
    //     return;
    // }

    // EnsureReadParams();
    // stream->DefaultWriteObject();
}

} // namespace Conscrypt
} // namespace Org
