
#ifndef __ORG_APACHE_HARMONY_CRYPTO_INTERNAL_CNULLCIPHERSPI_H__
#define __ORG_APACHE_HARMONY_CRYPTO_INTERNAL_CNULLCIPHERSPI_H__

#include "_Org_Apache_Harmony_Crypto_Internal_CNullCipherSpi.h"

using Elastos::Security::IAlgorithmParameters;
using Elastos::Security::IKey;
using Elastos::Security::ISecureRandom;
using Elastos::Security::Spec::IAlgorithmParameterSpec;
using Elastos::IO::IByteBuffer;

namespace Org {
namespace Apache {
namespace Harmony {
namespace Crypto {
namespace Internal {

CarClass(CNullCipherSpi)
    , public CipherSpi
{
public:
    CAR_OBJECT_DECL()

    CARAPI EngineSetMode(
        /* [in] */ const String& mode);

    CARAPI EngineSetPadding(
        /* [in] */ const String& adding);

    CARAPI EngineGetBlockSize(
        /* [out] */ Int32* size);

    CARAPI EngineGetOutputSize(
        /* [in] */ Int32 inputLen,
        /* [out] */ Int32* size);

    CARAPI EngineGetIV(
        /* [out, callee] */ ArrayOf<Byte>** iv);

    CARAPI EngineGetParameters(
        /* [out] */ IAlgorithmParameters **param);

    CARAPI EngineInit(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey* key,
        /* [in] */ ISecureRandom* random);

    CARAPI EngineInitEx(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey* key,
        /* [in] */ IAlgorithmParameterSpec* params,
        /* [in] */ ISecureRandom* random);

    CARAPI EngineInitEx2(
        /* [in] */ Int32 opmode,
        /* [in] */ IKey* key,
        /* [in] */ IAlgorithmParameters* params,
        /* [in] */ ISecureRandom* random);

    CARAPI EngineUpdate(
        /* [in] */ ArrayOf<Byte>* input,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [out, callee] */ ArrayOf<Byte>** op);

    CARAPI EngineUpdateEx(
        /* [in] */ ArrayOf<Byte>* ip,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [out] */ ArrayOf<Byte>* op,
        /* [in] */ Int32 outputOffset,
        /* [out] */ Int32* number);

    CARAPI EngineUpdateEx2(
        /* [in] */ IByteBuffer* ip,
        /* [in] */ IByteBuffer* op,
        /* [out] */ Int32* number);

    CARAPI EngineDoFinal(
        /* [in] */ ArrayOf<Byte>* ip,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [out, callee] */ ArrayOf<Byte>** bytes);

    CARAPI EngineDoFinalEx(
        /* [in] */ ArrayOf<Byte>* ip,
        /* [in] */ Int32 inputOffset,
        /* [in] */ Int32 inputLen,
        /* [out] */ ArrayOf<Byte>* op,
        /* [in] */ Int32 outputOffset,
        /* [out] */ Int32* number);

    CARAPI EngineDoFinalEx2(
        /* [in] */ IByteBuffer* ip,
        /* [in] */ IByteBuffer* op,
        /* [out] */ Int32* number);

    CARAPI EngineWrap(
        /* [in] */ IKey* keyToWrap,
        /* [out, callee] */ ArrayOf<Byte>** wrappedKey);

    CARAPI EngineUnwrap(
        /* [in] */ ArrayOf<Byte>* wrappedKey,
        /* [in] */ const String& wrappedKeyAlgorithm,
        /* [in] */ Int32 wrappedKeyType,
        /* [out] */ IKey** key);

    CARAPI EngineGetKeySize(
        /* [in] */ IKey* key,
        /* [out] */ Int32* size);

private:
    // TODO: Add your private member variables here.
};

} // namespace Internal
} // namespace Crypto
} // namespace Harmony
} // namespace Apache
} // namespace Org

#endif // __ORG_APACHE_HARMONY_CRYPTO_INTERNAL_CNULLCIPHERSPI_H__
