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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/Binder.h"
#include "elastos/droid/os/CUserHandle.h"
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

using Elastos::Droid::Os::CUserHandle;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(Binder, Object, IBinder)

Binder::Binder()
    : mObject(0)
{}

Binder::~Binder()
{
    // Destroy();
}

ECode Binder::constructor()
{
    return NOERROR;
}

Boolean Binder::IsProxy(
    /* [in] */ IInterface* iface)
{
    return IBinder::Probe(iface) != NULL;
}

ECode Binder::ToString(
    /* [out] */ String* str)
{
    return Object::ToString(str);
}

Int32 Binder::GetCallingPid()
{
    return android::IPCThreadState::self()->getCallingPid();
}

Int32 Binder::GetCallingUid()
{
    return android::IPCThreadState::self()->getCallingUid();
}

Int64 Binder::ClearCallingIdentity()
{
    return android::IPCThreadState::self()->clearCallingIdentity();
}

AutoPtr<IUserHandle> Binder::GetCallingUserHandle()
{
    Int32 userId = CUserHandle::GetUserId(GetCallingUid());
    AutoPtr<IUserHandle> userHandle;
    CUserHandle::New(userId, (IUserHandle**)&userHandle);
    return userHandle;
}

ECode Binder::RestoreCallingIdentity(
    /* [in] */ Int64 token)
{
    // XXX temporary sanity check to debug crashes.
    Int32 uid = (Int32)(token >> 32);
    if (uid > 0 && uid < 999) {
        // In Android currently there are no uids in this range.
//        char buf[128];
//        sprintf(buf, "Restoring bad calling ident: 0x%Lx", token);
//        jniThrowException(env, "java/lang/IllegalStateException", buf);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    android::IPCThreadState::self()->restoreCallingIdentity(token);
    return NOERROR;
}

ECode Binder::SetThreadStrictModePolicy(
    /* [in] */ Int32 policyMask)
{
    android::IPCThreadState::self()->setStrictModePolicy(policyMask);
    return NOERROR;
}

Int32 Binder::GetThreadStrictModePolicy()
{
    return android::IPCThreadState::self()->getStrictModePolicy();
}

ECode Binder::FlushPendingCommands()
{
    android::IPCThreadState::self()->flushCommands();
    return NOERROR;
}

ECode Binder::JoinThreadPool()
{
    android::sp<android::IBinder> b = android::ProcessState::self()->getContextObject(NULL);
    android::IPCThreadState::self()->joinThreadPool();
    return NOERROR;
}

void Binder::Destroy()
{
}

} // namespace Os
} // namespace Droid
} // namespace Elastos

