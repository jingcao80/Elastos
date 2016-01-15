#ifndef __MONKEYGETAPPFRAMERATEEVENT_H__
#define __MONKEYGETAPPFRAMERATEEVENT_H__

#include "MonkeyEvent.h"
#include "elastos/droid/os/SystemClock.h"
//#include "BufferedReader.h"

using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::IO::IFile;
using Elastos::IO::CFile;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IFileInputStream;
using Elastos::IO::CFileInputStream;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
//using Elastos::IO::BufferedReader;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::Droid::Os::SystemClock;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeyGetAppFrameRateEvent : public MonkeyEvent
{
public:

protected:
    MonkeyGetAppFrameRateEvent();

    MonkeyGetAppFrameRateEvent(
        /* [in] */ const String& status,
        /* [in] */ const String& activityName,
        /* [in] */ const String& testCaseName);

    MonkeyGetAppFrameRateEvent(
        /* [in] */ const String& status,
        /* [in] */ const String& activityName);

    MonkeyGetAppFrameRateEvent(
        /* [in] */ const String& status);

    virtual CARAPI_(Int32) InjectEvent(
        /* [in] */ IIWindowManager *iwm,
        /* [in] */ IIActivityManager *iam,
        /* [in] */ Int32 verbose);

    CARAPI Init(
        /* [in] */ const String& status,
        /* [in] */ const String& activityName,
        /* [in] */ const String& testCaseName);

    CARAPI Init(
        /* [in] */ const String& status,
        /* [in] */ const String& activityName);

    CARAPI Init(
        /* [in] */ const String& status);

private:
    CARAPI_(Float) GetAverageFrameRate(
        /* [in] */ Int32 totalNumberOfFrame,
        /* [in] */ Float duration);

    //Todo, just for log info.
    CARAPI WriteAverageFrameRate();

    //inside initializer
    CARAPI Initializer();

    CARAPI_(String) GetNumberOfFrames(
        /* [in] */ FILE *reader);

private:
    String GET_APP_FRAMERATE_TMPL;
    String mStatus;

    static Int64 sStartTime;
    static Int64 sEndTime;//in milliseconds;
    static Float sDuration;//in seconds;
    static String sActivityName;
    static String sTestCaseName;
    static Int32 sStartFrameNo;
    static Int32 sEndFrameNo;

    static const String TAG;
    static volatile const String LOG_FILE;
    static AutoPtr<IPattern> NO_OF_FRAMES_PATTERN;
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif //__MONKEYGETAPPFRAMERATEEVENT_H__
