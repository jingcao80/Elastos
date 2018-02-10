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

#ifndef __ELASTOS_DROID_OS_CSTRICTMODEVMPOLICYBUILDER_H__
#define __ELASTOS_DROID_OS_CSTRICTMODEVMPOLICYBUILDER_H__

#include "_Elastos_Droid_Os_CStrictModeVmPolicyBuilder.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/core/Object.h>

using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CStrictModeVmPolicyBuilder)
    , public Object
    , public IStrictModeVmPolicyBuilder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IStrictModeVmPolicy* policy);

    /**
     * Set an upper bound on how many instances of a class can be in memory
     * at once.  Helps to prevent object leaks.
     */
    CARAPI SetClassInstanceLimit(
        /* [in] */ HANDLE klass,
        /* [in] */ Int32 instanceLimit);

    /**
     * Detect leaks of {@link android.app.Activity} subclasses.
     */
    CARAPI DetectActivityLeaks();

    /**
     * Detect everything that's potentially suspect.
     *
     * <p>In the Honeycomb release this includes leaks of
     * SQLite cursors, Activities, and other closable objects
     * but will likely expand in future releases.
     */
    CARAPI DetectAll();
    /**
     * Detect when an
     * {@link android.database.sqlite.SQLiteCursor} or other
     * SQLite object is finalized without having been closed.
     *
     * <p>You always want to explicitly close your SQLite
     * cursors to avoid unnecessary database contention and
     * temporary memory leaks.
     */
    CARAPI DetectLeakedSqlLiteObjects();

    /**
     * Detect when an {@link java.io.Closeable} or other
     * object with a explict termination method is finalized
     * without having been closed.
     *
     * <p>You always want to explicitly close such objects to
     * avoid unnecessary resources leaks.
     */
    CARAPI DetectLeakedClosableObjects();

    /**
     * Detect when a {@code file://} {@link android.net.Uri} is exposed beyond this
     * app. The receiving app may not have access to the sent path.
     * Instead, when sharing files between apps, {@code content://}
     * should be used with permission grants.
     */
    CARAPI DetectFileUriExposure();

    /**
     * Detect when a {@link BroadcastReceiver} or
     * {@link ServiceConnection} is leaked during {@link Context}
     * teardown.
     */
    CARAPI DetectLeakedRegistrationObjects();

    /**
     * Crashes the whole process on violation.  This penalty runs at
     * the end of all enabled penalties so yo you'll still get
     * your logging or other violations before the process dies.
     */
    CARAPI PenaltyDeath();

    /**
     * Log detected violations to the system log.
     */
    CARAPI PenaltyLog();
    /**
     * Enable detected violations log a stacktrace and timing data
     * to the {@link android.os.DropBoxManager DropBox} on policy
     * violation.  Intended mostly for platform integrators doing
     * beta user field data collection.
     */
    CARAPI PenaltyDropBox();


    /**
     * Construct the VmPolicy instance.
     *
     * <p>Note: if no penalties are enabled before calling
     * <code>build</code>, {@link #penaltyLog} is implicitly
     * set.
     */
    CARAPI Build(
        /* [out] */ IStrictModeVmPolicy** policy);

private:
    CARAPI Enable(
        /* [in] */ Int32 bit);

private:
    Int32 mMask;

    HashMap<HANDLE, Int32 > mClassInstanceLimit;  // null until needed
    Boolean mClassInstanceLimitNeedCow;             // need copy-on-write
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CSTRICTMODEVMPOLICYBUILDER_H__
