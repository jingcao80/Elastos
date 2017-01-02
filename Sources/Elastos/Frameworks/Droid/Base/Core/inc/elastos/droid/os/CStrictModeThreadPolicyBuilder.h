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

#ifndef __ELASTOS_DROID_OS_CSTRICTMODETHREADPOLICYBUILDER_H__
#define __ELASTOS_DROID_OS_CSTRICTMODETHREADPOLICYBUILDER_H__

#include "_Elastos_Droid_Os_CStrictModeThreadPolicyBuilder.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>
#include "elastos/droid/os/CStrictMode.h"

using Elastos::Droid::Os::IStrictModeThreadPolicy;

namespace Elastos {
namespace Droid {
namespace Os {

CarClass(CStrictModeThreadPolicyBuilder)
    , public Object
    , public IStrictModeThreadPolicyBuilder
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Create a Builder that detects nothing and has no
     * violations.  (but note that {@link #build} will default
     * to enabling {@link #penaltyLog} if no other penalties
     * are specified)
     */
    CARAPI constructor();

    /**
     * Initialize a Builder from an existing ThreadPolicy.
     */
    CARAPI constructor(
        /* [in] */ IStrictModeThreadPolicy* policy);

    /**
     * Detect everything that's potentially suspect.
     *
     * <p>As of the Gingerbread release this includes network and
     * disk operations but will likely expand in future releases.
     */
    CARAPI DetectAll();

    /**
     * Disable the detection of everything.
     */
    CARAPI PermitAll();

    /**
     * Enable detection of network operations.
     */
    CARAPI DetectNetwork();

    /**
     * Disable detection of network operations.
     */
    CARAPI PermitNetwork();

    /**
     * Enable detection of disk reads.
     */
    CARAPI DetectDiskReads();

    /**
     * Disable detection of disk reads.
     */
    CARAPI PermitDiskReads();

    /**
     * Enable detection of slow calls.
     */
    CARAPI DetectCustomSlowCalls();

    /**
     * Disable detection of slow calls.
     */
    CARAPI PermitCustomSlowCalls();

    /**
     * Enable detection of disk writes.
     */
    CARAPI DetectDiskWrites();

    /**
     * Disable detection of disk writes.
     */
    CARAPI PermitDiskWrites();

    /**
     * Show an annoying dialog to the developer on detected
     * violations, rate-limited to be only a little annoying.
     */
    CARAPI PenaltyDialog();

    /**
     * Crash the whole process on violation.  This penalty runs at
     * the end of all enabled penalties so you'll still get
     * see logging or other violations before the process dies.
     *
     * <p>Unlike {@link #penaltyDeathOnNetwork}, this applies
     * to disk reads, disk writes, and network usage if their
     * corresponding detect flags are set.
     */
    CARAPI PenaltyDeath();

    /**
     * Crash the whole process on any network usage.  Unlike
     * {@link #penaltyDeath}, this penalty runs
     * <em>before</em> anything else.  You must still have
     * called {@link #detectNetwork} to enable this.
     *
     * <p>In the Honeycomb or later SDKs, this is on by default.
     */
    CARAPI PenaltyDeathOnNetwork();

    /**
     * Flash the screen during a violation.
     */
    CARAPI PenaltyFlashScreen();

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
     * Construct the ThreadPolicy instance.
     *
     * <p>Note: if no penalties are enabled before calling
     * <code>build</code>, {@link #penaltyLog} is implicitly
     * set.
     */
    CARAPI Build(
        /* [out] */ IStrictModeThreadPolicy** policy);

private:
    CARAPI Enable(
        /* [in] */ Int32 bit);

    CARAPI Disable(
        /* [in] */ Int32 bit);

private:
        Int32 mMask;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_CSTRICTMODETHREADPOLICYBUILDER_H__
