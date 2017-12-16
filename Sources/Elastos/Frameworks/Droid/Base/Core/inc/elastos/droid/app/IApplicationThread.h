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

#ifndef __ELASTOS_DROID_APP_IAPPLICATIONTHREAD_H__
#define __ELASTOS_DROID_APP_IAPPLICATIONTHREAD_H__

#include "Elastos.Droid.App.h"
#include <binder/Binder.h>
#include <binder/Parcel.h>

namespace Elastos {
namespace Droid {
namespace App {

class IApplicationThreadStub
    : public android::BBinder
{
public:
    IApplicationThreadStub(
        /* [in] */ IApplicationThread* appThread);

private:
    void onLastStrongRef(
        /* [in] */ const void* id);

    android::status_t onTransact(
        /* [in] */ uint32_t code,
        /* [in] */ const android::Parcel& data,
        /* [out] */ android::Parcel* reply,
        /* [in] */ uint32_t flags = 0);

private:
    static const Boolean DEBUG;
    static const String TAG;

    static const String DESCRIPTOR;
    static const Int32 TRANSACTION_schedulePauseActivity;
    static const Int32 TRANSACTION_scheduleStopActivity;
    static const Int32 TRANSACTION_scheduleWindowVisibility;
    static const Int32 TRANSACTION_scheduleResumeActivity;
    static const Int32 TRANSACTION_scheduleLaunchActivity;
    static const Int32 TRANSACTION_bindApplication;
    static const Int32 TRANSACTION_scheduleConfigurationChanged;
    static const Int32 TRANSACTION_scheduleRelaunchActivity;
    static const Int32 TRANSACTION_scheduleSleeping;
    static const Int32 TRANSACTION_scheduleTrimMemory;
    static const Int32 TRANSACTION_setProcessState;
    static const Int32 TRANSACTION_scheduleEnterAnimationComplete;

    AutoPtr<IApplicationThread> mAppThread;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_IAPPLICATIONTHREAD_H__
