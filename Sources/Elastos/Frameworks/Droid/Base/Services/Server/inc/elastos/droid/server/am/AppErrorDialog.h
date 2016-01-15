
#ifndef __ELASTOS_DROID_SERVER_AM_APPERRORDIALOG_H__
#define __ELASTOS_DROID_SERVER_AM_APPERRORDIALOG_H__

#include "elastos/droid/os/HandlerBase.h"
#include "elastos/droid/server/am/BaseErrorDialog.h"
#include "elastos/droid/server/am/AppErrorResult.h"
#include "elastos/droid/server/am/ProcessRecord.h"

using namespace Elastos::Core;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class AppErrorDialog : public BaseErrorDialog
{
private:
    class MyHandler
        : public HandlerBase
    {
    public:
        MyHandler(
            /* [in] */ AppErrorDialog* host);

        ~MyHandler();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AppErrorDialog* mHost;
    };

public:
    AppErrorDialog(
        /* [in] */ IContext* context,
        /* [in] */ CActivityManagerService* service,
        /* [in] */ AppErrorResult* result,
        /* [in] */ ProcessRecord* app);

private:
    AutoPtr<CActivityManagerService> mService;

public:
    // Event 'what' codes
    static const Int32 FORCE_QUIT;
    static const Int32 FORCE_QUIT_AND_REPORT;

    // 5-minute timeout, then we automatically dismiss the crash dialog
    static const Int64 DISMISS_TIMEOUT;

private:
    AutoPtr<AppErrorResult> mResult;
    AutoPtr<ProcessRecord> mProc;
    AutoPtr<IHandler> mHandler;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_APPERRORDIALOG_H__
