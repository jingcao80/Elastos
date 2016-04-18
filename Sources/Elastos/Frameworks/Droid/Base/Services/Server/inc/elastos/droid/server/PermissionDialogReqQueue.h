#ifndef __ELASTOS_DROID_SERVER_PERMISSIONDIALOGREQQUEUE_H__
#define __ELASTOS_DROID_SERVER_PERMISSIONDIALOGREQQUEUE_H__

#include "elastos/droid/server/PermissionDialog.h"
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
        PermissionDialogReq();

        CARAPI_(void) Set(
            /* [in] */ Int32 res);

        CARAPI_(Int32) Get();

    private:
        Boolean mHasResult;
        Int32 mResult;
    };

public:
    PermissionDialogReqQueue();

    CARAPI_(AutoPtr<PermissionDialog>) GetDialog();

    CARAPI_(void) NotifyAll(
        /* [in] */ Int32 mode);

    CARAPI_(void) Register(
        /* [in] */ PermissionDialogReq* res);

    CARAPI_(void) SetDialog(
        /* [in] */ PermissionDialog* dialog);

private:
    AutoPtr<PermissionDialog> mDialog;
    AutoPtr<List<AutoPtr<PermissionDialogReq> > > mResultList;
};

} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVER_PERMISSIONDIALOGREQQUEUE_H__