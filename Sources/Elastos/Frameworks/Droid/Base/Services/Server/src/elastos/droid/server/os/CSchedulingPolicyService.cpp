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

#include "elastos/droid/server/os/CSchedulingPolicyService.h"
#include <Elastos.Droid.Content.h>
#include <elastos/droid/os/Process.h>
#include <elastos/utility/logging/Slogger.h>
#include <binder/IServiceManager.h>
#include <binder/Parcel.h>

using Elastos::Droid::Os::Process;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::IBinderHelper;
using Elastos::Droid::Os::CBinderHelper;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IISchedulingPolicyService;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Os {

// Keep in sync with frameworks/base/core/java/android/os/ISchedulingPolicyService.aidl
enum {
    REQUEST_PRIORITY_TRANSACTION = IBinder::FIRST_CALL_TRANSACTION,
};

android::status_t CSchedulingPolicyService::NativeSchedulingPolicyService::onTransact(
    /* [in] */ uint32_t code,
    /* [in] */ const android::Parcel& data,
    /* [in] */ android::Parcel* reply,
    /* [in] */ uint32_t flags)
{
    switch (code) {
    case REQUEST_PRIORITY_TRANSACTION:
    {
        data.enforceInterface(android::String16("android.os.ISchedulingPolicyService"));
        Int32 arg0;
        arg0 = data.readInt32();
        Int32 arg1;
        arg1 = data.readInt32();
        Int32 arg2;
        arg2 = data.readInt32();
        Int32 result;
        mHost->RequestPriority(arg0, arg1, arg2, &result);
        reply->writeNoException();
        reply->writeInt32(result);
        break;
    }
    default:
        return BBinder::onTransact(code, data, reply, flags);
    }
    return android::NO_ERROR;
}

const String CSchedulingPolicyService::TAG("CSchedulingPolicyService");

// Minimum and maximum values allowed for requestPriority parameter prio
const Int32 CSchedulingPolicyService::PRIORITY_MIN = 1;
const Int32 CSchedulingPolicyService::PRIORITY_MAX = 3;

CAR_INTERFACE_IMPL_2(CSchedulingPolicyService, Object, IISchedulingPolicyService, IBinder)

CAR_OBJECT_IMPL(CSchedulingPolicyService)

CSchedulingPolicyService::CSchedulingPolicyService()
{}

ECode CSchedulingPolicyService::constructor()
{
    mNative = new NativeSchedulingPolicyService(this);
    android::sp<android::IServiceManager> sm = android::defaultServiceManager();
    int res = sm->addService(android::String16("scheduling_policy"), mNative);
    if (res != 0) {
        Slogger::E(TAG, "add service scheduling_policy failed");
        return E_RUNTIME_EXCEPTION;
    }
    return NOERROR;
}

ECode CSchedulingPolicyService::RequestPriority(
    /* [in] */ Int32 pid,
    /* [in] */  Int32 tid,
    /* [in] */  Int32 prio,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    //Log.i(TAG, "requestPriority(pid=" + pid + ", tid=" + tid + ", prio=" + prio + ")");

    // Verify that caller is mediaserver, priority is in range, and that the
    // callback thread specified by app belongs to the app that called mediaserver.
    // Once we've verified that the caller is mediaserver, we can trust the pid but
    // we can't trust the tid.  No need to explicitly check for pid == 0 || tid == 0,
    // since if not the case then the getThreadGroupLeader() test will also fail.
    AutoPtr<IBinderHelper> binder;
    CBinderHelper::AcquireSingleton((IBinderHelper**)&binder);
    Int32 uid;
    binder->GetCallingUid(&uid);
    if (uid != IProcess::MEDIA_UID || prio < PRIORITY_MIN ||
            prio > PRIORITY_MAX || Process::GetThreadGroupLeader(tid) != pid) {
        *result = IPackageManager::PERMISSION_DENIED;
    }
    // try {
        // make good use of our CAP_SYS_NICE capability
    ECode ec = Process::SetThreadGroup(tid, uid == pid ?
        IProcess::THREAD_GROUP_AUDIO_SYS : IProcess::THREAD_GROUP_AUDIO_APP);
    FAIL_GOTO(ec, _EXIT_)

    // must be in this order or it fails the schedulability constraint
    ec = Process::SetThreadScheduler(tid, IProcess::PROC_SCHED_FIFO, prio);

_EXIT_:
    // } catch (RuntimeException e) {
    if (ec == (ECode)E_RUNTIME_EXCEPTION) {
        *result = IPackageManager::PERMISSION_DENIED;
        return NOERROR;
    }
    // }

    *result = IPackageManager::PERMISSION_GRANTED;
    return NOERROR;
}

ECode CSchedulingPolicyService::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}


} // namespace Os
} // namespace Server
} // namespace Droid
} // namespace Elastos

