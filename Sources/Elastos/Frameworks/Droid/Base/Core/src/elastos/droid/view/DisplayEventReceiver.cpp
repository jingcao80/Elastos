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

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/view/DisplayEventReceiver.h"
#include "elastos/droid/os/Looper.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::Looper;
using Elastos::Droid::Os::MessageQueue;
using Elastos::Droid::Os::NativeMessageQueue;
using Elastos::Core::CCloseGuardHelper;
using Elastos::Core::ICloseGuardHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace View {

const String DisplayEventReceiver::TAG("DisplayEventReceiver");

CAR_INTERFACE_IMPL(DisplayEventReceiver, Object, IDisplayEventReceiver)

DisplayEventReceiver::DisplayEventReceiver(
    /* [in] */ ILooper* looper)
{
    if (looper == NULL) {
        Logger::E(TAG, "looper must not be NULL");
        assert(0);
    }

    AutoPtr<ICloseGuardHelper> helper;
    CCloseGuardHelper::AcquireSingleton((ICloseGuardHelper**)&helper);
    helper->Get((ICloseGuard**)&mCloseGuard);
    mMessageQueue = ((Looper*)looper)->GetQueue();
    NativeInit();

    mCloseGuard->Open(String("DisplayEventReceiver::Dispose"));
}

DisplayEventReceiver::~DisplayEventReceiver()
{
    Dispose(TRUE);
}

void DisplayEventReceiver::NativeInit()
{
    HANDLE nativeQueue;
    mMessageQueue->GetNativeMessageQueue(&nativeQueue);
    AutoPtr<MessageQueue> messageQueue = reinterpret_cast<NativeMessageQueue*>(nativeQueue);
    if (messageQueue == NULL) {
        Logger::E(TAG, "MessageQueue is not initialized.");
        assert(0) ;
    }

    mNativeReceiver = new NativeDisplayEventReceiver(this, messageQueue);
    android::status_t status = mNativeReceiver->initialize();
    if (status) {
        Logger::E(TAG, "Failed to initialize display event receiver.  status=%d", status);
        assert(0) ;
    }
}

ECode DisplayEventReceiver::Dispose()
{
    Dispose(FALSE);
    return NOERROR;
}

void DisplayEventReceiver::Dispose(
    /* [in] */ Boolean finalized)
{
    if (mCloseGuard != NULL) {
        if (finalized) {
            mCloseGuard->WarnIfOpen();
        }
        mCloseGuard->Close();
    }

    if (mNativeReceiver != NULL) {
        mNativeReceiver->dispose();
        mNativeReceiver = NULL;
    }
    mMessageQueue = NULL;
}

ECode DisplayEventReceiver::OnVsync(
    /* [in] */ Int64 timestampNanos,
    /* [in] */ Int32 builtInDisplayId,
    /* [in] */ Int32 frame)
{
    return NOERROR;
}

ECode DisplayEventReceiver::OnHotplug(
    /* [in] */ Int64 timestampNanos,
    /* [in] */ Int32 builtInDisplayId,
    /* [in] */ Boolean connected)
{
    return NOERROR;
}

ECode DisplayEventReceiver::ScheduleVsync()
{
    if (mNativeReceiver == NULL) {
        Logger::W(TAG, "Attempted to schedule a vertical sync pulse but the display event "
            "receiver has already been disposed.");
    }
    else {
        android::status_t status = mNativeReceiver->scheduleVsync();
        if (status) {
            Logger::E(TAG, "Failed to schedule next vertical sync pulse.  status=%d", status);
            return E_RUNTIME_EXCEPTION;
        }
    }
    return NOERROR;
}

void DisplayEventReceiver::DispatchVsync(
    /* [in] */ Int64 timestampNanos,
    /* [in] */ Int32 builtInDisplayId,
    /* [in] */ Int32 frame)
{
    OnVsync(timestampNanos, builtInDisplayId, frame);
}

void DisplayEventReceiver::DispatchHotplug(
    /* [in] */ Int64 timestampNanos,
    /* [in] */ Int32 builtInDisplayId,
    /* [in] */ Boolean connected)
{
    OnHotplug(timestampNanos, builtInDisplayId, connected);
}

} // namespace View
} // namespace Droid
} // namespace Elastos
