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
    /* [in] */ IWeakReference* host)
    : mWeakHost(host)
{}

ECode DozeTrigger::DozeRunnable::Run()
{
    AutoPtr<IObject> obj;
    mWeakHost->Resolve(EIID_IObject, (IInterface**)&obj);
    if (obj) {
        DozeTrigger* host = (DozeTrigger*)obj.Get();
        host->mSleepRunnable->Run();
        host->mIsDozing = FALSE;
        host->mHasTriggered = TRUE;
    }
    return NOERROR;
}

//=================================
// DozeTrigger
//=================================

DozeTrigger::DozeTrigger()
    : mIsDozing(FALSE)
    , mHasTriggered(FALSE)
    , mDozeDurationSeconds(0)
{
}

ECode DozeTrigger::constructor(
    /* [in] */ Int32 dozeDurationSeconds,
    /* [in] */ IRunnable* sleepRunnable)
{
    mDozeDurationSeconds = dozeDurationSeconds;
    mSleepRunnable = sleepRunnable;

    CHandler::New((IHandler**)&mHandler);
    AutoPtr<IWeakReference> wr;
    GetWeakReference((IWeakReference**)&wr);
    mDozeRunnable = new DozeRunnable(wr);
    return NOERROR;
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