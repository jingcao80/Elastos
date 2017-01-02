//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "MonkeySourceScript.h"
#include <elastos/core/StringUtils.h>
#include "elastos/droid/os/SystemClock.h"
#include <unistd.h>

using Elastos::Core::StringUtils;
using Elastos::Core::ICharSequence;
using Elastos::IO::IFileInputStream;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::Text::ISimpleStringSplitter;
using Elastos::Droid::Text::CSimpleStringSplitter;
using Elastos::Droid::Text::ISimpleStringSplitter;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

const String MonkeySourceScript::HEADER_COUNT = String("count=");
const String MonkeySourceScript::HEADER_SPEED = String("speed=");
const Boolean MonkeySourceScript::THIS_DEBUG = FALSE;
const Int64 MonkeySourceScript::SLEEP_COMPENSATE_DIFF = 16;
const Int32 MonkeySourceScript::MAX_ONE_TIME_READS = 100;
const String MonkeySourceScript::EVENT_KEYWORD_POINTER = String("DispatchPointer");
const String MonkeySourceScript::EVENT_KEYWORD_TRACKBALL = String("DispatchTrackball");
const String MonkeySourceScript::EVENT_KEYWORD_ROTATION = String("RotateScreen");
const String MonkeySourceScript::EVENT_KEYWORD_KEY = String("DispatchKey");
const String MonkeySourceScript::EVENT_KEYWORD_FLIP = String("DispatchFlip");
const String MonkeySourceScript::EVENT_KEYWORD_KEYPRESS = String("DispatchPress");
const String MonkeySourceScript::EVENT_KEYWORD_ACTIVITY = String("LaunchActivity");
const String MonkeySourceScript::EVENT_KEYWORD_INSTRUMENTATION = String("LaunchInstrumentation");
const String MonkeySourceScript::EVENT_KEYWORD_WAIT = String("UserWait");
const String MonkeySourceScript::EVENT_KEYWORD_LONGPRESS = String("LongPress");
const String MonkeySourceScript::EVENT_KEYWORD_POWERLOG = String("PowerLog");
const String MonkeySourceScript::EVENT_KEYWORD_WRITEPOWERLOG = String("WriteLog");
const String MonkeySourceScript::EVENT_KEYWORD_RUNCMD = String("RunCmd");
const String MonkeySourceScript::EVENT_KEYWORD_TAP = String("Tap");
const String MonkeySourceScript::EVENT_KEYWORD_PROFILE_WAIT = String("ProfileWait");
const String MonkeySourceScript::EVENT_KEYWORD_DEVICE_WAKEUP = String("DeviceWakeUp");
const String MonkeySourceScript::EVENT_KEYWORD_INPUT_STRING = String("DispatchString");
const String MonkeySourceScript::EVENT_KEYWORD_PRESSANDHOLD = String("PressAndHold");
const String MonkeySourceScript::EVENT_KEYWORD_DRAG = String("Drag");
const String MonkeySourceScript::EVENT_KEYWORD_PINCH_ZOOM = String("PinchZoom");
const String MonkeySourceScript::EVENT_KEYWORD_START_FRAMERATE_CAPTURE = String("StartCaptureFramerate");
const String MonkeySourceScript::EVENT_KEYWORD_END_FRAMERATE_CAPTURE = String("EndCaptureFramerate");
const String MonkeySourceScript::EVENT_KEYWORD_START_APP_FRAMERATE_CAPTURE =
            String("StartCaptureAppFramerate");
const String MonkeySourceScript::EVENT_KEYWORD_END_APP_FRAMERATE_CAPTURE = String("EndCaptureAppFramerate");

    // a line at the end of the header
const String MonkeySourceScript::STARTING_DATA_LINE = String("start data >>");
Int32 MonkeySourceScript::LONGPRESS_WAIT_TIME = 2000; // wait time for the long
Int64 MonkeySourceScript::mProfileWaitTime = 5000; //Wait time for each user profile
Int64 MonkeySourceScript::mDeviceSleepTime = 30000; //Device sleep time

MonkeySourceScript::MonkeySourceScript()
    : mEventCountInScript(0)
    , mVerbose(0)
    , mSpeed(1.0)
    , mScriptFileName()
    , mLastRecordedDownTimeKey(0)
    , mLastRecordedDownTimeMotion(0)
    , mLastExportDownTimeKey(0)
    , mLastExportDownTimeMotion(0)
    , mLastExportEventTime(-1)
    , mLastRecordedEventTime(-1)
    , mFileOpened(FALSE)
{
}

