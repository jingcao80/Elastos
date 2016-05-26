
#include "CNullCipherSpi.h"
#include <cmdef.h>

namespace Org {
namespace Apache {
namespace Harmony {
namespace Crypto {
namespace Internal {

CAR_OBJECT_IMPL(CNullCipherSpi)

ECode CNullCipherSpi::EngineSetMode(
    /* [in] */ const String& mode)
{
    // Do nothing
    return NOERROR;
}

ECode CNullCipherSpi::EngineSetPadding(
    /* [in] */ const String& padding)
{
    // Do nothing
    return NOERROR;
}

ECode CNullCipherSpi::EngineGetBlockSize(
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = 1;
    return NOERROR;
}

ECode CNullCipherSpi::EngineGetOutputSize(
    /* [in] */ Int32 inputLen,
    /* [out] */ Int32* size)
{
    VALIDATE_NOT_NULL(size)
    *size = inputLen;
    return NOERROR;
}

ECode CNullCipherSpi::EngineGetIV(
    /* [out, callee] */ ArrayOf<Byte>** iv)
{
    VALIDATE_NOT_NULL(iv)
    *iv = ArrayOf<Byte>::Alloc(8);
    REFCOUNT_ADD(*iv)
    return NOERROR;
}

ECode CNullCipherSpi::EngineGetParameters(
    /* [out] */ IAlgorithmParameters** param)
{
    VALIDATE_NOT_NULL(param)
    *param = NULL;
    return NOERROR;
}

ECode CNullCipherSpi::EngineInit(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey* key,
    /* [in] */ ISecureRandom* random)
{
    // Do nothing
    return NOERROR;
}

ECode CNullCipherSpi::EngineInitEx(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey* key,
    /* [in] */ IAlgorithmParameterSpec* params,
    /* [in] */ ISecureRandom* random)
{
    // Do nothing
    return NOERROR;
}

ECode CNullCipherSpi::EngineInitEx2(
    /* [in] */ Int32 opmode,
    /* [in] */ IKey* key,
    /* [in] */ IAlgorithmParameters* params,
    /* [in] */ ISecureRandom* random)
{
    // Do nothing
    return NOERROR;
}

ECode CNullCipherSpi::EngineUpdate(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [out, callee] */ ArrayOf<Byte>** op)
{
    VALIDATE_NOT_NULL(op)
    if (NULL == input) {
        return NOERROR;
    }
    AutoPtr<ArrayOf<Byte> > result = ArrayOf<Byte>::Alloc(inputLen);
    result->Copy(0, input, inputOffset, inputLen);
    *op = result;
    REFCOUNT_ADD(*op)
    return NOERROR;
}

ECode CNullCipherSpi::EngineUpdateEx(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [out] */ ArrayOf<Byte>* output,
    /* [in] */ Int32 outputOffset,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    if (NULL == input) {
        *number = 0;
        return NOERROR;
    }
    output->Copy(outputOffset, input, inputOffset, inputLen);
    *number = inputLen;
    return NOERROR;
}

ECode CNullCipherSpi::EngineUpdateEx2(
    /* [in] */ IByteBuffer* input,
    /* [out] */ IByteBuffer * output,
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number)
    *number = 0;
    if (NULL == input) {
        return E_NULL_POINTER_EXCEPTION;
    } else if (NULL == output) {
        return E_NULL_POINTER_EXCEPTION;
    }
    Int32 limit, position;
    input->GetLimit(&limit);
    input->GetPosition(&position);
    Int32 result = limit - position;
    FAIL_RETURN(output->PutByteBuffer(input))
    *number = result;
    return NOERROR;
}

ECode CNullCipherSpi::EngineDoFinal(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [out, callee] */ ArrayOf<Byte>** bytes)
{
    VALIDATE_NOT_NULL(bytes)
    *bytes = NULL;

    if (NULL == input) {
        return NOERROR;
    }
    return EngineUpdate(input, inputOffset, inputLen, bytes);
}

ECode CNullCipherSpi::EngineDoFinalEx(
    /* [in] */ ArrayOf<Byte>* input,
    /* [in] */ Int32 inputOffset,
    /* [in] */ Int32 inputLen,
    /* [out] */ ArrayOf<Byte>* output,
    /* [in] */ Int32 outputOffset,
    /* [out] */ Int32* number)
{
    return EngineUpdateEx(input, inputOffset, inputLen, output,
        outputOffset, number);
}

ECode CNullCipherSpi::EngineDoFinalEx2(
    /* [in] */ IByteBuffer* input,
    /* [out] */ IByteBuffer* output,
    /* [out] */ Int32* number)
{
    return EngineUpdateEx2(input, output, number);
}

ECode CNullCipherSpi::EngineWrap(
    /* [in] */ Elastos::Security::IKey* keyToWrap,
    /* [out, callee] */ ArrayOf<Byte>** wrappedKey)
{
    VALIDATE_NOT_NULL(wrappedKey)
    *wrappedKey = NULL;
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CNullCipherSpi::EngineUnwrap(
    /* [in] */ ArrayOf<Byte>* wrappedKey,
    /* [in] */ const String& wrappedKeyAlgorithm,
    /* [in] */ Int32 wrappedKeyType,
    /* [out] */ IKey** key)
{
    VALIDATE_NOT_NULL(key)
    *key = NULL;
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CNullCipherSpi::EngineGetKeySize(
    /* [in] */ Elastos::Security::IKey * pKey,
    /* [out] */ Int32 * pSize)
{
    VALIDATE_NOT_NULL(pSize)
    *pSize = 0;
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

} // namespace Internal
} // namespace Crypto
} // namespace Harmony
} // namespace Apache
} // namespace Org

