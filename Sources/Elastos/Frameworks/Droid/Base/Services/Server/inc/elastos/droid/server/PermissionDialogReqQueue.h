#ifndef __ELASTOS_DROID_SERVER_PERMISSIONDIALOGREQQUEUE_H__
#define __ELASTOS_DROID_SERVER_PERMISSIONDIALOGREQQUEUE_H__

#include <_Elastos.Droid.Server.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>

using Elastos::Utility::Etl::List;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Server {

class PermissionDialogReqQueue
    : public Object
{
public:
    class PermissionDialogReq
        : public Object
    {
    public:
        TO_STRING_IMPL("PermissionDialogReqQueue::PermissionDialogReq")

        PermissionDialogReq();

        CARAPI_(void) Set(
            /* [in] */ Int32 res);

        CARAPI_(Int32) Get();

    private:
        Boolean mHasResult;
        Int32 mResult;
    };

public:
    TO_STRING_IMPL("PermissionDialogReqQueue")

    PermissionDialogReqQueue();

    CARAPI_(AutoPtr<IPermissionDialog>) GetDialog();

    CARAPI_(void) NotifyAll(
        /* [in] */ Int32 mode);

    CARAPI_(void) Register(
        /* [in] */ PermissionDialogReq* res);

    CARAPI_(void) SetDialog(
        /* [in] */ IPermissionDialog* dialog);

private:
    AutoPtr<IPermissionDialog> mDialog;
    List<AutoPtr<PermissionDialogReq> > mResultList;
};

} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVER_PERMISSIONDIALOGREQQUEUE_H__