void MonkeySourceScript::Init(
    /* [in] */ IRandom *random,
    /* [in] */ const String& filename,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle,
    /* [in] */ Int64 profileWaitTime,
    /* [in] */ Int64 deviceSleepTime
    )
{
    mScriptFileName = filename;
    ASSERT_SUCCEEDED(CMonkeyEventQueue::New(random, throttle, randomizeThrottle,
        (IMonkeyEventQueue**)&mQ));
    mProfileWaitTime = profileWaitTime;
    mDeviceSleepTime = deviceSleepTime;
}

void MonkeySourceScript::ResetValue()
{
    mLastRecordedDownTimeKey = 0;
    mLastRecordedDownTimeMotion = 0;
    mLastRecordedEventTime = -1;
    mLastExportDownTimeKey = 0;
    mLastExportDownTimeMotion = 0;
    mLastExportEventTime = -1;
}

Boolean MonkeySourceScript::ReadHeader()
{
    mFileOpened = TRUE;

    CFileInputStream::New(mScriptFileName, (IFileInputStream**)&mFStream);
    CDataInputStream::New((IInputStream*)mFStream, (IDataInputStream**)&mInputStream);
    AutoPtr<IInputStreamReader> isr;
    CInputStreamReader::New(mInputStream, (IInputStreamReader**)&isr);
    mBufferedReader = NULL;
    CBufferedReader::New(isr, (IBufferedReader**)&mBufferedReader);

    String line;
    do {
        mBufferedReader->ReadLine(&line);
        if(line.IsNull())
            break;
        line = line.Trim();
        if (line.IndexOf(HEADER_COUNT) >= 0) {
            String value = line.Substring(HEADER_COUNT.GetLength() + 1).Trim();
            ECode e = StringUtils::ParseInt32(value, &mEventCountInScript);
            if (FAILED(e)) {
                PFL_EX("error code: %08x", e);
                return FALSE;
            }
        }
        else if(line.IndexOf(HEADER_SPEED) >= 0)  {
            String value = line.Substring(HEADER_COUNT.GetLength() + 1).Trim();
            ECode e = StringUtils::ParseDouble(value, &mSpeed);
            if (FAILED(e)) {
                PFL_EX("error code: %08x", e);
                return FALSE;
            }
        }
        else if(line.IndexOf(STARTING_DATA_LINE) >= 0) {
            return TRUE;
        }
    }
    while(TRUE);

    return FALSE;
}

Int32 MonkeySourceScript::ReadLines()
{
    String line;
    for (Int32 i = 0; i < MAX_ONE_TIME_READS; ++i) {
        mBufferedReader->ReadLine(&line);
        if (line.IsNull()) {
            return i;
        }

        line = line.Trim();
        ProcessLine(line);
    }
    return MAX_ONE_TIME_READS;
}

