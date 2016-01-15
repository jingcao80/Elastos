#include "elastos/droid/media/audiofx/CVisualizerHelper.h"
#include "elastos/droid/media/audiofx/CVisualizer.h"

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

CAR_INTERFACE_IMPL(CVisualizerHelper, Singleton, IVisualizerHelper)

CAR_SINGLETON_IMPL(CVisualizerHelper)

/**
 * Returns the capture size range.
 * @return the mininum capture size is returned in first array element and the maximum in second
 * array element.
 */
ECode CVisualizerHelper::GetCaptureSizeRange(
    /* [out, callee] */ ArrayOf<Int32>** captureSizeRange)
{
    return CVisualizer::GetCaptureSizeRange(captureSizeRange);
}

/**
 * Returns the maximum capture rate for the callback capture method. This is the maximum value
 * for the rate parameter of the
 * {@link #setDataCaptureListener(OnDataCaptureListener, int, boolean, boolean)} method.
 * @return the maximum capture rate expressed in milliHertz
 */
ECode CVisualizerHelper::GetMaxCaptureRate(
    /* [out] */ Int32* captureRate)
{
    return CVisualizer::GetMaxCaptureRate(captureRate);
}

} // namespace Audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos
