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