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

#include "elastos/droid/server/PermissionDialogReqQueue.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;

namespace Elastos {
namespace Droid {
namespace Server {
//-----------------------------------------------------------------------------------------
//                  PermissionDialogReqQueue::PermissionDialogReq
//-----------------------------------------------------------------------------------------
PermissionDialogReqQueue::PermissionDialogReq::PermissionDialogReq()
    : mHasResult(FALSE)
    , mResult(0)
{}

void PermissionDialogReqQueue::PermissionDialogReq::Set(
    /* [in] */ Int32 res)
{
    AutoLock syncLock(this);
    mHasResult = TRUE;
    mResult = res;
    NotifyAll();
}

Int32 PermissionDialogReqQueue::PermissionDialogReq::Get()
{
    AutoLock syncLock(this);
    while (!mHasResult) {
        // try {
        Wait();
        // } catch (InterruptedException e) {
        // }
    }

    return mResult;
}

//-----------------------------------------------------------------------------------------
//                  PermissionDialogReqQueue
//-----------------------------------------------------------------------------------------
PermissionDialogReqQueue::PermissionDialogReqQueue()
{
}

AutoPtr<IPermissionDialog> PermissionDialogReqQueue::GetDialog()
{
    return mDialog;
}

void PermissionDialogReqQueue::SetDialog(
    /* [in] */ IPermissionDialog* dialog)
{
    mDialog = dialog;
}

void PermissionDialogReqQueue::Register(
    /* [in] */ PermissionDialogReq* res)
{
    AutoLock syncLock(this);
    mResultList.PushBack(res);
}

void PermissionDialogReqQueue::NotifyAll(
    /* [in] */ Int32 mode)
{
    AutoLock syncLock(this);
    while (!mResultList.IsEmpty()) {
        List<AutoPtr<PermissionDialogReq> >::Iterator it = mResultList.Begin();
        AutoPtr<PermissionDialogReq> res = *it;
        res->Set(mode);
        mResultList.Erase(it);
    }
}

} // Server
} // Droid
} // Elastos