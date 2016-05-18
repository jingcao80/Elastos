
#ifndef __ELASTOS_DROID_COMMANDS_AM_AM_H__
#define __ELASTOS_DROID_COMMANDS_AM_AM_H__

#include "elastos/droid/internal/os/BaseCommand.h"
#include "elastos/droid/os/Runnable.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IIActivityController;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::App::IInstrumentation;
using Elastos::Droid::App::IInstrumentationWatcher;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntentReceiver;
using Elastos::Droid::Internal::Os::BaseCommand;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Utility::IArrayMap;
using Elastos::Core::ICharSequence;
using Elastos::Core::IComparator;
using Elastos::Core::IThread;
using Elastos::Core::Object;
using Elastos::IO::IPrintStream;
using Elastos::IO::IReader;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

class Am : public BaseCommand
{
private:
    class System
    {
    public:
        static AutoPtr<IPrintStream> Out;
        static AutoPtr<IPrintStream> Err;
    };

public:
    class MyActivityController
        : public Object
        , public IIActivityController
    {
    private:
        class GdbRunnable : Runnable
        {
        public:
            GdbRunnable(
                /* [in] */ MyActivityController* host,
                /* [in] */ IReader* converter);

            CAR_INTERFACE_DECL()

            CARAPI Run();

        private:
            MyActivityController* mHost;
            AutoPtr<IReader> mConverter;
        };

    public:
        MyActivityController();

        virtual ~MyActivityController() = 0;

        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IIActivityManager* am,
            /* [in] */ const String& gdbPort);

        CARAPI ActivityResuming(
            /* [in] */ const String& pkg,
            /* [out] */ Boolean* result);

        CARAPI ActivityStarting(
            /* [in] */ IIntent* intent,
            /* [in] */ const String& pkg,
            /* [out] */ Boolean* result);

        CARAPI AppCrashed(
            /* [in] */ const String& processName,
            /* [in] */ Int32 pid,
            /* [in] */ const String& shortMsg,
            /* [in] */ const String& longMsg,
            /* [in] */ Int64 timeMillis,
            /* [in] */ const String& stackTrace,
            /* [out] */ Boolean* result);

        CARAPI AppEarlyNotResponding(
            /* [in] */ const String& processName,
            /* [in] */ Int32 pid,
            /* [in] */ const String& annotation,
            /* [out] */ Int32* result);

        CARAPI AppNotResponding(
            /* [in] */ const String& processName,
            /* [in] */ Int32 pid,
            /* [in] */ const String& processStats,
            /* [out] */ Int32* result);

        CARAPI SystemNotResponding(
            /* [in] */ const String& message,
            /* [out] */ Int32* result);

        CARAPI_(void) KillGdbLocked();

        CARAPI_(void) WaitControllerLocked(
            /* [in] */ Int32 pid,
            /* [in] */ Int32 state,
            /* [out] */ Int32* result);

        CARAPI_(void) ResumeController(
            /* [in] */ Int32 result);

        CARAPI_(void) PrintMessageForState();

        CARAPI Run();

    public:
        static const Int32 STATE_NORMAL = 0;
        static const Int32 STATE_CRASHED = 1;
        static const Int32 STATE_EARLY_ANR = 2;
        static const Int32 STATE_ANR = 3;

        static const Int32 RESULT_DEFAULT = 0;

        static const Int32 RESULT_CRASH_DIALOG = 0;
        static const Int32 RESULT_CRASH_KILL = 1;

        static const Int32 RESULT_EARLY_ANR_CONTINUE = 0;
        static const Int32 RESULT_EARLY_ANR_KILL = 1;

        static const Int32 RESULT_ANR_DIALOG = 0;
        static const Int32 RESULT_ANR_KILL = 1;
        static const Int32 RESULT_ANR_WAIT = 1;

        String mGdbPort;
        Int32 mState;
        Int32 mResult;
        AutoPtr<IProcess> mGdbProcess;
        AutoPtr<IThread> mGdbThread;
        Boolean mGotGdbPrint;