void MonkeySourceScript::HandleEvent(
        /* [in] */ const String& s,
        /* [in] */ List<String> *args)
{
    // Handle key event
    if (s.IndexOf(EVENT_KEYWORD_KEY) >= 0 && args->GetSize() == 8) {
        PFL_EX(" old key\n");
        Int64 downTime = StringUtils::ParseInt64((*args)[0]);
        Int64 eventTime = StringUtils::ParseInt64((*args)[1]);
        Int32 action = StringUtils::ParseInt32((*args)[2]);
        Int32 code = StringUtils::ParseInt32((*args)[3]);
        Int32 repeat = StringUtils::ParseInt32((*args)[4]);
        Int32 metaState = StringUtils::ParseInt32((*args)[5]);
        Int32 device = StringUtils::ParseInt32((*args)[6]);
        Int32 scancode = StringUtils::ParseInt32((*args)[7]);

        AutoPtr<IMonkeyKeyEvent> e;
        CMonkeyKeyEvent::New(downTime, eventTime, action, code, repeat,
            metaState, device, scancode, (IMonkeyKeyEvent**)&e);
        PFL_EX(" Key code %d", code);
        mQ->AddLast(IMonkeyEvent::Probe(e));
        PFL_EX("Added key up \n");
        return;
    }

    // Handle trackball or pointer events
    if ((s.IndexOf(EVENT_KEYWORD_POINTER) >= 0
        || s.IndexOf(EVENT_KEYWORD_TRACKBALL) >= 0)
        && args->GetSize() == 12)
    {
        Int64 downTime = StringUtils::ParseInt64((*args)[0]);
        Int64 eventTime = StringUtils::ParseInt64((*args)[1]);
        Int32 action = StringUtils::ParseInt32((*args)[2]);
        Float x = StringUtils::ParseFloat((*args)[3]);
        Float y = StringUtils::ParseFloat((*args)[4]);
        Float pressure = StringUtils::ParseFloat((*args)[5]);
        Float size = StringUtils::ParseFloat((*args)[6]);
        Int32 metaState = StringUtils::ParseInt32((*args)[7]);
        Float xPrecision = StringUtils::ParseFloat((*args)[8]);
        Float yPrecision = StringUtils::ParseFloat((*args)[9]);
        Int32 device = StringUtils::ParseInt32((*args)[10]);
        Int32 edgeFlags = StringUtils::ParseInt32((*args)[11]);

        AutoPtr<IMonkeyMotionEvent> e;
        if (s.IndexOf("Pointer") > 0) {
            CMonkeyTouchEvent::New(action, (IMonkeyMotionEvent**)&e);
        }
        else {
            CMonkeyTrackballEvent::New(action, (IMonkeyMotionEvent**)&e);
        }

        e->SetDownTime(downTime);
        e->SetEventTime(eventTime);
        e->SetMetaState(metaState);
        e->SetPrecision(xPrecision, yPrecision);
        e->SetDeviceId(device);
        e->SetEdgeFlags(edgeFlags);
        e->AddPointer(0, x, y, pressure, size);
        mQ->AddLast(IMonkeyEvent::Probe(e));
        return;
    }

    // Handle screen rotation events
    if ((s.IndexOf(EVENT_KEYWORD_ROTATION) >= 0) && args->GetSize() == 2) {
        Int32 rotationDegree = StringUtils::ParseInt32((*args)[0]);
        Int32 persist = StringUtils::ParseInt32((*args)[1]);
        if ((rotationDegree == ISurface::ROTATION_0) ||
            (rotationDegree == ISurface::ROTATION_90) ||
            (rotationDegree == ISurface::ROTATION_180) ||
            (rotationDegree == ISurface::ROTATION_270))
        {
            AutoPtr<IMonkeyRotationEvent> e;
            CMonkeyRotationEvent::New(rotationDegree,
            persist != 0, (IMonkeyRotationEvent**)&e);
            mQ->AddLast(IMonkeyEvent::Probe(e));
        }
        return;
    }

    // Handle tap event
    if ((s.IndexOf(EVENT_KEYWORD_TAP) >= 0) && args->GetSize() >= 2) {
        Float x = StringUtils::ParseFloat((*args)[0]);
        Float y = StringUtils::ParseFloat((*args)[1]);
        Int64 tapDuration = 0;
        if (args->GetSize() == 3) {
            tapDuration = StringUtils::ParseInt64((*args)[2]);
        }

        // Set the default parameters
        Int64 downTime = SystemClock::GetUptimeMillis();
        AutoPtr<IMonkeyMotionEvent> e1;
        CMonkeyTouchEvent::New(IMotionEvent::ACTION_DOWN, (IMonkeyMotionEvent**)&e1);
        e1->SetDownTime(downTime);
        e1->SetEventTime(downTime);
        e1->AddPointer(0, x, y, 1, 5);
        mQ->AddLast(IMonkeyEvent::Probe(e1));

        if (tapDuration > 0)  {
            AutoPtr<IMonkeyWaitEvent> e;
            CMonkeyWaitEvent::New(tapDuration, (IMonkeyWaitEvent**)&e);
            mQ->AddLast(IMonkeyEvent::Probe(e));
        }

        AutoPtr<IMonkeyMotionEvent> e2;
        CMonkeyTouchEvent::New(IMotionEvent::ACTION_UP, (IMonkeyMotionEvent**)&e2);
        e2->SetDownTime(downTime);
        e2->SetEventTime(downTime);
        e2->AddPointer(0, x, y, 1, 5);
        mQ->AddLast(IMonkeyEvent::Probe(e2));
        return;
    }

    //Handle the press and hold
    if ((s.IndexOf(EVENT_KEYWORD_PRESSANDHOLD) >= 0) && args->GetSize() == 3) {
        Float x = StringUtils::ParseFloat((*args)[0]);
        Float y = StringUtils::ParseFloat((*args)[1]);
        Int64 pressDuration = StringUtils::ParseInt64((*args)[2]);

        // Set the default parameters
        Int64 downTime = SystemClock::GetUptimeMillis();

        AutoPtr<IMonkeyMotionEvent> e1;
        CMonkeyTouchEvent::New(IMotionEvent::ACTION_DOWN, (IMonkeyMotionEvent**)&e1);
        e1->SetDownTime(downTime);
        e1->SetEventTime(downTime);
        e1->AddPointer(0, x, y, 1, 5);
        mQ->AddLast(IMonkeyEvent::Probe(e1));

        AutoPtr<IMonkeyWaitEvent> e2;
        CMonkeyWaitEvent::New(pressDuration, (IMonkeyWaitEvent**)&e2);
        mQ->AddLast(IMonkeyEvent::Probe(e2));

        AutoPtr<IMonkeyTouchEvent> e3;
        CMonkeyTouchEvent::New(IMotionEvent::ACTION_UP, (IMonkeyTouchEvent**)&e3);
        e3->SetDownTime(downTime + pressDuration);
        e3->SetEventTime(downTime + pressDuration);
        e3->AddPointer(0, x, y, 1, 5);
        mQ->AddLast(IMonkeyEvent::Probe(e3));
        return;
    }

    // Handle drag event
    if ((s.IndexOf(EVENT_KEYWORD_DRAG) >= 0) && args->GetSize() == 5) {
        Float xStart = StringUtils::ParseFloat((*args)[0]);
        Float yStart = StringUtils::ParseFloat((*args)[1]);
        Float xEnd = StringUtils::ParseFloat((*args)[2]);
        Float yEnd = StringUtils::ParseFloat((*args)[3]);
        Int32 stepCount = StringUtils::ParseInt32((*args)[4]);

        Float x = xStart;
        Float y = yStart;
        Int64 downTime = SystemClock::GetUptimeMillis();
        Int64 eventTime = SystemClock::GetUptimeMillis();

        if (stepCount > 0) {
            Float xStep = (xEnd - xStart) / stepCount;
            Float yStep = (yEnd - yStart) / stepCount;
            AutoPtr<IMonkeyMotionEvent> e;
            CMonkeyTouchEvent::New(IMotionEvent::ACTION_DOWN, (IMonkeyMotionEvent**)&e);
            e->SetDownTime(downTime);
            e->SetEventTime(eventTime);
            e->AddPointer(0, x, y, 1, 5);
            mQ->AddLast(IMonkeyEvent::Probe(e));

            for (Int32 i = 0; i < stepCount; ++i) {
                x += xStep;
                y += yStep;
                eventTime = SystemClock::GetUptimeMillis();
                e = NULL;
                CMonkeyTouchEvent::New(IMotionEvent::ACTION_MOVE, (IMonkeyMotionEvent**)&e);
                e->SetDownTime(downTime);
                e->SetEventTime(eventTime);
                e->AddPointer(0, x, y, 1, 5);
                mQ->AddLast(IMonkeyEvent::Probe(e));
            }

            eventTime = SystemClock::GetUptimeMillis();
            e = NULL;
            CMonkeyTouchEvent::New(IMotionEvent::ACTION_UP, (IMonkeyMotionEvent**)&e);
            e->SetDownTime(downTime);
            e->SetEventTime(eventTime);
            e->AddPointer(0, x, y, 1, 5);
            mQ->AddLast(IMonkeyEvent::Probe(e));
        }
    }

    // Handle pinch or zoom action
    if ((s.IndexOf(EVENT_KEYWORD_PINCH_ZOOM) >= 0) && args->GetSize() == 9) {
        //Parse the parameters
        Float pt1xStart = StringUtils::ParseFloat((*args)[0]);
        Float pt1yStart = StringUtils::ParseFloat((*args)[1]);
        Float pt1xEnd = StringUtils::ParseFloat((*args)[2]);
        Float pt1yEnd = StringUtils::ParseFloat((*args)[3]);

        Float pt2xStart = StringUtils::ParseFloat((*args)[4]);
        Float pt2yStart = StringUtils::ParseFloat((*args)[5]);
        Float pt2xEnd = StringUtils::ParseFloat((*args)[6]);
        Float pt2yEnd = StringUtils::ParseFloat((*args)[7]);

        Int32 stepCount = StringUtils::ParseInt32((*args)[8]);

        Float x1 = pt1xStart;
        Float y1 = pt1yStart;
        Float x2 = pt2xStart;
        Float y2 = pt2yStart;

        Int64 downTime = SystemClock::GetUptimeMillis();
        Int64 eventTime = SystemClock::GetUptimeMillis();

        if (stepCount > 0) {
            Float pt1xStep = (pt1xEnd - pt1xStart) / stepCount;
            Float pt1yStep = (pt1yEnd - pt1yStart) / stepCount;

            Float pt2xStep = (pt2xEnd - pt2xStart) / stepCount;
            Float pt2yStep = (pt2yEnd - pt2yStart) / stepCount;

            AutoPtr<IMonkeyMotionEvent> e;
            CMonkeyTouchEvent::New(IMotionEvent::ACTION_DOWN, (IMonkeyMotionEvent**)&e);
            e->SetDownTime(downTime);
            e->SetEventTime(eventTime);
            e->AddPointer(0, x1, y1, 1, 5);
            mQ->AddLast(IMonkeyEvent::Probe(e));

            e = NULL;
            CMonkeyTouchEvent::New(IMotionEvent::ACTION_POINTER_DOWN
                | (1 << IMotionEvent::ACTION_POINTER_INDEX_SHIFT), (IMonkeyMotionEvent**)&e);
            e->SetDownTime(downTime);
            e->AddPointer(0, x1, y1);
            e->AddPointer(0, x2, y2);
            e->SetIntermediateNote(TRUE);
            mQ->AddLast(IMonkeyEvent::Probe(e));

            for (Int32 i = 0; i < stepCount; ++i) {
                x1 += pt1xStep;
                y1 += pt1yStep;
                x2 += pt2xStep;
                y2 += pt2yStep;

                eventTime = SystemClock::GetUptimeMillis();
                e = NULL;
                CMonkeyTouchEvent::New(IMotionEvent::ACTION_MOVE, (IMonkeyMotionEvent**)&e);
                e->SetDownTime(downTime);
                e->SetEventTime(eventTime);
                e->AddPointer(0, x1, y1, 1, 5);
                e->AddPointer(0, x2, y2, 1, 5);
                mQ->AddLast(IMonkeyEvent::Probe(e));
            }

            eventTime = SystemClock::GetUptimeMillis();
            e = NULL;
            CMonkeyTouchEvent::New(IMotionEvent::ACTION_POINTER_UP, (IMonkeyMotionEvent**)&e);
            e->SetDownTime(downTime);
            e->SetEventTime(eventTime);
            e->AddPointer(0, x1, y1);
            e->AddPointer(0, x2, y2);
            mQ->AddLast(IMonkeyEvent::Probe(e));
        }
    }

    // Handle flip events
    if (s.IndexOf(EVENT_KEYWORD_FLIP) >= 0 && args->GetSize() == 1) {
        Boolean keyboardOpen = (*args)[0].Equals("true");
        AutoPtr<IMonkeyFlipEvent> e;
        CMonkeyFlipEvent::New(keyboardOpen, (IMonkeyFlipEvent**)&e);
        mQ->AddLast(IMonkeyEvent::Probe(e));
    }

    // Handle launch events
    if (s.IndexOf(EVENT_KEYWORD_ACTIVITY) >= 0 && args->GetSize() >= 2) {
        String pkg_name = (*args)[0];
        String cl_name = (*args)[1];
        Int64 alarmTime = 0;

        AutoPtr<IComponentName> app;
        CComponentName::New(pkg_name, cl_name, (IComponentName**)&app);
        if (args->GetSize() > 2) {
            ECode e = StringUtils::ParseInt64((*args)[2], &alarmTime);
            if (FAILED(e)) {
                //System.err.println("// " + e.toString());
                PFL_EX("// error code: %d", e);
                return;
            }
        }

        if (2 == args->GetSize()) {
            AutoPtr<IMonkeyActivityEvent> e;
            CMonkeyActivityEvent::New(app, (IMonkeyActivityEvent**)&e);
            mQ->AddLast(IMonkeyEvent::Probe(e));
        }
        else {
            AutoPtr<IMonkeyActivityEvent> e;
            CMonkeyActivityEvent::New(app, alarmTime, (IMonkeyActivityEvent**)&e);
            mQ->AddLast(IMonkeyEvent::Probe(e));
        }
        return;
    }

    //Handle the device wake up event
    if (s.IndexOf(EVENT_KEYWORD_DEVICE_WAKEUP) >= 0) {
        String pkg_name("com.google.android.powerutil");
        String cl_name("com.google.android.powerutil.WakeUpScreen");
        Int64 deviceSleepTime = mDeviceSleepTime;

        //Start the wakeUpScreen test activity to turn off the screen.
        AutoPtr<IComponentName> app;
        CComponentName::New(pkg_name, cl_name, (IComponentName**)&app);
        AutoPtr<IMonkeyEvent> e;
        CMonkeyActivityEvent::New(app, deviceSleepTime, (IMonkeyEvent**)&e);
        mQ->AddLast(e);

        //inject the special key for the wakeUpScreen test activity.
        e = NULL;
        CMonkeyKeyEvent::New(IKeyEvent::ACTION_DOWN,
            IKeyEvent::KEYCODE_0, (IMonkeyEvent**)&e);
        mQ->AddLast(e);

        e = NULL;
        CMonkeyKeyEvent::New(IKeyEvent::ACTION_UP,
            IKeyEvent::KEYCODE_0, (IMonkeyEvent**)&e);
        mQ->AddLast(e);

        //Add the wait event after the device sleep event so that the monkey
        //can continue after the device wake up.
        e = NULL;
        CMonkeyWaitEvent::New(deviceSleepTime + 3000, (IMonkeyEvent**)&e);
        mQ->AddLast(e);

        //Insert the menu key to unlock the screen
        e = NULL;
        CMonkeyKeyEvent::New(IKeyEvent::ACTION_DOWN,
            IKeyEvent::KEYCODE_MENU, (IMonkeyEvent**)&e);
        mQ->AddLast(e);

        e = NULL;
        CMonkeyKeyEvent::New(IKeyEvent::ACTION_UP,
            IKeyEvent::KEYCODE_MENU, (IMonkeyEvent**)&e);
        mQ->AddLast(e);

        //Insert the back key to dismiss the test activity
        e = NULL;
        CMonkeyKeyEvent::New(IKeyEvent::ACTION_DOWN,
            IKeyEvent::KEYCODE_BACK, (IMonkeyEvent**)&e);
        mQ->AddLast(e);

        e = NULL;
        CMonkeyKeyEvent::New(IKeyEvent::ACTION_UP,
            IKeyEvent::KEYCODE_BACK, (IMonkeyEvent**)&e);
        mQ->AddLast(e);
        return;
    }

    // Handle launch instrumentation events
    if (s.IndexOf(EVENT_KEYWORD_INSTRUMENTATION) >= 0 && args->GetSize() == 2) {
        String test_name = (*args)[0];
        String runner_name = (*args)[1];
        AutoPtr<IMonkeyInstrumentationEvent> e;
        CMonkeyInstrumentationEvent::New(test_name, runner_name,
            (IMonkeyInstrumentationEvent**)&e);
        mQ->AddLast(e);
        return;
    }

    // Handle wait events
    if (s.IndexOf(EVENT_KEYWORD_WAIT) >= 0 && args->GetSize() == 1) {
        Int64 sleeptime = StringUtils::ParseInt32((*args)[0]);
        AutoPtr<IMonkeyEvent> e;
        CMonkeyWaitEvent::New(sleeptime, (IMonkeyEvent**)&e);
        mQ->AddLast(e);
        return;
    }

    // Handle the profile wait time
    if (s.IndexOf(EVENT_KEYWORD_PROFILE_WAIT) >= 0) {
        AutoPtr<IMonkeyEvent> e;
        CMonkeyWaitEvent::New(mProfileWaitTime, (IMonkeyEvent**)&e);
        mQ->AddLast(e);
        return;
    }

    // Handle keypress events
    if (s.IndexOf(EVENT_KEYWORD_KEYPRESS) >= 0 && args->GetSize() == 1) {
        String key_name = (*args)[0];

        Int32 keyCode;
        AutoPtr<IMonkeySourceRandomHelper> helper;
        ASSERT_SUCCEEDED(CMonkeySourceRandomHelper::AcquireSingleton((IMonkeySourceRandomHelper**)&helper));
        helper->GetKeyCode(key_name, &keyCode);

        if (IKeyEvent::KEYCODE_UNKNOWN == keyCode) {
            return;
        }

        AutoPtr<IMonkeyEvent> e;
        CMonkeyKeyEvent::New(IKeyEvent::ACTION_DOWN,
        keyCode, (IMonkeyEvent**)&e);
        mQ->AddLast(e);

        e = NULL;
        CMonkeyKeyEvent::New(IKeyEvent::ACTION_UP,
        keyCode, (IMonkeyEvent**)&e);
        mQ->AddLast(e);
        return;
    }

    // Handle longpress events
    if (s.IndexOf(EVENT_KEYWORD_LONGPRESS) >= 0) {
        AutoPtr<IMonkeyEvent> e;
        CMonkeyKeyEvent::New(IKeyEvent::ACTION_DOWN,
            IKeyEvent::KEYCODE_DPAD_CENTER, (IMonkeyEvent**)&e);
        mQ->AddLast(e);

        e = NULL;
        CMonkeyWaitEvent::New(LONGPRESS_WAIT_TIME, (IMonkeyEvent**)&e);
        mQ->AddLast(e);

        e = NULL;
        CMonkeyKeyEvent::New(IKeyEvent::ACTION_UP,
            IKeyEvent::KEYCODE_DPAD_CENTER, (IMonkeyEvent**)&e);
        mQ->AddLast(e);
    }

    //The power log event is mainly for the automated power framework
    if (s.IndexOf(EVENT_KEYWORD_POWERLOG) >= 0 && args->IsEmpty() == FALSE) {
        String power_log_type = (*args)[0];
        String test_case_status;

        if (args->GetSize() == 1) {
            AutoPtr<IMonkeyEvent> e;
            CMonkeyPowerEvent::New(power_log_type, (IMonkeyEvent**)&e);
            mQ->AddLast(e);
        }
        else if (args->GetSize() == 2) {
            test_case_status = (*args)[1];
            AutoPtr<IMonkeyEvent> e;
            CMonkeyPowerEvent::New(power_log_type,
                test_case_status, (IMonkeyEvent**)&e);
            mQ->AddLast(e);
        }
    }

    //Write power log to sdcard
    if (s.IndexOf(EVENT_KEYWORD_WRITEPOWERLOG) >= 0) {
        AutoPtr<IMonkeyEvent> e;
        CMonkeyPowerEvent::New((IMonkeyEvent**)&e);
        mQ->AddLast(e);
    }

    //Run the shell command
    if (s.IndexOf(EVENT_KEYWORD_RUNCMD) >= 0 && args->GetSize() == 1) {
        String cmd = (*args)[0];
        AutoPtr<IMonkeyEvent> e;
        CMonkeyCommandEvent::New(cmd, (IMonkeyEvent**)&e);
        mQ->AddLast(e);
    }

    //Input the string through the shell command
    if (s.IndexOf(EVENT_KEYWORD_INPUT_STRING) >= 0 && args->GetSize() == 1) {
        String input = (*args)[0];
        String cmd;
        cmd.AppendFormat("input text  %s", input.string());
        AutoPtr<IMonkeyEvent> e;
        CMonkeyCommandEvent::New(cmd, (IMonkeyEvent**)&e);
        mQ->AddLast(e);
        return;
    }

    if (s.IndexOf(EVENT_KEYWORD_START_FRAMERATE_CAPTURE) >= 0) {
        AutoPtr<IMonkeyEvent> e;
        CMonkeyGetFrameRateEvent::New(String("start"), (IMonkeyEvent**)&e);
        mQ->AddLast(e);
        return;
    }

    if (s.IndexOf(EVENT_KEYWORD_END_FRAMERATE_CAPTURE) >= 0 && args->GetSize() == 1) {
        String input = (*args)[0];
        AutoPtr<IMonkeyEvent> e;
        CMonkeyGetFrameRateEvent::New(String("end"), input, (IMonkeyEvent**)&e);
        mQ->AddLast(e);
        return;
    }

    if (s.IndexOf(EVENT_KEYWORD_START_APP_FRAMERATE_CAPTURE) >= 0 && args->GetSize() == 1) {
        String app = (*args)[0];
        AutoPtr<IMonkeyEvent> e;
        CMonkeyGetAppFrameRateEvent::New(String("start"), app, (IMonkeyEvent**)&e);
        mQ->AddLast(e);
        return;
    }

    if (s.IndexOf(EVENT_KEYWORD_END_APP_FRAMERATE_CAPTURE) >= 0 && args->GetSize() == 2) {
        String app = (*args)[0];
        String label = (*args)[1];
        AutoPtr<IMonkeyEvent> e;
        CMonkeyGetAppFrameRateEvent::New(String("end"), app, label, (IMonkeyEvent**)&e);
        mQ->AddLast(e);
        return;
    }
}

