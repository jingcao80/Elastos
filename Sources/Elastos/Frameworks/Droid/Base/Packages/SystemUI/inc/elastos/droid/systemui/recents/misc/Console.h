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

#ifndef  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_CONSOLE_H__
#define  __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_CONSOLE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Utility::IMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Recents {
namespace Misc {

class Console
{
public:
    /** Logs a key */
    static CARAPI_(void) Log(
        /* [in] */ const String& key);

    /** Logs a conditioned key */
    static CARAPI_(void) Log(
        /* [in] */ Boolean condition,
        /* [in] */ const String& key);

    /** Logs a key in a specific color */
    static CARAPI_(void) Log(
        /* [in] */ Boolean condition,
        /* [in] */ const String& key,
        /* [in] */ IInterface* data);

    /** Logs a key with data in a specific color */
    static CARAPI_(void) Log(
        /* [in] */ Boolean condition,
        /* [in] */ const String& key,
        /* [in] */ IInterface* data,
        /* [in] */ const String& color);

    /** Logs an error */
    static CARAPI_(void) LogError(
        /* [in] */ IContext* condition,
        /* [in] */ const String& msg);

    /** Logs a raw error */
    static CARAPI_(void) LogRawError(
        /* [in] */ const String& msg,
        /* [in] */ ECode e);

    /** Logs a divider bar */
    static CARAPI_(void) LogDivider(
        /* [in] */ Boolean condition);

    /** Starts a time trace */
    static CARAPI_(void) LogStartTracingTime(
        /* [in] */ Boolean condition,
        /* [in] */ const String& key);

    /** Continues a time trace */
    static CARAPI_(void) LogTraceTime(
        /* [in] */ Boolean condition,
        /* [in] */ const String& key,
        /* [in] */ const String& desc);

    /** Logs a stack trace */
    static CARAPI_(void) LogStackTrace();

    /** Logs a stack trace to a certain depth */
    static CARAPI_(void) LogStackTrace(
        /* [in] */ Int32 depth);

    /** Logs a stack trace to a certain depth with a key */
    static CARAPI_(void) LogStackTrace(
        /* [in] */ const String& key,
        /* [in] */ Int32 depth);


    /** Returns the stringified MotionEvent action */
    static CARAPI_(String) MotionEventActionToString(
        /* [in] */ Int32 action);

    static CARAPI_(String) TrimMemoryLevelToString(
        /* [in] */ Int32 level);

public:
    // Timer
    const static AutoPtr<IMap> mTimeLogs;

    // Colors
    const static String AnsiReset;
    const static String AnsiBlack;
    const static String AnsiRed;      // SystemUIHandshake
    const static String AnsiGreen;    // MeasureAndLayout
    const static String AnsiYellow;   // SynchronizeViewsWithModel
    const static String AnsiBlue;     // TouchEvents, Search
    const static String AnsiPurple;   // Draw
    const static String AnsiCyan;     // ClickEvents
    const static String AnsiWhite;

    // Console enabled state
    static Boolean Enabled;
};

} // namespace Misc
} // namespace Recents
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_RECENTS_MISC_CONSOLE_H__