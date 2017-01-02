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

#include "elastos/droid/server/CUpdateLockService.h"
#include <elastos/droid/os/Binder.h>
#include <elastos/droid/Manifest.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Manifest;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::Binder;
using Elastos::Droid::Os::IUpdateLock;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::IUserHandleHelper;
using Elastos::Droid::Os::CUserHandleHelper;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::EIID_IIUpdateLock;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Server {

const Boolean CUpdateLockService::DEBUG = FALSE;
const String CUpdateLockService::TAG("UpdateLockService");
const String CUpdateLockService::PERMISSION("android.permission.UPDATE_LOCK");

CUpdateLockService::LockWatcher::LockWatcher(
    /* [in] */ IHandler* h,
    /* [in] */ const String& tag,
    /* [in] */ CUpdateLockService* owner)
    : TokenWatcher(h, tag)
    , mOwner(owner)
{
}

ECode CUpdateLockService::LockWatcher::Acquired()
{
    if (DEBUG) {
        Slogger::D(TAG, String("first acquire; broadcasting convenient=false") );
    }
    return mOwner->SendLockChangedBroadcast(FALSE);
}

ECode CUpdateLockService::LockWatcher::Released()
{
    if (DEBUG) {
        Slogger::D(TAG, String("last release; broadcasting convenient=true") );
    }
    return mOwner->SendLockChangedBroadcast(TRUE);
}

CAR_INTERFACE_IMPL_2(CUpdateLockService, Object, IIUpdateLock, IBinder)

CAR_OBJECT_IMPL(CUpdateLockService)

ECode CUpdateLockService::constructor(
    /* [in] */ IContext* context)
{
    mContext = context;
    AutoPtr<IHandler> handler;
    CHandler::New((IHandler**)&handler);
    mLocks = new LockWatcher((IHandler*)handler, String("UpdateLocks"), this);

    // Consider just-booting to be a reasonable time to allow
    // interruptions for update installation etc.
    return SendLockChangedBroadcast(TRUE);
}

ECode CUpdateLockService::SendLockChangedBroadcast(
    /* [in] */ Boolean state)
{
    // Safe early during boot because this broadcast only goes to registered receivers.
    Int64 oldIdent = Binder::ClearCallingIdentity();
    //try {
    AutoPtr<IIntent> intent;
    CIntent::New(IUpdateLock::UPDATE_LOCK_CHANGED, (IIntent**)&intent);
    intent->PutBooleanExtra(IUpdateLock::NOW_IS_CONVENIENT, state);
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 millis;
    system->GetCurrentTimeMillis(&millis);
    intent->PutExtra(IUpdateLock::TIMESTAMP, millis);
    intent->AddFlags(IIntent::FLAG_RECEIVER_REGISTERED_ONLY_BEFORE_BOOT);

    AutoPtr<IUserHandleHelper> helper;
    CUserHandleHelper::AcquireSingleton((IUserHandleHelper**)&helper);
    AutoPtr<IUserHandle> all;
    helper->GetALL((IUserHandle**)&all);
    mContext->SendStickyBroadcastAsUser(intent, all);
    //} finally {
    Binder::RestoreCallingIdentity(oldIdent);
    //}

    return NOERROR;
}

ECode CUpdateLockService::AcquireUpdateLock(
    /* [in] */ IBinder* token,
    /* [in] */ const String& tag)
{
    if (DEBUG) {
        Slogger::D(TAG, "acquire(0x%x) by %s", token, MakeTag(tag).string());
    }

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(PERMISSION, String("acquireUpdateLock")));
    mLocks->AcquireToken(token, MakeTag(tag));

    return NOERROR;
}

ECode CUpdateLockService::ReleaseUpdateLock(
    /* [in] */ IBinder* token)
{
    if (DEBUG) {
        Slogger::D(TAG, "release(0x%x)", token);
    }

    FAIL_RETURN(mContext->EnforceCallingOrSelfPermission(PERMISSION, String("releaseUpdateLock")));
    mLocks->ReleaseToken(token);

    return NOERROR;
}

String CUpdateLockService::MakeTag(
    /* [in] */ const String& tag)
{
    StringBuilder b("{tag=");
    b += tag;
    b += " uid=";
    b += Binder::GetCallingUid();
    b += " pid=";
    b += Binder::GetCallingPid();
    b += '}';
    return b.ToString();
}

ECode CUpdateLockService::Dump(
    /* [in] */ IFileDescriptor *fd,
    /* [in] */ IPrintWriter *pw,
    /* [in] */ ArrayOf<String> *args)
{
    Int32 result;
    FAIL_RETURN(mContext->CheckCallingOrSelfPermission(Elastos::Droid::Manifest::permission::DUMP, &result));
    if (result != IPackageManager::PERMISSION_GRANTED) {
        StringBuilder b("Permission Denial: can't dump update lock service from from pid=");
        b += Binder::GetCallingPid();
        b += ", uid=";
        b += Binder::GetCallingUid();
        pw->Println(b.ToString());
        return NOERROR;
    }

    mLocks->Dump(pw);
    return NOERROR;
}

ECode CUpdateLockService::ToString(
    /* [out] */ String* str)
{
    return E_NOT_IMPLEMENTED;
}

}//Server
}//Droid
}//Elastos