void MonkeySourceScript::ProcessLine(
        /* [in] */ const String& line)
{
    Int32 index1 = line.IndexOf("(");
    Int32 index2 = line.IndexOf(")");
    if (index1 < 0 || index2 < 0) {
        return;
    }

    AutoPtr<List<String> > args = new List<String>();
    AutoPtr<ISimpleStringSplitter> splitter;
    CSimpleStringSplitter::New(',', (ISimpleStringSplitter**)&splitter);
    splitter->SetString(line.Substring(index1 + 1, index2));

    Boolean has = FALSE;
    while (splitter->HasNext(&has), has) {
        AutoPtr<IInterface> cs;
        splitter->Next((IInterface**)&cs);
        String arg;
        ICharSequence::Probe(cs)->ToString(&arg);
        args->PushBack(arg.Trim());
    }
    HandleEvent(line, args);
}

void MonkeySourceScript::CloseFile()
{
    mFileOpened = FALSE;
    mFStream->Close();
    mInputStream->Close();
}

void MonkeySourceScript::ReadNextBatch()
{
    Int32 linesRead = 0;

    if (THIS_DEBUG) {
        PFL_EX("readNextBatch(): reading next batch of events");
    }

    if (!mFileOpened) {
        ResetValue();
        ReadHeader();
    }

    linesRead = ReadLines();

    if (linesRead == 0) {
        CloseFile();
    }
}

