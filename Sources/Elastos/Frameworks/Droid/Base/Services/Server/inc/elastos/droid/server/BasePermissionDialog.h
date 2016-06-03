
#ifndef __ELASTOS_DROID_SERVER_BASEPERMISSIONDIALOG_H__
#define __ELASTOS_DROID_SERVER_BASEPERMISSIONDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos.Droid.Server.h"
#include "elastos/droid/app/AlertDialog.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Droid::App::AlertDialog;
using Elastos::Droid::Os::Handler;

namespace Elastos {
namespace Droid {
namespace Server {

class BasePermissionDialog
    : public AlertDialog
    , public IBasePermissionDialog
{
public:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CBasePermissionDialog::MyHandler")

        MyHandler(
            /* [in] */ BasePermissionDialog* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    public:
        BasePermissionDialog* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    BasePermissionDialog();

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

#endif //__ELASTOS_DROID_SERVER_BASEPERMISSIONDIALOG_H__
