
#ifndef __ELASTOS_DROID_SERVER_AM_APPNOTRESPONDINGDIALOG_H__
#define __ELASTOS_DROID_SERVER_AM_APPNOTRESPONDINGDIALOG_H__

#include "BaseErrorDialog.h"
#include "ProcessRecord.h"
#include "elastos/droid/os/HandlerBase.h"

using namespace Elastos::Core;
using Elastos::Droid::Os::HandlerBase;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class AppNotRespondingDialog : public BaseErrorDialog
{
private:
    class MyHandler
        : public HandlerBase
    {
    public:
        MyHandler(
            /* [in] */ AppNotRespondingDialog* host);

        ~MyHandler();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AppNotRespondingDialog* mHost;
    };

public:
    // friend class PositiveButtonOnClickListener;
    // friend class NeutralButtonOnClickListener;

    AppNotRespondingDialog(
        /* [in] */ CActivityManagerService* service,
        /* [in] */ IContext* context,
        /* [in] */ ProcessRecord* app,
        /* [in] */ ActivityRecord* activity,
        /* [in] */ Boolean aboveSystem);

    CARAPI_(void) OnStop();

private:
    static const String TAG;

    AutoPtr<IHandler> mHandler;

public:
    // Event 'what' codes
    static const Int32 FORCE_CLOSE;
    static const Int32 WAIT;
    static const Int32 WAIT_AND_REPORT;

private:
    AutoPtr<CActivityManagerService> mService;
    AutoPtr<ProcessRecord> mProc;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_APPNOTRESPONDINGDIALOG_H__
