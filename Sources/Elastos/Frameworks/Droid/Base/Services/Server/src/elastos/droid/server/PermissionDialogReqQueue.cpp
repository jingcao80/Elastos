#include "elastos/droid/server/PermissionDialogReqQueue.h"
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
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
    {    AutoLock syncLock(this);
    mHasResult = TRUE;
    mResult = res;
    assert(0 && "TODO");
    // NotifyAll();
    }
}

Int32 PermissionDialogReqQueue::PermissionDialogReq::Get()
{
    {    AutoLock syncLock(this);
        while (!mHasResult) {
            // try {
            assert(0 && "TODO");
                // wait();
            // } catch (InterruptedException e) {
            // }
        }
    }

    return mResult;
}

//-----------------------------------------------------------------------------------------
//                  PermissionDialogReqQueue
//-----------------------------------------------------------------------------------------
PermissionDialogReqQueue::PermissionDialogReqQueue()
{
    mResultList = new List<AutoPtr<PermissionDialogReq> >();
}

AutoPtr<PermissionDialog> PermissionDialogReqQueue::GetDialog()
{
    return mDialog;
}

void PermissionDialogReqQueue::SetDialog(
    /* [in] */ PermissionDialog* dialog)
{
    mDialog = dialog;
}

void PermissionDialogReqQueue::Register(
    /* [in] */ PermissionDialogReq* res)
{
    {    AutoLock syncLock(this);
        mResultList->PushBack(res);
    }
}

void PermissionDialogReqQueue::NotifyAll(
    /* [in] */ Int32 mode)
{
    {    AutoLock syncLock(this);
        while (mResultList->GetSize() != 0) {
            AutoPtr<PermissionDialogReq> res = (*mResultList)[0];
            res->Set(mode);
            mResultList->Remove(0);
        }
    }
}

} // Server
} // Droid
} // Elastos