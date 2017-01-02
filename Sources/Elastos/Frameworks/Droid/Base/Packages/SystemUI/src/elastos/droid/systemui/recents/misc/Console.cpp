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

#include "elastos/droid/systemui/recents/misc/Console.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.IO.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/Thread.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IComponentCallbacks2;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Widget::CToastHelper;
using Elastos::Droid::Widget::IToastHelper;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::CInteger64;
using Elastos::Core::IInteger64;
using Elastos::Core::IStackTraceElement;
using Elastos::Core::ISystem;
using Elastos::Core::IThread;
using Elastos::Core::Math;
using Elastos::Core::StringBuilder;
using Elastos::Core::Thread;
using Elastos::IO::IPrintStream;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

const String Console::AnsiReset("\u001B[0m");
const String Console::AnsiBlack("\u001B[30m");
const String Console::AnsiRed("\u001B[31m");      // SystemUIHandshake
const String Console::AnsiGreen("\u001B[32m");    // MeasureAndLayout
const String Console::AnsiYellow("\u001B[33m");   // SynchronizeViewsWithModel
const String Console::AnsiBlue("\u001B[34m");     // TouchEvents, Search
const String Console::AnsiPurple("\u001B[35m");   // Draw
const String Console::AnsiCyan("\u001B[36m");     // ClickEvents
const String Console::AnsiWhite("\u001B[37m");

// Console enabled state
Boolean Console::Enabled = FALSE;

void Console::Log(
    /* [in] */ const String& key)
{
    AutoPtr<ICharSequence> cs;
    CString::New(String(""), (ICharSequence**)&cs);
    Log(TRUE, key, cs, AnsiReset);
}

void Console::Log(
    /* [in] */ Boolean condition,
    /* [in] */ const String& key)
{
    if (condition) {
        AutoPtr<ICharSequence> cs;
        CString::New(String(""), (ICharSequence**)&cs);
        Log(condition, key, cs, AnsiReset);
    }
}

void Console::Log(
    /* [in] */ Boolean condition,
    /* [in] */ const String& key,
    /* [in] */ IInterface* data)
{
    if (condition) {
        Log(condition, key, data, AnsiReset);
    }
}

void Console::Log(
    /* [in] */ Boolean condition,
    /* [in] */ const String& key,
    /* [in] */ IInterface* data,
    /* [in] */ const String& color)
{
    if (condition) {
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        AutoPtr<IPrintStream> out;
        sys->GetOut((IPrintStream**)&out);
        StringBuilder sb; sb += color; sb += key; sb += AnsiReset; sb += " "; sb += TO_CSTR(data);
        out->Println(sb.ToString());
    }
}

void Console::LogError(
    /* [in] */ IContext* condition,
    /* [in] */ const String& msg)
{
    Logger::E("Recents", msg.string());
    AutoPtr<ICharSequence> cs;
    CString::New(msg, (ICharSequence**)&cs);
    AutoPtr<IToastHelper> toastHelper;
    CToastHelper::AcquireSingleton((IToastHelper**)&toastHelper);
    AutoPtr<IToast> toast;
    toastHelper->MakeText(condition, cs, IToast::LENGTH_SHORT, (IToast**)&toast);
    toast->Show();
}

void Console::LogRawError(
    /* [in] */ const String& msg,
    /* [in] */ ECode e)
{
    Logger::E("Recents", "%s, ec=%08x", msg.string(), e);
}

void Console::LogDivider(
    /* [in] */ Boolean condition)
{
    if (condition) {
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        AutoPtr<IPrintStream> out;
        sys->GetOut((IPrintStream**)&out);
        Int64 value;
        sys->GetCurrentTimeMillis(&value);
        StringBuilder sb("==== ["); sb += value; sb += "] ====";
        out->Println(sb.ToString());
    }
}

void Console::LogStartTracingTime(
    /* [in] */ Boolean condition,
    /* [in] */ const String& key)
{
    if (condition) {
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int64 curTime;
        sys->GetCurrentTimeMillis(&curTime);
        AutoPtr<ICharSequence> keycs;
        CString::New(key, (ICharSequence**)&keycs);
        AutoPtr<IInteger64> ct;
        CInteger64::New(curTime, (IInteger64**)&ct);
        mTimeLogs->Put(keycs, ct);
        AutoPtr<ICharSequence> cs;
        StringBuilder sb("started @ "); sb += curTime;
        CString::New(sb.ToString(), (ICharSequence**)&cs);
        Console::Log(condition, String("[Recents|") + key + String("]"), cs);
    }
}

