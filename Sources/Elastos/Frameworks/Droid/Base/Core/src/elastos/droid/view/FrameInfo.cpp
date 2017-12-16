
#include "elastos/droid/view/FrameInfo.h"
#include <elastos/core/Math.h>

using Elastos::Core::CSystem;

namespace Elastos {
namespace Droid {
namespace View {

const Int32 FrameInfo::FLAGS;
const Int64 FrameInfo::FLAG_WINDOW_LAYOUT_CHANGED;
const Int32 FrameInfo::INTENDED_VSYNC;
const Int32 FrameInfo::VSYNC;
const Int32 FrameInfo::OLDEST_INPUT_EVENT;
const Int32 FrameInfo::NEWEST_INPUT_EVENT;
const Int32 FrameInfo::HANDLE_INPUT_START;
const Int32 FrameInfo::ANIMATION_START;
const Int32 FrameInfo::PERFORM_TRAVERSALS_START;
const Int32 FrameInfo::DRAW_START;

FrameInfo::FrameInfo()
{
    mFrameInfo = ArrayOf<Int64>::Alloc(9);
    CSystem::AcquireSingleton((ISystem**)&mSystem);
}

void FrameInfo::SetVsync(
    /* [in] */ Int64 intendedVsync,
    /* [in] */ Int64 usedVsync)
{
    (*mFrameInfo)[INTENDED_VSYNC] = intendedVsync;
    (*mFrameInfo)[VSYNC] = usedVsync;
    (*mFrameInfo)[OLDEST_INPUT_EVENT] = Elastos::Core::Math::INT64_MAX_VALUE;
    (*mFrameInfo)[NEWEST_INPUT_EVENT] = 0;
    (*mFrameInfo)[FLAGS] = 0;
}

void FrameInfo::UpdateInputEventTime(
    /* [in] */ Int64 inputEventTime,
    /* [in] */ Int64 inputEventOldestTime)
{
    if (inputEventOldestTime < (*mFrameInfo)[OLDEST_INPUT_EVENT]) {
        (*mFrameInfo)[OLDEST_INPUT_EVENT] = inputEventOldestTime;
    }
    if (inputEventTime > (*mFrameInfo)[NEWEST_INPUT_EVENT]) {
        (*mFrameInfo)[NEWEST_INPUT_EVENT] = inputEventTime;
    }
}

void FrameInfo::MarkInputHandlingStart()
{
    Int64 nanoTime;
    mSystem->GetNanoTime(&nanoTime);
    (*mFrameInfo)[HANDLE_INPUT_START] = nanoTime;
}

void FrameInfo::MarkAnimationsStart()
{
    Int64 nanoTime;
    mSystem->GetNanoTime(&nanoTime);
    (*mFrameInfo)[ANIMATION_START] = nanoTime;
}

void FrameInfo::MarkPerformTraversalsStart()
{
    Int64 nanoTime;
    mSystem->GetNanoTime(&nanoTime);
    (*mFrameInfo)[PERFORM_TRAVERSALS_START] = nanoTime;
}

void FrameInfo::MarkDrawStart()
{
    Int64 nanoTime;
    mSystem->GetNanoTime(&nanoTime);
    (*mFrameInfo)[DRAW_START] = nanoTime;
}

void FrameInfo::AddFlags(
    /* [in] */ Int64 flags)
{
    (*mFrameInfo)[FLAGS] |= flags;
}

} // namespace View
} // namespace Droid
} // namespace Elastos
