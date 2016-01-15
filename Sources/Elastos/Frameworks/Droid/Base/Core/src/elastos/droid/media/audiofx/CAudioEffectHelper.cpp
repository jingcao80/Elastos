#include "elastos/droid/media/audiofx/CAudioEffectHelper.h"
#include "elastos/droid/media/audiofx/AudioEffect.h"

using Elastos::Droid::Media::Audiofx::AudioEffect;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_INTERFACE_IMPL(CAudioEffectHelper, Singleton, IAudioEffectHelper)

CAR_SINGLETON_IMPL(CAudioEffectHelper)

ECode CAudioEffectHelper::QueryEffects(
    /* [out, callee] */ ArrayOf<IAudioEffectDescriptor*>** descriptors)
{
    return AudioEffect::QueryEffects(descriptors);
}

/**
 * Query all audio pre processing effects applied to the AudioRecord with the supplied
 * audio session ID. Returns an array of {@link android.media.audiofx.AudioEffect.Descriptor}
 * objects.
 * @param audioSession system wide unique audio session identifier.
 * @throws IllegalStateException
 * @hide
 */

ECode CAudioEffectHelper::QueryPreProcessings(
    /* [in] */ Int32 audioSession,
    /* [out, callee] */ ArrayOf<IAudioEffectDescriptor*>** descriptors)
{
    return AudioEffect::QueryPreProcessings(audioSession, descriptors);
}

/**
 * Checks if the device implements the specified effect type.
 * @param type the requested effect type.
 * @return true if the device implements the specified effect type, false otherwise.
 * @hide
 */
ECode CAudioEffectHelper::IsEffectTypeAvailable(
    /* [in] */ IUUID* type,
    /* [out] */ Boolean* result)
{
    return AudioEffect::IsEffectTypeAvailable(type, result);
}

ECode CAudioEffectHelper::IsError(
    /* [in] */ Int32 status,
    /* [out] */ Boolean* result)
{
    return AudioEffect::IsError(status, result);
}

ECode CAudioEffectHelper::ByteArrayToInt32(
    /* [in] */ ArrayOf<Byte>* valueBuf,
    /* [out] */ Int32* result)
{
    return AudioEffect::ByteArrayToInt32(valueBuf, result);
}

ECode CAudioEffectHelper::ByteArrayToInt32(
    /* [in] */ ArrayOf<Byte>* valueBuf,
    /* [in] */ Int32 offset,
    /* [out] */ Int32* result)
{
    return AudioEffect::ByteArrayToInt32(valueBuf, offset, result);
}

ECode CAudioEffectHelper::Int32ToByteArray(
    /* [in] */ Int32 value,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    return AudioEffect::Int32ToByteArray(value, result);
}

ECode CAudioEffectHelper::ByteArrayToInt16(
    /* [in] */ ArrayOf<Byte>* valueBuf,
    /* [out] */ Int16* result)
{
    return AudioEffect::ByteArrayToInt16(valueBuf, result);
}

ECode CAudioEffectHelper::ByteArrayToInt16(
    /* [in] */ ArrayOf<Byte>* valueBuf,
    /* [in] */ Int32 offset,
    /* [out] */ Int16* result)
{
    return AudioEffect::ByteArrayToInt16(valueBuf, offset, result);
}

ECode CAudioEffectHelper::Int16ToByteArray(
    /* [in] */ Int16 value,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    return AudioEffect::Int16ToByteArray(value, result);
}

ECode CAudioEffectHelper::ConcatArrays(
    /* [in] */ ArrayOf<Byte>* array1,
    /* [in] */ ArrayOf<Byte>* array2,
    /* [out, callee] */ ArrayOf<Byte>** result)
{
    return AudioEffect::ConcatArrays(array1, array2, result);
}
} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
