#include "elastos/droid/systemui/recents/misc/DozeTrigger.h"

using Elastos::Droid::Os::CHandler;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

//=================================
// DozeTrigger::DozeRunnable
//=================================

DozeTrigger::DozeRunnable::DozeRunnable(
    /* [in] */ DozeTrigger* host)
    : mHost(host)
{}

ECode DozeTrigger::DozeRunnable::Run()
{
    mHost->mSleepRunnable->Run();
    mHost->mIsDozing = FALSE;
    mHost->mHasTriggered = TRUE;
    return NOERROR;
}

//=================================
// DozeTrigger
//=================================

DozeTrigger::DozeTrigger(
    /* [in] */ Int32 dozeDurationSeconds,
    /* [in] */ IRunnable* sleepRunnable)
    : mDozeDurationSeconds(dozeDurationSeconds)
    , mSleepRunnable(sleepRunnable)
{
    CHandler::New((IHandler**)&mHandler);
    mDozeRunnable = new DozeRunnable(this);
}

void DozeTrigger::StartDozing()
{
    ForcePoke();
    mHasTriggered = FALSE;
}

void DozeTrigger::StopDozing()
{
    mHandler->RemoveCallbacks(mDozeRunnable);
    mIsDozing = FALSE;
}

void DozeTrigger::Poke()
{
    if (mIsDozing) {
        ForcePoke();
    }
}

void DozeTrigger::ForcePoke()
{
    mHandler->RemoveCallbacks(mDozeRunnable);
    Boolean result;
    mHandler->PostDelayed(mDozeRunnable, mDozeDurationSeconds * 1000, &result);
    mIsDozing = TRUE;
}

Boolean DozeTrigger::IsDozing()
{
    return mIsDozing;
}

Boolean DozeTrigger::HasTriggered()
{
    return mHasTriggered;
}

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos