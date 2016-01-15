
#ifndef __MYACTIVITYCONTROLLER_H__
#define __MYACTIVITYCONTROLLER_H__

#include <ext/frameworkext.h>
#include <elastos/core/Object.h>
#include "CAm.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Runnable;
using Elastos::Core::IThread;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

extern "C" const InterfaceID EIID_MyActivityController;

class MyActivityController
{
public:
    MyActivityController();
    ~MyActivityController();

    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid) = 0;

    CARAPI ActivityStarting(
        /* [in] */ IIntent * intent,
        /* [in] */ const String& pkg,
        /* [out] */ Boolean * res);

    CARAPI ActivityResuming(
        /* [in] */ const String& pkg,
        /* [out] */ Boolean * res);

    CARAPI AppCrashed(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& shortMsg,
        /* [in] */ const String& longMsg,
        /* [in] */ Int64 timeMillis,
        /* [in] */ const String& stackTrace,
        /* [out] */ Boolean * res);

    CARAPI AppEarlyNotResponding(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& annotation,
        /* [out] */ Int32 * res);

    CARAPI AppNotResponding(
        /* [in] */ const String& processName,
        /* [in] */ Int32 pid,
        /* [in] */ const String& processStats,
        /* [out] */ Int32 * res);

    CARAPI Init(
        /* [in] */ const String& gdbPort);

    CARAPI KillGdbLocked();

    CARAPI WaitControllerLocked(
        /* [in] */ Int32 pid,
        /* [in] */ Int32 state,
        /* [out] */ Int32* result);

    CARAPI ResumeController(
        /* [in] */ Int32 result);

    CARAPI PrintMessageForState();

    CARAPI Run();

    CARAPI Init(
        /* [in] */ Handle32 am);

    CARAPI Init(
        /* [in] */ Handle32 am,
        /* [in] */ const String& gdbPort);

private:
        class GdbThreadRunnable : public Runnable
        {
        public:
            GdbThreadRunnable(
                /* [in] */ MyActivityController* host);

            virtual CARAPI Run();

        private:
            MyActivityController* mHost;
        };

public:
    String mGdbPort;

    static const Int32 STATE_NORMAL = 0;
    static const Int32 STATE_CRASHED = 1;
    static const Int32 STATE_EARLY_ANR = 2;
    static const Int32 STATE_ANR = 3;

    Int32 mState;

    static const Int32 RESULT_DEFAULT = 0;

    static const Int32 RESULT_CRASH_DIALOG = 0;
    static const Int32 RESULT_CRASH_KILL = 1;

    static const Int32 RESULT_EARLY_ANR_CONTINUE = 0;
    static const Int32 RESULT_EARLY_ANR_KILL = 1;

    static const Int32 RESULT_ANR_DIALOG = 0;
    static const Int32 RESULT_ANR_KILL = 1;
    static const Int32 RESULT_ANR_WAIT = 1;

    Int32 mResult;

    AutoPtr<IThread> mGdbThread;
    Boolean mGotGdbPrint;
    FILE* mFile;
private:
    Object mLock;
    CAm* mHost;
};

}
}
}
}

#endif // __MYACTIVITYCONTROLLER_H__
