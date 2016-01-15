
#include "COpenSSLCipherRSA_PKCS1.h"

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {


ECode COpenSSLCipherRSA_PKCS1::EngineSetMode(
    /* [in] */ const String& mode)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineSetPadding(
    /* [in] */ const String& padding)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineGetBlockSize(
    /* [out] */ Int32 * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineGetOutputSize(
    /* [in] */ Int32 inputLen,
    /* [out] */ Int32 * pSize)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineGetIV(
    /* [out, callee] */ ArrayOf<Byte> ** ppIv)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineGetParameters(
    /* [out] */ Elastos::Security::IAlgorithmParameters ** ppParam)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineInit(
    /* [in] */ Int32 opmode,
    /* [in] */ Elastos::Security::IKey * pKey,
    /* [in] */ Elastos::Security::ISecureRandom * pRandom)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineInitEx(
    /* [in] */ Int32 opmode,
    /* [in] */ Elastos::Security::IKey * pKey,
    /* [in] */ Elastos::Security::Spec::IAlgorithmParameterSpec * pParams,
    /* [in] */ Elastos::Security::ISecureRandom * pRandom)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineInitEx2(
    /* [in] */ Int32 opmode,
    /* [in] */ Elastos::Security::IKey * pKey,
    /* [in] */ Elastos::Security::IAlgorithmParameters * pParams,
    /* [in] */ Elastos::Security::ISecureRandom * pRandom)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineUpdate(
    /* [in] */ ArrayOf<Byte> * pInput,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [out, callee] */ ArrayOf<Byte> ** ppOutput)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineUpdateEx(
    /* [in] */ ArrayOf<Byte> * pInput,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [in] */ ArrayOf<Byte> * pOutput,
    /* [in] */ Int32 outputOffset,
    /* [out] */ Int32 * pNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineUpdateEx2(
    /* [in] */ Elastos::IO::IByteBuffer * pInput,
    /* [in] */ Elastos::IO::IByteBuffer * pOutput,
    /* [out] */ Int32 * pNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineDoFinal(
    /* [in] */ ArrayOf<Byte> * pInput,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [out, callee] */ ArrayOf<Byte> ** ppBytes)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineDoFinalEx(
    /* [in] */ ArrayOf<Byte> * pInput,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [in] */ ArrayOf<Byte> * pOutput,
    /* [in] */ Int32 outputOffset,
    /* [out] */ Int32 * pNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineDoFinalEx2(
    /* [in] */ Elastos::IO::IByteBuffer * pInput,
    /* [in] */ Elastos::IO::IByteBuffer * pOutput,
    /* [out] */ Int32 * pNumber)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineWrap(
    /* [in] */ Elastos::Security::IKey * pKeyToWrap,
    /* [out, callee] */ ArrayOf<Byte> ** ppWrappedKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineUnwrap(
    /* [in] */ ArrayOf<Byte> * pWrappedKey,
    /* [in] */ const String& wrappedKeyAlgorithm,
    /* [in] */ Int32 wrappedKeyType,
    /* [out] */ Elastos::Security::IKey ** ppKey)
{
    // TODO: Add your code here
    return E_NOT_IMPLEMENTED;
}

ECode COpenSSLCipherRSA_PKCS1::EngineGetKeySize(
    /* [in] */ Elastos::Security::IKey * pKey,
    /* [out] */ Int32 * pSize)
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

