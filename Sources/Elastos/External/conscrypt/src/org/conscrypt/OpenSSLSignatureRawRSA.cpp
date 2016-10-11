
#include "Elastos.CoreLibrary.Security.h"
#include "OpenSSLSignatureRawRSA.h"
#include "NativeCrypto.h"
#include "OpenSSLRSAPrivateCrtKey.h"
#include "OpenSSLRSAPrivateKey.h"
#include "OpenSSLRSAPublicKey.h"

using Elastos::Security::Interfaces::IRSAPrivateCrtKey;
using Elastos::Security::Interfaces::IRSAPublicKey;

namespace Org {
namespace Conscrypt {

//=========================================
// OpenSSLSignatureRawRSA::
//=========================================
CAR_INTERFACE_IMPL(OpenSSLSignatureRawRSA, Object, IOpenSSLSignatureRawRSA)

ECode OpenSSLSignatureRawRSA::EngineUpdate(
    /* [in] */ Byte input)
{
    Int32 oldOffset = mInputOffset++;

    if (mInputOffset > mInputBuffer->GetLength()) {
        mInputIsTooLong = TRUE;
        return;
    }

    (*mInputBuffer)[oldOffset] = input;
    return NOERROR;
}

ECode OpenSSLSignatureRawRSA::EngineUpdate(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 len)
{
    Int32 oldOffset = mInputOffset;
    mInputOffset += len;

    if (mInputOffset > mInputBuffer->GetLength()) {
        mInputIsTooLong = TRUE;
        return NOERROR;
    }

    mInputBuffer->Copy(oldOffset, input, offset, len);
    return NOERROR;
}

ECode OpenSSLSignatureRawRSA::EngineGetParameter(
    /* [in] */ const String& param,
    /* [out] */ IInterface** value)
{
    VALIDATE_NOT_NULL(*value)
    *value = NULL;
    return NOERROR;
}

ECode OpenSSLSignatureRawRSA::EngineInitSign(
    /* [in] */ IPrivateKey* privateKey)
{
    if (IOpenSSLRSAPrivateKey::Probe(privateKey) != NULL) {
        AutoPtr<IOpenSSLRSAPrivateKey> rsaPrivateKey = IOpenSSLRSAPrivateKey::Probe(privateKey);
        IOpenSSLKeyHolder::Probe(rsaPrivateKey)->GetOpenSSLKey((IOpenSSLKey**)&mKey);
    }
    else if (IRSAPrivateCrtKey::Probe(privateKey) != NULL) {
        AutoPtr<IRSAPrivateCrtKey> rsaPrivateKey = IRSAPrivateCrtKey::Probe(privateKey);
        OpenSSLRSAPrivateCrtKey::GetInstance(rsaPrivateKey, (IOpenSSLKey**)&mKey);
    }
    else if (IRSAPrivateKey::Probe(privateKey) != NULL) {
        AutoPtr<IRSAPrivateKey> rsaPrivateKey = IRSAPrivateKey::Probe(privateKey);
        mKey = OpenSSLRSAPrivateKey::GetInstance(rsaPrivateKey);
    }
    else {
        // throw new InvalidKeyException("Need RSA private key");
        return NOERROR;
    }

    // Allocate buffer according to RSA modulus size.
    Int64 context = 0;
    mKey->GetPkeyContext(&context);
    Int32 maxSize = 0;
    NativeCrypto::RSA_size(context, &maxSize);
    mInputBuffer = ArrayOf<Byte>::Alloc(maxSize);
    mInputOffset = 0;
    return NOERROR;
}

ECode OpenSSLSignatureRawRSA::EngineInitVerify(
    /* [in] */ IPublicKey* publicKey)
{
    if (IOpenSSLRSAPublicKey::Probe(publicKey) != NULL) {
        AutoPtr<IOpenSSLRSAPublicKey> rsaPublicKey = IOpenSSLRSAPublicKey::Probe(publicKey);
        IOpenSSLKeyHolder::Probe(rsaPublicKey)->GetOpenSSLKey((IOpenSSLKey**)&mKey);
    }
    else if (IRSAPublicKey::Probe(publicKey) != NULL) {
        AutoPtr<IRSAPublicKey> rsaPublicKey = IRSAPublicKey::Probe(publicKey);
        OpenSSLRSAPublicKey::GetInstance(rsaPublicKey, (IOpenSSLKey**)&mKey);
    }
    else {
        // throw new InvalidKeyException("Need RSA public key");
        return NOERROR;
    }

    // Allocate buffer according to RSA modulus size.
    Int64 context = 0;
    mKey->GetPkeyContext(&context);
    Int32 maxSize = 0;
    NativeCrypto::RSA_size(context, &maxSize);
    mInputBuffer = ArrayOf<Byte>::Alloc(maxSize);
    mInputOffset = 0;
    return NOERROR;
}

ECode OpenSSLSignatureRawRSA::EngineSetParameter(
    /* [in] */ String param,
    /* [in] */ IInterface* value)
{
    return NOERROR;
}

ECode OpenSSLSignatureRawRSA::EngineSign(
    /* [out, callee] */ ArrayOf<Byte>** sign)
{
    VALIDATE_NOT_NULL(sign)
    if (mKey == NULL) {
        // This can't actually happen, but you never know...
        // throw new SignatureException("Need RSA private key");
        *sign = NULL;
        return NOERROR;
    }

    if (mInputIsTooLong) {
        // throw new SignatureException("input length " + mInputOffset + " != "
        //         + mInputBuffer->GetLength() + " (modulus size)");
        *sign = NULL;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > outputBuffer = ArrayOf<Byte>::Alloc(mInputBuffer->GetLength());
    // try {
    Int64 context = 0;
    mKey->GetPkeyContext(&context);
    Int32 res = 0;
    NativeCrypto::RSA_private_encrypt(mInputOffset, mInputBuffer, outputBuffer,
            context, INativeCrypto::RSA_PKCS1_PADDING, &res);
    mInputOffset = 0;
    *sign = outputBuffer;
    return NOERROR;
    // } catch (Exception ex) {
    //     throw new SignatureException(ex);
    // } finally {
    // }
}

ECode OpenSSLSignatureRawRSA::EngineVerify(
    /* [in] */ ArrayOf<Byte>* sigBytes,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    if (mKey == NULL) {
        // This can't actually happen, but you never know...
        // throw new SignatureException("Need RSA public key");
        *result = FALSE;
        return NOERROR;
    }

    if (mInputIsTooLong) {
        *result = FALSE;
        return NOERROR;
    }

    AutoPtr<ArrayOf<Byte> > outputBuffer = ArrayOf<Byte>::Alloc(mInputBuffer->GetLength());
    // try {
        Int32 resultSize = 0;
        // try {
            Int64 context = 0;
            mKey->GetPkeyContext(&context);
            NativeCrypto::RSA_public_decrypt(sigBytes->GetLength(), sigBytes,
                outputBuffer, context, INativeCrypto::RSA_PKCS1_PADDING, &resultSize);
        // } catch (SignatureException e) {
        //     throw e;
        // } catch (Exception e) {
        //     return false;
        // }
        /* Make this constant time by comparing every byte. */
        Boolean matches = (resultSize == mInputOffset);
        for (Int32 i = 0; i < resultSize; i++) {
            if ((*mInputBuffer)[i] != (*outputBuffer)[i]) {
                matches = FALSE;
            }
        }
        mInputOffset = 0;
        *result = matches;
        return NOERROR;
    // } catch (Exception ex) {
    //     throw new SignatureException(ex);
    // } finally {
    // }
}

} // namespace Conscrypt
} // namespace Org
