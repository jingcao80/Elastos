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

#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CFACEUNLOCKFACELOCKCALLBACK_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CFACEUNLOCKFACELOCKCALLBACK_H__

#include "_Elastos_Droid_SystemUI_Keyguard_CFaceUnlockFaceLockCallback.h"
#include "elastos/droid/systemui/keyguard/FaceUnlock.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"

using Elastos::Droid::Internal::Policy::IIFaceLockCallback;
using Elastos::Droid::Os::IHandlerCallback;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

CarClass(CFaceUnlockFaceLockCallback)
    , public Object
    , public IIFaceLockCallback
    , public IBinder
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IHandlerCallback* face);

    /**
     * Called when Face Unlock wants to grant access to the user.
     */
    CARAPI Unlock();

    /**
     * Called when Face Unlock wants to go to the backup.
     */
    CARAPI Cancel();

    /**
     * Called when Face Unlock wants to increment the number of failed attempts.
     */
    CARAPI ReportFailedAttempt();

    /**
     * Called when Face Unlock wants to keep the screen alive and active for a specific amount
     * of time.
     */
    CARAPI PokeWakelock(
        /* [in] */ Int32 millis);

    CARAPI ToString(
        /* [out] */ String* str);

private:
    AutoPtr<FaceUnlock> mFace;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_KEYGUARD_CFACEUNLOCKFACELOCKCALLBACK_H__
