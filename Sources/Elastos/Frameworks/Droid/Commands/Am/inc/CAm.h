
#ifndef __CAM_H__
#define __CAM_H__

#include "_CAm.h"
#include <ext/frameworkext.h>

using Elastos::Droid::App::IIActivityManager;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntentReceiver;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CarClass(CAm)
{
public:
/**
 * Command-line entry point.
 *
 * @param args The command-line arguments
 */
    CARAPI Main(
        /* [in] */ const ArrayOf<String>& args);

    CAm();

    CARAPI_(Int32) ParseUserArg(
        /* [in] */ const String& arg);

    CARAPI RemoveWallOption();

    CARAPI_(AutoPtr<IIActivityManager>) GetAm();

private:
    static CARAPI ShowUsage();

    CARAPI Run(
    /* [in] */ ArrayOf<String>*args);

    CARAPI MakeIntent(
        /* [in] */ Int32 defUser,
        /* [out] */ IIntent** returnedIntent);

    CARAPI_(void) RunStartService();

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

    CARAPI RunStopUser();

    CARAPI RunMonitor();

    CARAPI RunScreenCompat();

    CARAPI RunDisplaySize();

    CARAPI RunDisplayDensity();

    CARAPI RunToUri(
        /* [in] */ Boolean intentScheme);

    CARAPI NextOption(
    /* [out] */ String* nextOpt);

    CARAPI_(String) NextArg();

    CARAPI NextArgRequired(
        /* [out] */ String* nextArgRequired);

private:
    AutoPtr<IIActivityManager> mAm;
    AutoPtr< ArrayOf<String> > mArgs;
    Int32 mNextArg;
    String mCurArgData;
    Int32 mStartFlags;
    Boolean mWaitOption;
    Boolean mStopOption;
    Int32 mRepeat;
    Int32 mUserId;
    String mProfileFile;
};

}
}
}
}

#endif // __CAM_H__
