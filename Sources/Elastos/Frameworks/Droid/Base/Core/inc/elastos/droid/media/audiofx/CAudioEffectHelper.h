
#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CAUDIOEFFECTHELPER_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CAUDIOEFFECTHELPER_H__

#include "_Elastos_Droid_Media_Audiofx_CAudioEffectHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>             // include 单例基类

using Elastos::Utility::IUUID;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CarClass(CAudioEffectHelper)
    , public Singleton
    , public IAudioEffectHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Query all effects available on the platform. Returns an array of
     * {@link android.media.audiofx.AudioEffect.Descriptor} objects
     *
     * @throws IllegalStateException
     */
    CARAPI QueryEffects(
        /* [out, callee] */ ArrayOf<IAudioEffectDescriptor*>** descriptors);

    /**
     * Query all audio pre processing effects applied to the AudioRecord with the supplied
     * audio session ID. Returns an array of {@link android.media.audiofx.AudioEffect.Descriptor}
     * objects.
     * @param audioSession system wide unique audio session identifier.
     * @throws IllegalStateException
     * @hide
     */

    CARAPI QueryPreProcessings(
        /* [in] */ Int32 audioSession,
        /* [out, callee] */ ArrayOf<IAudioEffectDescriptor*>** descriptors);

    /**
     * Checks if the device implements the specified effect type.
     * @param type the requested effect type.
     * @return true if the device implements the specified effect type, false otherwise.
     * @hide
     */
    CARAPI IsEffectTypeAvailable(
        /* [in] */ IUUID* type,
        /* [out] */ Boolean* result);

    CARAPI IsError(
        /* [in] */ Int32 status,
        /* [out] */ Boolean* result);

    /**
     * @hide
     */
    CARAPI ByteArrayToInt32(
        /* [in] */ ArrayOf<Byte>* valueBuf,
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI ByteArrayToInt32(
        /* [in] */ ArrayOf<Byte>* valueBuf,
        /* [in] */ Int32 offset,
        /* [out] */ Int32* result);

    /**
     * @hide
     */
    CARAPI Int32ToByteArray(
        /* [in] */ Int32 value,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * @hide
     */
    CARAPI ByteArrayToInt16(
        /* [in] */ ArrayOf<Byte>* valueBuf,
        /* [out] */ Int16* result);

    /**
     * @hide
     */
    CARAPI ByteArrayToInt16(
        /* [in] */ ArrayOf<Byte>* valueBuf,
        /* [in] */ Int32 offset,
        /* [out] */ Int16* result);

    /**
     * @hide
     */
    CARAPI Int16ToByteArray(
        /* [in] */ Int16 value,
        /* [out, callee] */ ArrayOf<Byte>** result);

    /**
     * @hide
     */
    CARAPI ConcatArrays(
        /* [in] */ ArrayOf<Byte>* array1,
        /* [in] */ ArrayOf<Byte>* array2,
        /* [out, callee] */ ArrayOf<Byte>** result);
};

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOFX_CAUDIOEFFECTHELPER_H__
