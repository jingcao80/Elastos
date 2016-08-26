
#ifndef __ELASTOS_DROID_SERVER_AM_APPWAITINGFORDEBUGGERDIALOG_H__
#define __ELASTOS_DROID_SERVER_AM_APPWAITINGFORDEBUGGERDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/am/BaseErrorDialog.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/server/am/CActivityManagerService.h"
#include "elastos/droid/os/Handler.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class AppWaitingForDebuggerDialog : public BaseErrorDialog
{
private:
    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("AppWaitingForDebuggerDialog::MyHandler")

        MyHandler(
            /* [in] */ AppWaitingForDebuggerDialog* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
        /* [in] */ IMessage* msg);

    private:
        AppWaitingForDebuggerDialog* mHost;
    };

public:
    CARAPI constructor(
        /* [in] */ CActivityManagerService* service,
        /* [in] */ IContext* context,
        /* [in] */ ProcessRecord* app);

    CARAPI OnStop();

public:
    CActivityManagerService* mService;  // weak-ref
    AutoPtr<ProcessRecord> mProc;

private:
    AutoPtr<ICharSequence> mAppName;
    AutoPtr<IHandler> mHandler;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_APPWAITINGFORDEBUGGERDIALOG_H__
