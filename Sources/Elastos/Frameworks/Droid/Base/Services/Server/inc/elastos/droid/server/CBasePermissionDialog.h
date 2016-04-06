
#ifndef __ELASTOS_DROID_SERVER_CBASEPERMISSIONDIALOG_H__
#define __ELASTOS_DROID_SERVER_CBASEPERMISSIONDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CBasePermissionDialog.h"
#include "elastos/droid/app/AlertDialog.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Os::Handler;

namespace Elastos {
namespace Droid {
namespace Server {

CarClass(CBasePermissionDialog)
    , public AlertDialog
    , public IBasePermissionDialog
{
public:
    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ CBasePermissionDialog* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        CBasePermissionDialog* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CBasePermissionDialog();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI OnStart();

    CARAPI DispatchKeyEvent(
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

private:
    AutoPtr<IHandler> mHandler;

    Boolean mConsuming;
};

} //namespace Server
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_SERVER_CBASEPERMISSIONDIALOG_H__
