//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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