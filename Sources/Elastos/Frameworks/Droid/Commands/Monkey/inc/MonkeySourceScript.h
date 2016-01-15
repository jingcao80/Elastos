#ifndef __MONKEYSOURCESCRIPT_H__
#define __MONKEYSOURCESCRIPT_H__

#include <ext/frameworkext.h>
#include <elastos/utility/etl/List.h>
#include "Elastos.Droid.Commands.Monkey.h"

using Elastos::IO::IFileInputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::IInputStream;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IReader;
using Elastos::IO::CFileInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::CBufferedReader;
using Elastos::Core::IRandom;
using Elastos::Utility::Etl::List;
using Elastos::IO::IBufferedReader;
using Elastos::Droid::View::ISurface;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::CInputStreamReader;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

class MonkeySourceScript{
public:
    MonkeySourceScript();

    void Init(
        /* [in] */ IRandom *random,
        /* [in] */ const String& filename,
        /* [in] */ Int64 throttle,
        /* [in] */ Boolean randomizeThrottle,
        /* [in] */ Int64 profileWaitTime,
        /* [in] */ Int64 deviceSleepTime
        );

virtual CARAPI_(Boolean) Validate();

virtual CARAPI SetVerbose(
        /* [in] */ Int32 verbose);

virtual CARAPI GetNextEvent(
        /* [out] */ IMonkeyEvent **event);

private:
    /**
     * Resets the globals used to timeshift events.
    */
    void ResetValue();

    /**
     * Reads the header of the script file.
     *
     * @return True if the file header could be parsed, and false otherwise.
     * @throws IOException If there was an error reading the file.
    */
    Boolean ReadHeader();

    Int32 ReadLines();

    void HandleEvent(
        /* [in] */ const String& s,
        /* [in] */ List<String> *args);

    void ProcessLine(
        /* [in] */ const String& line);

    void CloseFile();

    void ReadNextBatch();

    void NeedSleep(
        /* [in] */Int64 time);

    void AdjustKeyEventTime(
        /* [in] */ IMonkeyKeyEvent *e);

    void AdjustMotionEventTime(
        /* [in] */ IMonkeyMotionEvent *e);

public:
    AutoPtr<IFileInputStream> mFStream;

    AutoPtr<IDataInputStream> mInputStream;

    AutoPtr<IBufferedReader> mBufferedReader;

private:
    Int32 mEventCountInScript; // total number of events in the file

    Int32 mVerbose;

    Double mSpeed;

    String mScriptFileName;

    AutoPtr<IMonkeyEventQueue> mQ;

    static const String HEADER_COUNT;

    static const String HEADER_SPEED;

    Int64 mLastRecordedDownTimeKey;

    Int64 mLastRecordedDownTimeMotion;

    Int64 mLastExportDownTimeKey;

    Int64 mLastExportDownTimeMotion;

    Int64 mLastExportEventTime;

    Int64 mLastRecordedEventTime;

    static const Boolean THIS_DEBUG;

    // a parameter that compensates the difference of real elapsed time and
    // time in theory
    static const Int64 SLEEP_COMPENSATE_DIFF;

    // maximum number of events that we read at one time
    static const Int32 MAX_ONE_TIME_READS;

    // event key word in the capture log
    static const String EVENT_KEYWORD_POINTER;

    static const String EVENT_KEYWORD_TRACKBALL;

    static const String EVENT_KEYWORD_ROTATION;

    static const String EVENT_KEYWORD_KEY;

    static const String EVENT_KEYWORD_FLIP;

    static const String EVENT_KEYWORD_KEYPRESS;

    static const String EVENT_KEYWORD_ACTIVITY;

    static const String EVENT_KEYWORD_INSTRUMENTATION;

    static const String EVENT_KEYWORD_WAIT;

    static const String EVENT_KEYWORD_LONGPRESS;

    static const String EVENT_KEYWORD_POWERLOG;

    static const String EVENT_KEYWORD_WRITEPOWERLOG;

    static const String EVENT_KEYWORD_RUNCMD;

    static const String EVENT_KEYWORD_TAP;

    static const String EVENT_KEYWORD_PROFILE_WAIT;

    static const String EVENT_KEYWORD_DEVICE_WAKEUP;

    static const String EVENT_KEYWORD_INPUT_STRING;

    static const String EVENT_KEYWORD_PRESSANDHOLD;

    static const String EVENT_KEYWORD_DRAG;

    static const String EVENT_KEYWORD_PINCH_ZOOM;

    static const String EVENT_KEYWORD_START_FRAMERATE_CAPTURE;

    static const String EVENT_KEYWORD_END_FRAMERATE_CAPTURE;

    static const String EVENT_KEYWORD_START_APP_FRAMERATE_CAPTURE;

    static const String EVENT_KEYWORD_END_APP_FRAMERATE_CAPTURE;

    // a line at the end of the header
    static const String STARTING_DATA_LINE;

    Boolean mFileOpened;

    static Int32 LONGPRESS_WAIT_TIME; // wait time for the long

    static Int64 mProfileWaitTime; //Wait time for each user profile

    static Int64 mDeviceSleepTime; //Device sleep time
};

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

#endif // __MONKEYSOURCESCRIPT_H__