void Console::LogTraceTime(
    /* [in] */ Boolean condition,
    /* [in] */ const String& key,
    /* [in] */ const String& desc)
{
    if (condition) {
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int64 timeDiff;
        sys->GetCurrentTimeMillis(&timeDiff);
        AutoPtr<ICharSequence> cs;
        CString::New(key, (ICharSequence**)&cs);
        AutoPtr<IInterface> obj;
        mTimeLogs->Get(cs, (IInterface**)&obj);
        Int64 v;
        IInteger64::Probe(obj)->GetValue(&v);
        timeDiff -= v;
        cs = NULL;
        StringBuilder sb("+"); sb += timeDiff; sb += " ms";
        CString::New(sb.ToString(), (ICharSequence**)&cs);
        Console::Log(condition, String("[Recents|") + key + String("|") + desc + String("]"), cs);
    }
}

void Console::LogStackTrace()
{
    LogStackTrace(String(""), 99);
}

void Console::LogStackTrace(
    /* [in] */ Int32 depth)
{
    LogStackTrace(String(""), depth);
}

void Console::LogStackTrace(
    /* [in] */ const String& key,
    /* [in] */ Int32 depth)
{
#if 0
    Int32 offset = 0;
    AutoPtr<IThread> thread = Thread::GetCurrentThread();
    //TODO  thread->GetStackTrace undefined
    AutoPtr<ArrayOf<IStackTraceElement*> > callStack;
    thread->GetStackTrace((ArrayOf<IStackTraceElement*>**)&callStack);
    String tinyStackTrace("");
    // Skip over the known stack trace classes
    for (Int32 i = 0; i < callStack->GetLength(); i++) {
        AutoPtr<IStackTraceElement> el = (*callStack)[i];
        #if 0 //TODO
        String className = el.getClassName();
        if (className.indexOf("dalvik.system.VMStack") == -1 &&
            className.indexOf("java.lang.Thread") == -1 &&
            className.indexOf("recents.Console") == -1) {
            break;
        } else {
            offset++;
        }
        #endif
    }
    // Build the pretty stack trace
    Int32 start = Math::Min(offset + depth, callStack->GetLength());
    Int32 end = offset;
    String indent("");
    for (Int32 i = start - 1; i >= end; i--) {
        AutoPtr<IStackTraceElement> el = (*callStack)[i];
        #if 0 //TODO
        tinyStackTrace += indent + " -> " + el.getClassName() +
                "[" + el.getLineNumber() + "]." + el.getMethodName();
        if (i > end) {
            tinyStackTrace += "\n";
            indent += "  ";
        }
        #endif
    }
    Log(TRUE, key, tinyStackTrace, IConsole::AnsiRed);
#endif
}


String Console::MotionEventActionToString(
    /* [in] */ Int32 action)
{
    switch (action) {
        case IMotionEvent::ACTION_DOWN:
            return String("Down");
        case IMotionEvent::ACTION_UP:
            return String("Up");
        case IMotionEvent::ACTION_MOVE:
            return String("Move");
        case IMotionEvent::ACTION_CANCEL:
            return String("Cancel");
        case IMotionEvent::ACTION_POINTER_DOWN:
            return String("Pointer Down");
        case IMotionEvent::ACTION_POINTER_UP:
            return String("Pointer Up");
        default:
            StringBuilder sb("");
            sb += action;
            return sb.ToString();
    }
}

String Console::TrimMemoryLevelToString(
    /* [in] */ Int32 level)
{
    switch (level) {
        case IComponentCallbacks2::TRIM_MEMORY_UI_HIDDEN:
            return String("UI Hidden");
        case IComponentCallbacks2::TRIM_MEMORY_RUNNING_MODERATE:
            return String("Running Moderate");
        case IComponentCallbacks2::TRIM_MEMORY_BACKGROUND:
            return String("Background");
        case IComponentCallbacks2::TRIM_MEMORY_RUNNING_LOW:
            return String("Running Low");
        case IComponentCallbacks2::TRIM_MEMORY_MODERATE:
            return String("Moderate");
        case IComponentCallbacks2::TRIM_MEMORY_RUNNING_CRITICAL:
            return String("Critical");
        case IComponentCallbacks2::TRIM_MEMORY_COMPLETE:
            return String("Complete");
        default:
            StringBuilder sb("");
            sb += level;
            return sb.ToString();
    }
}

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos