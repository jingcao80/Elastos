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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_RETRYMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_RETRYMANAGER_H__

#include "Elastos.CoreLibrary.Utility.h"
#include "Elastos.Droid.Utility.h"
#include "_Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::IPair;

using Elastos::Core::Object;
using Elastos::Utility::IRandom;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

class RetryManager
    : public Object
    , public IRetryManager
{
private:
    /**
     * Retry record with times in milli-seconds
     */
    class RetryRec
        : public Object
    {
    public:
        RetryRec(
            /* [in] */ Int32 delayTime,
            /* [in] */ Int32 randomizationTime);

    public:
        Int32 mDelayTime;
        Int32 mRandomizationTime;
    };

public:
    CAR_INTERFACE_DECL()

    RetryManager();

    /** Constructor */
    CARAPI constructor();

    //@Override
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Configure for a simple linear sequence of times plus
     * a random value.
     *
     * @param maxRetryCount is the maximum number of retries
     *        before isRetryNeeded returns FALSE.
     * @param retryTime is a time that will be returned by getRetryTime.
     * @param randomizationTime a random value between 0 and
     *        randomizationTime will be added to retryTime. this
     *        parameter may be 0.
     * @return TRUE if successful
     */
    CARAPI Configure(
        /* [in] */ Int32 maxRetryCount,
        /* [in] */ Int32 retryTime,
        /* [in] */ Int32 randomizationTime,
        /* [out] */ Boolean* result);

    /**
     * Configure for using string which allow arbitrary
     * sequences of times. See class comments for the
     * string format.
     *
     * @return TRUE if successful
     */
    CARAPI Configure(
        /* [in] */ const String& _configStr,
        /* [out] */ Boolean* result);

    /**
     * Report whether data reconnection should be retried
     *
     * @return {@code TRUE} if the max retries has not been reached. {@code
     *         FALSE} otherwise.
     */
    CARAPI IsRetryNeeded(
        /* [out] */ Boolean* result);

    /**
     * Return the timer that should be used to trigger the data reconnection
     */
    CARAPI GetRetryTimer(
        /* [out] */ Int32* result);

    /**
     * @return retry count
     */
    CARAPI GetRetryCount(
        /* [out] */ Int32* result);

    /**
     * Increase the retry counter, does not change retry forever.
     */
    CARAPI IncreaseRetryCount();

    /**
     * Set retry count to the specified value
     */
    CARAPI SetRetryCount(
        /* [in] */ Int32 count);

    /**
     * Set current maximum retry count to the specified value
     */
    CARAPI SetCurMaxRetryCount(
        /* [in] */ Int32 count);

    /**
     * Restore CurMaxRetryCount
     */
    CARAPI RestoreCurMaxRetryCount();

    /**
     * Set retry forever to the specified value
     */
    CARAPI SetRetryForever(
        /* [in] */ Boolean retryForever);

    /**
     * Clear the data-retry counter
     */
    CARAPI ResetRetryCount();

    /**
     * Retry forever using last timeout time.
     */
    CARAPI RetryForeverUsingLastTimeout();

    /**
     * @return TRUE if retrying forever
     */
    CARAPI IsRetryForever(
        /* [out] */ Boolean* result);

private:
    /**
     * Parse an integer validating the value is not negative.
     *
     * @param name
     * @param stringValue
     * @return Pair.first == TRUE if stringValue an integer >= 0
     */
    AutoPtr<IPair> ParseNonNegativeInt(
        /* [in] */ String name,
        /* [in] */ String stringValue);

    /**
     * Validate an integer is >= 0 and logs an error if not
     *
     * @param name
     * @param value
     * @return Pair.first
     */
    Boolean ValidateNonNegativeInt(
        /* [in] */ String name,
        /* [in] */ Int32 value);

    /**
     * Return next random number for the index
     */
    Int32 NextRandomizationTime(
        /* [in] */ Int32 index);

    void Log(
        /* [in] */ String s);

public:
    static String LOGTAG;
    static Boolean DBG;
    static Boolean VDBG;

private:
    /** The array of retry records */
    AutoPtr<IArrayList> mRetryArray;

    /** When TRUE IsRetryNeeded() will always return TRUE */
    Boolean mRetryForever;

    /**
     * The maximum number of retries to attempt before
     * isRetryNeeded returns FALSE
     */
    Int32 mMaxRetryCount;

    Int32 mCurMaxRetryCount;

    /** The current number of retries */
    Int32 mRetryCount;

    /** Random number generator */
    AutoPtr<IRandom> mRng;

    String mConfig;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_RETRYMANAGER_H__
