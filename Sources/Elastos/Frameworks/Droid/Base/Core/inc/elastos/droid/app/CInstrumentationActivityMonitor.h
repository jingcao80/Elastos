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

#ifndef __ELASTOS_DROID_APP_CINSTRUMENTATIONACTIVITYMONITOR_H__
#define __ELASTOS_DROID_APP_CINSTRUMENTATIONACTIVITYMONITOR_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos_Droid_App_CInstrumentationActivityMonitor.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentFilter;
using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IInstrumentationActivityResult;

namespace Elastos {
namespace Droid {
namespace App {

CarClass(CInstrumentationActivityMonitor)
    , public Object
    , public IInstrumentationActivityMonitor
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CInstrumentationActivityMonitor();

    /**
     * Create a new ActivityMonitor that looks for a particular kind of
     * intent to be started.
     *
     * @param which The set of intents this monitor is responsible for.
     * @param result A canned result to return if the monitor is hit; can
     *               be null.
     * @param block Controls whether the monitor should block the activity
     *              start (returning its canned result) or let the call
     *              proceed.
     *
     * @see Instrumentation#addMonitor
     */
    CARAPI constructor(
        /* [in] */ IIntentFilter* which,
        /* [in] */ IInstrumentationActivityResult* result,
        /* [in] */ Boolean block);

    /**
     * Create a new ActivityMonitor that looks for a specific activity
     * class to be started.
     *
     * @param cls The activity class this monitor is responsible for.
     * @param result A canned result to return if the monitor is hit; can
     *               be null.
     * @param block Controls whether the monitor should block the activity
     *              start (returning its canned result) or let the call
     *              proceed.
     *
     * @see Instrumentation#addMonitor
     */
    CARAPI constructor(
        /* [in] */ const String& cls,
        /* [in] */ IInstrumentationActivityResult* result,
        /* [in] */ Boolean block);

    /**
     * Retrieve the filter associated with this ActivityMonitor.
     */
    CARAPI GetFilter(
        /* [out] */ IIntentFilter** filter);

    /**
     * Retrieve the result associated with this ActivityMonitor, or null if
     * none.
     */
    CARAPI GetResult(
        /* [out] */ IInstrumentationActivityResult** result);

    /**
     * Check whether this monitor blocks activity starts (not allowing the
     * actual activity to run) or allows them to execute normally.
     */
    CARAPI IsBlocking(
        /* [out] */ Boolean* result);

    /**
     * Retrieve the number of times the monitor has been hit so far.
     */
    CARAPI GetHits(
        /* [out] */ Int32* time);

    CARAPI SetHits(
        /* [in] */ Int32 time);

    /**
     * Retrieve the most recent activity class that was seen by this
     * monitor.
     */
    CARAPI GetLastActivity(
        /* [out] */ IActivity** activity);

    /**
     * Block until an Activity is created that matches this monitor,
     * returning the resulting activity.
     *
     * @return Activity
     */
    CARAPI WaitForActivity(
        /* [out] */ IActivity** activity);

    /**
     * Block until an Activity is created that matches this monitor,
     * returning the resulting activity or till the timeOut period expires.
     * If the timeOut expires before the activity is started, return null.
     *
     * @param timeOut Time to wait before the activity is created.
     *
     * @return Activity
     */
    CARAPI WaitForActivityWithTimeout(
        /* [in] */ Int64 timeOut,
        /* [out] */ IActivity** activity);

    CARAPI Match(
        /* [in] */ IContext* who,
        /* [in] */ IActivity* activity,
        /* [in] */ IIntent* intent,
        /* [out] */ Boolean* result);

public:
    // This is protected by 'Instrumentation.this.mSync'.
    /*package*/ Int32 mHits;

    // This is protected by 'this'.
    /*package*/ AutoPtr<IActivity> mLastActivity;

private:
    AutoPtr<IIntentFilter> mWhich;
    String mClass;
    AutoPtr<IInstrumentationActivityResult> mResult;
    Boolean mBlock;
};

} // namespace App
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_APP_CINSTRUMENTATIONACTIVITYMONITOR_H__
