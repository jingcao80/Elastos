
#ifndef __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLCIPHERRSA_PKCS1_H__
#define __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLCIPHERRSA_PKCS1_H__

#include "_Org_Apache_Harmony_Xnet_Provider_Jsse_COpenSSLCipherRSA_PKCS1.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Xnet {
namespace Provider {
namespace Jsse {

CarClass(COpenSSLCipherRSA_PKCS1)
    , public Object
{
public:
    CAR_OBJECT_DECL()

    CARAPI EngineSetMode(
        /* [in] */ const String& mode);

    CARAPI EngineSetPadding(
        /* [in] */ const String& padding);

    CARAPI EngineGetBlockSize(
        /* [out] */ Int32 * pSize);

    CARAPI EngineGetOutputSize(
        /* [in] */ Int32 inputLen,
        /* [out] */ Int32 * pSize);

    CARAPI EngineGetIV(
        /* [out, callee] */ ArrayOf<Byte> ** ppIv);

    CARAPI EngineGetParameters(
        /* [out] */ Elastos::Security::IAlgorithmParameters ** ppParam);

    CARAPI EngineInit(
        /* [in] */ Int32 opmode,
        /* [in] */ Elastos::Security::IKey * pKey,
        /* [in] */ Elastos::Security::ISecureRandom * pRandom);

    CARAPI EngineInitEx(
        /* [in] */ Int32 opmode,
        /* [in] */ Elastos::Security::IKey * pKey,
        /* [in] */ Elastos::Security::Spec::IAlgorithmParameterSpec * pParams,
        /* [in] */ Elastos::Security::ISecureRandom * pRandom);

    CARAPI EngineInitEx2(
        /* [in] */ Int32 opmode,
        /* [in] */ Elastos::Security::IKey * pKey,
        /* [in] */ Elastos::Security::IAlgorithmParameters * pParams,
        /* [in] */ Elastos::Security::ISecureRandom * pRandom);

    CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte> * pInput,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [out, callee] */ ArrayOf<Byte> ** ppOutput);

    CARAPI EngineUpdateEx(
        /* [in] */ ArrayOf<Byte> * pInput,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [in] */ ArrayOf<Byte> * pOutput,
        /* [in] */ Int32 outputOffset,
        /* [out] */ Int32 * pNumber);

    CARAPI EngineUpdateEx2(
        /* [in] */ Elastos::IO::IByteBuffer * pInput,
        /* [in] */ Elastos::IO::IByteBuffer * pOutput,
        /* [out] */ Int32 * pNumber);

    CARAPI EngineDoFinal(
        /* [in] */ ArrayOf<Byte> * pInput,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [out, callee] */ ArrayOf<Byte> ** ppBytes);

    CARAPI EngineDoFinalEx(
        /* [in] */ ArrayOf<Byte> * pInput,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [in] */ ArrayOf<Byte> * pOutput,
        /* [in] */ Int32 outputOffset,
        /* [out] */ Int32 * pNumber);

    CARAPI EngineDoFinalEx2(
        /* [in] */ Elastos::IO::IByteBuffer * pInput,
        /* [in] */ Elastos::IO::IByteBuffer * pOutput,
        /* [out] */ Int32 * pNumber);

    CARAPI EngineWrap(
        /* [in] */ Elastos::Security::IKey * pKeyToWrap,
        /* [out, callee] */ ArrayOf<Byte> ** ppWrappedKey);

    CARAPI EngineUnwrap(
        /* [in] */ ArrayOf<Byte> * pWrappedKey,
        /* [in] */ const String& wrappedKeyAlgorithm,
        /* [in] */ Int32 wrappedKeyType,
        /* [out] */ Elastos::Security::IKey ** ppKey);

    CARAPI EngineGetKeySize(
        /* [in] */ Elastos::Security::IKey * pKey,
        /* [out] */ Int32 * pSize);

private:
    // TODO: Add your private member variables here.
};

}
}
}
}
}
}

#endif // __ORG_APACHE_HARMONY_XNET_PROVIDER_JSSE_COPENSSLCIPHERRSA_PKCS1_H__