void MonkeySourceScript::NeedSleep(
        /* [in] */Int64 time)
{
    if (time < 1) {
        return;
    }
    usleep(time * 1000);
}

Boolean MonkeySourceScript::Validate()
{
    Boolean validHeader = ReadHeader();
    CloseFile();
    if (mVerbose > 0) {
        PFL_EX("Replaying %d events with speed %lf", mEventCountInScript, mSpeed);
    }
    return validHeader;
}

ECode MonkeySourceScript::SetVerbose(
        /* [in] */ Int32 verbose)
{
    mVerbose = verbose;
    return NOERROR;
}

void MonkeySourceScript::AdjustKeyEventTime(
    /* [in] */ IMonkeyKeyEvent *e)
{
    Int64 eventTime;
    e->GetEventTime(&eventTime);
    if (eventTime < 0) {
        return;
    }
    Int64 thisDownTime = 0;
    Int64 thisEventTime = 0;
    Int64 expectedDelay = 0;

    if (mLastRecordedEventTime <= 0) {
        // first time event
        thisDownTime = SystemClock::GetUptimeMillis();
        thisEventTime = thisDownTime;
    }
    else {
        Int64 downTime;
        e->GetDownTime(&downTime);
        if (downTime != mLastRecordedDownTimeKey) {
            e->GetDownTime(&thisDownTime);
        }
        else {
            thisDownTime = mLastExportDownTimeKey;
        }

        expectedDelay = (Int64) ((eventTime - mLastRecordedEventTime) * mSpeed);
        thisEventTime = mLastExportEventTime + expectedDelay;
        // add sleep to simulate everything in recording
        NeedSleep(expectedDelay - SLEEP_COMPENSATE_DIFF);
    }

    e->GetDownTime(&mLastRecordedDownTimeKey);
    e->GetEventTime(&mLastRecordedEventTime);
    e->SetDownTime(thisDownTime);
    e->SetEventTime(thisEventTime);
    mLastExportDownTimeKey = thisDownTime;
    mLastExportEventTime = thisEventTime;
}

