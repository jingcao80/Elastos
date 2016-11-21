
#include "Elastos.CoreLibrary.Security.h"
#include "Elastos.CoreLibrary.IO.h"
#include "OpenSSLRSAPrivateKey.h"
#include "NativeCrypto.h"
#include "Platform.h"
#include "COpenSSLKey.h"
#include "COpenSSLRSAPrivateCrtKey.h"
#include "COpenSSLRSAPrivateKey.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::CoreUtils;
using Elastos::Core::IByte;
using Elastos::Core::StringBuilder;
using Elastos::Math::CBigInteger;
using Elastos::Security::IKey;
using Elastos::Security::Interfaces::IRSAKey;
using Elastos::Security::Interfaces::EIID_IRSAPrivateKey;
using Elastos::Security::Interfaces::EIID_IRSAKey;
using Elastos::Security::EIID_IPrivateKey;
using Elastos::Security::EIID_IKey;
using Elastos::IO::EIID_ISerializable;
using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLRSAPrivateKey::
//=========================================
Int64 OpenSSLRSAPrivateKey::mSerialVersionUID = 4872170254439578735L;

CAR_INTERFACE_IMPL_7(OpenSSLRSAPrivateKey, Object, IKey, IPrivateKey, IRSAKey, IRSAPrivateKey, IOpenSSLKeyHolder, IOpenSSLRSAPrivateKey, ISerializable)

ECode OpenSSLRSAPrivateKey::constructor(
    /* [in] */ IOpenSSLKey* key)
{
    mKey = key;
    return NOERROR;
}

ECode OpenSSLRSAPrivateKey::constructor(
    /* [in] */ IOpenSSLKey* key,
    /* [in] */ ArrayOf<IArrayOf*>* params)
{
    constructor(key);
    mKey = key;

    AutoPtr<ArrayOf<AutoPtr<ArrayOf<Byte> > > > array = ArrayOf<AutoPtr<ArrayOf<Byte> > >::Alloc(params->GetLength());
    for (Int32 i = 0; i < params->GetLength(); i++) {
        AutoPtr<IArrayOf> iArrayOf = (*params)[i];
        Int32 len;
        iArrayOf->GetLength(&len);
        AutoPtr<ArrayOf<Byte> > bytes = ArrayOf<Byte>::Alloc(len);
        for (Int32 j = 0; j < len; j++) {
            AutoPtr<IInterface> obj;
            iArrayOf->Get(j, (IInterface**)&obj);
            AutoPtr<IByte> iB = IByte::Probe(obj);
            iB->GetValue(&(*bytes)[j]);
        }
        array->Set(i, bytes);
    }

    ReadParams(array);
    mFetchedParams = TRUE;
    return NOERROR;
}

