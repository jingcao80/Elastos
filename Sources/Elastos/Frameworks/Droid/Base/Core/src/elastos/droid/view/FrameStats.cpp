
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/FrameStats.h"

namespace Elastos {
namespace Droid {
namespace View {

CAR_INTERFACE_IMPL(FrameStats, Object, IFrameStats)

/**
 * Gets the refresh period of the display hosting the window(s) for
 * which these statistics apply.
 *
 * @return The refresh period in nanoseconds.
 */
ECode FrameStats::GetRefreshPeriodNano(
    /* [out] */ Int64* nano)
{
    *nano = mRefreshPeriodNano;
    return NOERROR;
}

/**
 * Gets the number of frames for which there is data.
 *
 * @return The number of frames.
 */
ECode FrameStats::GetFrameCount(
    /* [out] */ Int32* count)
{
    *count = mFramesPresentedTimeNano != NULL ?
        mFramesPresentedTimeNano->GetLength() : 0;
    return NOERROR;
}

/**
 * Gets the start time of the interval for which these statistics
 * apply. The start interval is the time when the first frame was
 * presented.
 *
 * @return The start time in nanoseconds or {@link #UNDEFINED_TIME_NANO}
 *         if there is no frame data.
 */
ECode FrameStats::GetStartTimeNano(
    /* [out] */ Int64* nano)
{
    Int32 count;
    GetFrameCount(&count);
    if (count <= 0) {
        *nano = UNDEFINED_TIME_NANO;
    }
    *nano = (*mFramesPresentedTimeNano)[0];
    return NOERROR;
}

/**
 * Gets the end time of the interval for which these statistics
 * apply. The end interval is the time when the last frame was
 * presented.
 *
 * @return The end time in nanoseconds or {@link #UNDEFINED_TIME_NANO}
 *         if there is no frame data.
 */
ECode FrameStats::GetEndTimeNano(
    /* [out] */ Int64* nano)
{
    Int32 count;
    GetFrameCount(&count);
    if (count <= 0) {
        *nano = UNDEFINED_TIME_NANO;
    }
    *nano = (*mFramesPresentedTimeNano)[mFramesPresentedTimeNano->GetLength() - 1];
    return NOERROR;
}

/**
 * Get the time a frame at a given index was presented.
 *
 * @param index The frame index.
 * @return The presented time in nanoseconds or {@link #UNDEFINED_TIME_NANO}
 *         if the frame is not presented yet.
 */
ECode FrameStats::GetFramePresentedTimeNano(
    /* [in] */ Int32 index,
    /* [out] */ Int64* nano)
{
    if (mFramesPresentedTimeNano == NULL) {
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *nano = (*mFramesPresentedTimeNano)[index];
    return NOERROR;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
