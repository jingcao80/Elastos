#ifndef __ELASTOS_DROID_SERVER_PERMISSIONDIALOG_H__
#define __ELASTOS_DROID_SERVER_PERMISSIONDIALOG_H__

#include "elastos/droid/server/CAppOpsService.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ICheckBox;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Server {

class PermissionDialog
    : public Object
{
private:
    class MyHandler
        : public Handler
    {
    public:
        MyHandler(
            /* [in] */ PermissionDialog* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        PermissionDialog* mHost;
    };

public:
    PermissionDialog(
        /* [in] */ IContext* context,
        /* [in] */ CAppOpsService* service,
        /* [in] */ Int32 code,
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName);

private:
    CARAPI_(String) GetAppName(
        /* [in] */ const String& packageName);

private:
    static const String TAG;
    AutoPtr<CAppOpsService> mService;
    String mPackageName;
    Int32 mCode;
    AutoPtr<IView> mView;
    AutoPtr<ICheckBox> mChoice;
    Int32 mUid;
    AutoPtr<ArrayOf<ICharSequence*> > mOpLabels;
    AutoPtr<IContext> mContext;
    AutoPtr<IHandler> mHandler;

public:
    // Event 'what' codes
    static const Int32 ACTION_ALLOWED;
    static const Int32 ACTION_IGNORED;
    static const Int32 ACTION_IGNORED_TIMEOUT;

    // 15s timeout, then we automatically dismiss the permission
    // dialog. Otherwise, it may cause watchdog timeout sometimes.
    static const Int64 DISMISS_TIMEOUT;
};

} // Server
} // Droid
} // Elastos

#endif // __ELASTOS_DROID_SERVER_PERMISSIONDIALOG_H__