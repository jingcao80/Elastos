
#ifndef __ELASTOS_DROID_SERVER_AM_STRICTMODEVIOLATIONDIALOG_H__
#define __ELASTOS_DROID_SERVER_AM_STRICTMODEVIOLATIONDIALOG_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/server/am/BaseErrorDialog.h"
#include "elastos/droid/server/am/AppErrorResult.h"
#include "elastos/droid/server/am/ProcessRecord.h"
#include "elastos/droid/os/HandlerBase.h"

using namespace Elastos::Core;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Am {

class StrictModeViolationDialog
    : public BaseErrorDialog
{
private:
    class MyHandler
        : public HandlerBase
    {
    public:
        MyHandler(
            /* [in] */ StrictModeViolationDialog* owner);

        ~MyHandler();

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        StrictModeViolationDialog* mOwner;
    };

public:
    StrictModeViolationDialog(
        /* [in] */ IContext* context,
        /* [in] */ CActivityManagerService* service,
        /* [in] */ AppErrorResult* result,
        /* [in] */ ProcessRecord* app);

private:
    friend class MyHandler;

    static const String TAG;

public:
    // Event 'what' codes
    static const Int32 ACTION_OK;
    static const Int32 ACTION_OK_AND_REPORT;

    // 5-minute timeout, then we automatically dismiss the crash dialog
    static const Int64 DISMISS_TIMEOUT;

private:
    CActivityManagerService* mService;
    AutoPtr<AppErrorResult> mResult;
    AutoPtr<ProcessRecord> mProc;

    AutoPtr<IHandler> mHandler;

    Object mProcLock;
};

} // namespace Am
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_AM_STRICTMODEVIOLATIONDIALOG_H__