void MonkeySourceScript::AdjustMotionEventTime(
    /* [in] */ IMonkeyMotionEvent *e)
{
    Int64 thisEventTime = SystemClock::GetUptimeMillis();
    Int64 thisDownTime;
    e->GetDownTime(&thisDownTime);

    if (thisDownTime == mLastRecordedDownTimeMotion) {
        // this event is the same batch as previous one
        e->SetDownTime(mLastExportDownTimeMotion);
    }
    else {
        // this event is the start of a new batch
        mLastRecordedDownTimeMotion = thisDownTime;
        // update down time to match current time
        e->SetDownTime(thisEventTime);
        mLastExportDownTimeMotion = thisEventTime;
    }
    // always refresh event time
    e->SetEventTime(thisEventTime);
}

ECode MonkeySourceScript::GetNextEvent(
    /* [out] */ IMonkeyEvent **event)
{
    AutoPtr<IMonkeyEvent> ev;

    Boolean isEmpty = FALSE;
    mQ->IsEmpty(&isEmpty);
    if (isEmpty) {
        ReadNextBatch();
    }
    mQ->GetFirst((IMonkeyEvent**)&ev);
    mQ->RemoveFirst();
    Int32 eventType;
    ev->GetEventType(&eventType);
    if (eventType == IMonkeyEvent::EVENT_TYPE_KEY) {
        AdjustKeyEventTime(IMonkeyKeyEvent::Probe(ev));
    }
    else if (eventType == IMonkeyEvent::EVENT_TYPE_TOUCH
        || eventType == IMonkeyEvent::EVENT_TYPE_TRACKBALL)
    {
        AdjustMotionEventTime(IMonkeyMotionEvent::Probe(ev));
    }

    *event = IMonkeyEvent::Probe(ev);
    REFCOUNT_ADD(*event);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