ECode OpenSSLRSAPrivateKey::GetOpenSSLKey(
    /* [out] */ IOpenSSLKey** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mKey;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAPrivateKey::constructor(
    /* [in] */ IRSAPrivateKeySpec* rsaKeySpec)
{
    constructor(Init(rsaKeySpec));
    return NOERROR;
}

AutoPtr<IOpenSSLKey> OpenSSLRSAPrivateKey::Init(
    /* [in] */ IRSAPrivateKeySpec* rsaKeySpec)
{
    AutoPtr<IBigInteger> modulus;
    rsaKeySpec->GetModulus((IBigInteger**)&modulus);
    AutoPtr<IBigInteger> privateExponent;
    rsaKeySpec->GetPrivateExponent((IBigInteger**)&privateExponent);

    if (modulus == NULL) {
        // throw new InvalidKeySpecException("modulus == null");
        return NULL;
    }
    else if (privateExponent == NULL) {
        // throw new InvalidKeySpecException("privateExponent == null");
        return NULL;
    }

    AutoPtr<ArrayOf<Byte> > mArr;
    modulus->ToByteArray((ArrayOf<Byte>**)&mArr);
    AutoPtr<ArrayOf<Byte> > pArr;
    privateExponent->ToByteArray((ArrayOf<Byte>**)&pArr);
    Int64 rsa = 0;
    NativeCrypto::EVP_PKEY_new_RSA(
           mArr,
           NULL,
           pArr,
           NULL,
           NULL,
           NULL,
           NULL,
           NULL,
           &rsa);
    AutoPtr<IOpenSSLKey> res;
    COpenSSLKey::New(rsa, (IOpenSSLKey**)&res);
    return res;
}

AutoPtr<IOpenSSLRSAPrivateKey> OpenSSLRSAPrivateKey::GetInstance(
    /* [in] */ IOpenSSLKey* key)
{
    Int64 keyContext = 0;
    key->GetPkeyContext(&keyContext);
    AutoPtr<ArrayOf<AutoPtr<ArrayOf<Byte> > > > params = ArrayOf<AutoPtr<ArrayOf<Byte> > >::Alloc(8);
    NativeCrypto::Get_RSA_private_params(
            keyContext,
            (ArrayOf<Byte>**)&((*params)[0]),
            (ArrayOf<Byte>**)&((*params)[1]),
            (ArrayOf<Byte>**)&((*params)[2]),
            (ArrayOf<Byte>**)&((*params)[3]),
            (ArrayOf<Byte>**)&((*params)[4]),
            (ArrayOf<Byte>**)&((*params)[5]),
            (ArrayOf<Byte>**)&((*params)[6]),
            (ArrayOf<Byte>**)&((*params)[7]));

    AutoPtr<ArrayOf<IArrayOf*> > array = ArrayOf<IArrayOf*>::Alloc(params->GetLength());
    for (Int32 i = 0; i < params->GetLength(); i++) {
        array->Set(i, CoreUtils::ConvertByteArray((*params)[i]));
    }

    if ((*params)[1] != NULL) {
        AutoPtr<IOpenSSLRSAPrivateKey> res;
        COpenSSLRSAPrivateCrtKey::New(key, array, (IOpenSSLRSAPrivateKey**)&res);
        return res;
    }
    AutoPtr<IOpenSSLRSAPrivateKey> res;
    COpenSSLRSAPrivateKey::New(key, array, (IOpenSSLRSAPrivateKey**)&res);
    return res;
}

AutoPtr<IOpenSSLKey> OpenSSLRSAPrivateKey::WrapPlatformKey(
    /* [in] */ IRSAPrivateKey* rsaPrivateKey)
{
    AutoPtr<IOpenSSLKey> wrapper;
    Platform::WrapRsaKey(IPrivateKey::Probe(rsaPrivateKey), (IOpenSSLKey**)&wrapper);
    if (wrapper != NULL) {
        return wrapper;
    }
    AutoPtr<IBigInteger> modulus;
    IRSAKey::Probe(rsaPrivateKey)->GetModulus((IBigInteger**)&modulus);
    AutoPtr<ArrayOf<Byte> > arr;
    modulus->ToByteArray((ArrayOf<Byte>**)&arr);
    Int64 keywrapper = 0;
    NativeCrypto::GetRSAPrivateKeyWrapper(rsaPrivateKey, arr, &keywrapper);
    AutoPtr<IOpenSSLKey> res;
    COpenSSLKey::New(keywrapper, (IOpenSSLKey**)&res);
    return res;
}

AutoPtr<IOpenSSLKey> OpenSSLRSAPrivateKey::GetInstance(
    /* [in] */ IRSAPrivateKey* rsaPrivateKey)
{
    /**
     * If the key is not encodable (PKCS11-like key), then wrap it and use
     * JNI upcalls to satisfy requests.
     */
    String fm;
    IKey::Probe(rsaPrivateKey)->GetFormat(&fm);
    if (fm.IsNull()) {
        return WrapPlatformKey(rsaPrivateKey);
    }

    AutoPtr<IBigInteger> modulus;
    IRSAKey::Probe(rsaPrivateKey)->GetModulus((IBigInteger**)&modulus);
    AutoPtr<IBigInteger> privateExponent;
    rsaPrivateKey->GetPrivateExponent((IBigInteger**)&privateExponent);

    if (modulus == NULL) {
        // throw new InvalidKeyException("modulus == null");
        return NULL;
    }
    else if (privateExponent == NULL) {
        // throw new InvalidKeyException("privateExponent == null");
        return NULL;
    }

    AutoPtr<ArrayOf<Byte> > mArr;
    modulus->ToByteArray((ArrayOf<Byte>**)&mArr);
    AutoPtr<ArrayOf<Byte> > pArr;
    privateExponent->ToByteArray((ArrayOf<Byte>**)&pArr);
    Int64 rsa = 0;
    NativeCrypto::EVP_PKEY_new_RSA(
           mArr,
           NULL,
           pArr,
           NULL,
           NULL,
           NULL,
           NULL,
           NULL,
           &rsa);
    AutoPtr<IOpenSSLKey> res;
    COpenSSLKey::New(rsa, (IOpenSSLKey**)&res);
    return res;
}

ECode OpenSSLRSAPrivateKey::EnsureReadParams()
{
    if (mFetchedParams) {
        return NOERROR;
    }
    Int64 keyContext = 0;
    mKey->GetPkeyContext(&keyContext);
    AutoPtr<ArrayOf<AutoPtr<ArrayOf<Byte> > > > params = ArrayOf<AutoPtr<ArrayOf<Byte> > >::Alloc(8);
    NativeCrypto::Get_RSA_private_params(
        keyContext,
        (ArrayOf<Byte>**)&((*params)[0]),
        (ArrayOf<Byte>**)&((*params)[1]),
        (ArrayOf<Byte>**)&((*params)[2]),
        (ArrayOf<Byte>**)&((*params)[3]),
        (ArrayOf<Byte>**)&((*params)[4]),
        (ArrayOf<Byte>**)&((*params)[5]),
        (ArrayOf<Byte>**)&((*params)[6]),
        (ArrayOf<Byte>**)&((*params)[7]));
    ReadParams(params);
    mFetchedParams = TRUE;
    return NOERROR;
}

ECode OpenSSLRSAPrivateKey::ReadParams(
    /* [in] */ ArrayOf<AutoPtr<ArrayOf<Byte> > >* params)
{
    Boolean bBased = FALSE;
    if ((*params)[0] == NULL) {
        // throw new NullPointerException("modulus == null");
        return E_NULL_POINTER_EXCEPTION;
    }
    else if ((*params)[2] == NULL && !(mKey->IsEngineBased(&bBased), bBased)) {
        // throw new NullPointerException("privateExponent == null");
        return E_NULL_POINTER_EXCEPTION;
    }

    CBigInteger::New(*((*params)[0]), (IBigInteger**)&mModulus);

    // ENGINE-based keys are not guaranteed to have a private exponent.
    if ((*params)[2] != NULL) {
        CBigInteger::New(*((*params)[2]), (IBigInteger**)&mPrivateExponent);
    }
    return NOERROR;
}

ECode OpenSSLRSAPrivateKey::GetPrivateExponent(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    Boolean bBased = FALSE;
    mKey->IsEngineBased(&bBased);
    if (bBased) {
        // throw new UnsupportedOperationException("private exponent cannot be extracted");
        return E_UNSUPPORTED_OPERATION_EXCEPTION;
    }

    EnsureReadParams();
    *result = mPrivateExponent;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAPrivateKey::GetModulus(
    /* [out] */ IBigInteger** result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    *result = mModulus;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode OpenSSLRSAPrivateKey::GetEncoded(
    /* [out] */ ArrayOf<Byte>** result)
{
    VALIDATE_NOT_NULL(result)
    /*
     * If we're using an OpenSSL ENGINE, there's no guarantee we can export
     * the key. Returning {@code null} tells the caller that there's no
     * encoded format.
     */
    Boolean bBased = FALSE;
    mKey->IsEngineBased(&bBased);
    if (bBased) {
        *result = NULL;
        return NOERROR;
    }

    Int64 keyContext = 0;
    mKey->GetPkeyContext(&keyContext);
    return NativeCrypto::I2d_PKCS8_PRIV_KEY_INFO(keyContext, result);
}

ECode OpenSSLRSAPrivateKey::GetFormat(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    /*
     * If we're using an OpenSSL ENGINE, there's no guarantee we can export
     * the key. Returning {@code null} tells the caller that there's no
     * encoded format.
     */
    Boolean bBased = FALSE;
    mKey->IsEngineBased(&bBased);
    if (bBased) {
        *result = NULL;
        return NOERROR;
    }

    *result = String("PKCS#8");
    return NOERROR;
}

ECode OpenSSLRSAPrivateKey::GetAlgorithm(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = String("RSA");
    return NOERROR;
}

ECode OpenSSLRSAPrivateKey::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (o == this->Probe(EIID_IInterface)) {
        *result = TRUE;
        return NOERROR;
    }

    if (IOpenSSLRSAPrivateKey::Probe(o) != NULL) {
        AutoPtr<IOpenSSLRSAPrivateKey> other = IOpenSSLRSAPrivateKey::Probe(o);
        AutoPtr<IOpenSSLKey> key;
        IOpenSSLKeyHolder::Probe(other)->GetOpenSSLKey((IOpenSSLKey**)&key);
        return IObject::Probe(mKey)->Equals(key, result);
    }

    if (IRSAPrivateKey::Probe(o) != NULL) {
        EnsureReadParams();
        AutoPtr<IRSAPrivateKey> other = IRSAPrivateKey::Probe(o);

        AutoPtr<IBigInteger> otherPrivateExponent;
        other->GetPrivateExponent((IBigInteger**)&otherPrivateExponent);
        AutoPtr<IBigInteger> modulus;
        IRSAKey::Probe(other)->GetModulus((IBigInteger**)&modulus);
        Boolean bModulus = FALSE, bExponent = FALSE;
        mModulus->Equals(modulus, &bModulus);
        mPrivateExponent->Equals(otherPrivateExponent, &bExponent);
        *result = bModulus && bExponent;
        return NOERROR;
    }

    *result = FALSE;
    return NOERROR;
}

ECode OpenSSLRSAPrivateKey::GetHashCode(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    EnsureReadParams();
    Int32 hash = 1;

    Int32 modulusHC = 0;
    IObject::Probe(mModulus)->GetHashCode(&modulusHC);
    hash = hash * 3 + modulusHC;
    if (mPrivateExponent != NULL) {
        Int32 hc = 0;
        IObject::Probe(mPrivateExponent)->GetHashCode(&hc);
        hash = hash * 7 + hc;
    }

    *result = hash;
    return NOERROR;
}

ECode OpenSSLRSAPrivateKey::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    StringBuilder sb("OpenSSLRSAPrivateKey{");

    Boolean engineBased = FALSE;
    mKey->IsEngineBased(&engineBased);
    if (engineBased) {
        sb.Append("key=");
        sb.Append(mKey);
        sb.Append('}');
    }

    EnsureReadParams();
    sb.Append("modulus=");
    String strModulus;
    mModulus->ToString(16, &strModulus);
    sb.Append(strModulus);
    sb.Append(',');

    if (!engineBased) {
        sb.Append("privateExponent=");
        String strPrivateExponent;
        mPrivateExponent->ToString(16, &strPrivateExponent);
        sb.Append(strPrivateExponent);
        sb.Append(',');
    }

    *result = sb.ToString();
    return NOERROR;
}

void OpenSSLRSAPrivateKey::ReadObject(
    /* [in] */ IObjectInputStream* stream)
{
    stream->DefaultReadObject();

    AutoPtr<ArrayOf<Byte> > mArr;
    mModulus->ToByteArray((ArrayOf<Byte>**)&mArr);
    AutoPtr<ArrayOf<Byte> > pArr;
    mPrivateExponent->ToByteArray((ArrayOf<Byte>**)&pArr);
    Int64 rsa = 0;
    NativeCrypto::EVP_PKEY_new_RSA(
            mArr,
            NULL,
            pArr,
            NULL,
            NULL,
            NULL,
            NULL,
            NULL,
            &rsa);
    COpenSSLKey::New(rsa, (IOpenSSLKey**)&mKey);
    mFetchedParams = TRUE;
}

void OpenSSLRSAPrivateKey::WriteObject(
    /* [in] */ IObjectOutputStream* stream)
{
    AutoPtr<IOpenSSLKey> key;
    GetOpenSSLKey((IOpenSSLKey**)&key);
    Boolean bBased = FALSE;
    key->IsEngineBased(&bBased);
    if (bBased) {
        // throw new NotSerializableException("engine-based keys can not be serialized");
        return;
    }

    EnsureReadParams();
    stream->DefaultWriteObject();
}

} // namespace Conscrypt
} // namespace Org