    private:
        AutoPtr<IIActivityManager> mAm;
    };

    class IntentReceiver
        : public Object
        , public IIntentReceiver
    {
    public:
        IntentReceiver();

        virtual ~IntentReceiver() = 0;

        CAR_INTERFACE_DECL()

        CARAPI PerformReceive(
            /* [in] */ IIntent* intent,
            /* [in] */ Int32 resultCode,
            /* [in] */ const String& data,
            /* [in] */ IBundle* extras,
            /* [in] */ Boolean ordered,
            /* [in] */ Boolean sticky,
            /* [in] */ Int32 sendingUser);

        CARAPI WaitForFinish();

    private:
        Boolean mFinished;
    };

    class InstrumentationWatcher
        : public Object
        , public IInstrumentationWatcher
    {
    public:
        InstrumentationWatcher();

        virtual ~InstrumentationWatcher() = 0;

        CAR_INTERFACE_DECL()

        CARAPI InstrumentationStatus(
            /* [in] */ IComponentName* name,
            /* [in] */ Int32 resultCode,
            /* [in] */ IBundle* results);

        CARAPI InstrumentationFinished(
            /* [in] */ IComponentName* name,
            /* [in] */ Int32 resultCode,
            /* [in] */ IBundle* results);

        /**
         * Set or reset "raw mode".  In "raw mode", all bundles are dumped.  In "pretty mode",
         * if a bundle includes Instrumentation.REPORT_KEY_STREAMRESULT, just print that.
         * @param rawMode true for raw mode, false for pretty mode.
         */
        CARAPI SetRawOutput(
            /* [in] */ Boolean rawMode);

        CARAPI WaitForFinish(
            /* [out] */ Boolean* result);

    private:
        Boolean mFinished;
        Boolean mRawMode;
    };

    class Comparator
        : public Object
        , public IComparator
    {
    public:
        Comparator(
            /* [in] */ IArrayMap* am);

        CAR_INTERFACE_DECL()

        // @Override
        CARAPI Compare(
            /* [in] */ IInterface* a,
            /* [in] */ IInterface* b,
            /* [out] */ Int32* result);

    private:
        AutoPtr<IArrayMap> mAm;
    };

public:
    Am();

    CARAPI OnRun();

    CARAPI OnShowUsage(
        /* [in] */ IPrintStream* outPrint);

    CARAPI_(Int32) ParseUserArg(
        /* [in] */ const String& arg);

    static CARAPI_(void) RemoveWallOption();

private:
    CARAPI MakeIntent(
        /* [in] */ Int32 defUser,
        /* [out] */ IIntent** intent);

    CARAPI RunStartService();

    CARAPI RunStopService();

    CARAPI RunStart();

    CARAPI RunForceStop();

    CARAPI RunKill();

    CARAPI RunKillAll();

    CARAPI SendBroadcast();

    CARAPI RunInstrument();

    CARAPI RunProfile();

    CARAPI RunDumpHeap();

    CARAPI RunSetDebugApp();

    CARAPI RunClearDebugApp();

    CARAPI RunBugReport();

    CARAPI RunSwitchUser();

    CARAPI RunStartUserInBackground();

    CARAPI RunStopUser();

    CARAPI RunMonitor();

    CARAPI RunHang();

    CARAPI RunRestart();

    CARAPI RunIdleMaintenance();

    CARAPI RunScreenCompat();

    CARAPI RunToUri(
        /* [in] */ Boolean intentScheme);

    CARAPI RunStack();

    CARAPI RunStackStart();

    CARAPI RunStackMoveTask();

    CARAPI RunStackResize();

    CARAPI RunStackList();

    CARAPI RunStackInfo();

    CARAPI RunLockTask();

    CARAPI_(AutoPtr<IList>) GetRecentConfigurations(
        /* [in] */ Int32 days);

    CARAPI RunGetConfig();

private:
    AutoPtr<IIActivityManager> mAm;

    Int32 mStartFlags;
    Boolean mWaitOption;
    Boolean mStopOption;

    Int32 mRepeat;
    Int32 mUserId;
    String mReceiverPermission;

    String mProfileFile;
    Int32 mSamplingInterval;
    Boolean mAutoStop;
};

}
}
}
}

#endif // __ELASTOS_DROID_COMMANDS_AM_AM_H__
