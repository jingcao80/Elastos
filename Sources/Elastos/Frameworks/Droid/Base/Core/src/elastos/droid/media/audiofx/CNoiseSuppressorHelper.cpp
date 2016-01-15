
#include "elastos/droid/media/audiofx/CNoiseSuppressorHelper.h"
#include "elastos/droid/media/audiofx/CNoiseSuppressor.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_INTERFACE_IMPL(CNoiseSuppressorHelper, Singleton, INoiseSuppressorHelper)

CAR_SINGLETON_IMPL(CNoiseSuppressorHelper)

/**
 * Checks if the device implements acoustic echo cancellation.
 * @return true if the device implements acoustic echo cancellation, false otherwise.
 */
ECode CNoiseSuppressorHelper::IsAvailable(
    /* [out] */ Boolean* result)
{
    return CNoiseSuppressor::IsAvailable(result);
}

/**
 * Creates an AcousticEchoCanceler and attaches it to the AudioRecord on the audio
 * session specified.
 * @param audioSession system wide unique audio session identifier. The AcousticEchoCanceler
 * will be applied to the AudioRecord with the same audio session.
 * @return AcousticEchoCanceler created or null if the device does not implement AEC.
 */
ECode CNoiseSuppressorHelper::Create(
    /* [in] */ Int32 audioSession,
    /* [out] */ INoiseSuppressor** result)
{
    return CNoiseSuppressor::New(audioSession, result);
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
