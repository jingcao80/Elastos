/**
 * Copyright (C) 2009 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.android.internal.telephony;

using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Utility::IPair;
using Elastos::Droid::Text::ITextUtils;

using Elastos::Utility::IRandom;
using Elastos::Utility::IArrayList;

/**
 * Retry manager allows a simple way to declare a series of
 * retry timeouts. After creating a RetryManager the configure
 * method is used to define the sequence. A simple linear series
 * may be initialized using configure with three integer parameters
 * The other configure method allows a series to be declared using
 * a string.
 *<p>
 * The format of the configuration string is a series of parameters
 * separated by a comma. There are two name value pair parameters plus a series
 * of delay times. The units of of these delay times is unspecified.
 * The name value pairs which may be specified are:
 *<ul>
 *<li>max_retries=<value>
 *<li>default_randomizationTime=<value>
 *</ul>
 *<p>
 * max_retries is the number of times that incrementRetryCount
 * maybe called before isRetryNeeded will return FALSE. if value
 * is infinite then isRetryNeeded will always return TRUE.
 *
 * default_randomizationTime will be used as the randomizationTime
 * for delay times which have no supplied randomizationTime. If
 * default_randomizationTime is not defined it defaults to 0.
 *<p>
 * The other parameters define The series of delay times and each
 * may have an optional randomization value separated from the
 * delay time by a colon.
 *<p>
 * Examples:
 * <ul>
 * <li>3 retries with no randomization value which means its 0:
 * <ul><li><code>"1000, 2000, 3000"</code></ul>
 *
 * <li>10 retries with a 500 default randomization value for each and
 * the 4..10 retries all using 3000 as the delay:
 * <ul><li><code>"max_retries=10, default_randomization=500, 1000, 2000, 3000"</code></ul>
 *
 * <li>4 retries with a 100 as the default randomization value for the first 2 values and
 * the other two having specified values of 500:
 * <ul><li><code>"default_randomization=100, 1000, 2000, 4000:500, 5000:500"</code></ul>
 *
 * <li>Infinite number of retries with the first one at 1000, the second at 2000 all
 * others will be at 3000.
 * <ul><li><code>"max_retries=infinite,1000,2000,3000</code></ul>
 * </ul>
 *
 * {@hide}
 */
public class RetryManager {
    static public final String LOG_TAG = "RetryManager";
    static public final Boolean DBG = FALSE;
    static public final Boolean VDBG = FALSE;

    /**
     * Retry record with times in milli-seconds
     */
    private static class RetryRec {
        RetryRec(Int32 delayTime, Int32 randomizationTime) {
            mDelayTime = delayTime;
            mRandomizationTime = randomizationTime;
        }

        Int32 mDelayTime;
        Int32 mRandomizationTime;
    }

    /** The array of retry records */
    private ArrayList<RetryRec> mRetryArray = new ArrayList<RetryRec>();

    /** When TRUE IsRetryNeeded() will always return TRUE */
    private Boolean mRetryForever;

    /**
     * The maximum number of retries to attempt before
     * isRetryNeeded returns FALSE
     */
    private Int32 mMaxRetryCount;

    private Int32 mCurMaxRetryCount;

    /** The current number of retries */
    private Int32 mRetryCount;

    /** Random number generator */
    private Random mRng = new Random();

    private String mConfig;

    /** Constructor */
    public RetryManager() {
        If (VDBG) Log("constructor");
    }

    //@Override
    CARAPI ToString(
        /* [out] */ String* str)
    {
        String ret = "RetryManager: { forever=" + mRetryForever + " maxRetry=" + mMaxRetryCount
                + " curMaxRetry=" + mCurMaxRetryCount + " retry=" + mRetryCount
                + " config={" + mConfig + "} retryArray={";
        For (RetryRec r : mRetryArray) {
            ret += r.mDelayTime + ":" + r.mRandomizationTime + " ";
        }
        ret += "}}";
        return ret;
    }

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
    public Boolean Configure(Int32 maxRetryCount, Int32 retryTime, Int32 randomizationTime) {
        Pair<Boolean, Integer> value;

        If (VDBG) Log("configure: " + maxRetryCount + ", " + retryTime + "," + randomizationTime);

        If (!ValidateNonNegativeInt("maxRetryCount", maxRetryCount)) {
            return FALSE;
        }

        If (!ValidateNonNegativeInt("retryTime", retryTime)) {
            return FALSE;
        }

        If (!ValidateNonNegativeInt("randomizationTime", randomizationTime)) {
            return FALSE;
        }

        mMaxRetryCount = maxRetryCount;
        mCurMaxRetryCount = mMaxRetryCount;

        ResetRetryCount();
        mRetryArray->Clear();
        mRetryArray->Add(new RetryRec(retryTime, randomizationTime));

        return TRUE;
    }

    /**
     * Configure for using string which allow arbitrary
     * sequences of times. See class comments for the
     * string format.
     *
     * @return TRUE if successful
     */
    public Boolean Configure(String configStr) {
        // Strip quotes if present.
        If ((configStr->StartsWith("\"") && configStr->EndsWith("\""))) {
            configStr = configStr->Substring(1, configStr->Length()-1);
        }
        If (VDBG) Log("configure: '" + configStr + "'");
        mConfig = configStr;

        If (!TextUtils->IsEmpty(configStr)) {
            Int32 defaultRandomization = 0;

            If (VDBG) Log("configure: not empty");

            mMaxRetryCount = 0;
            ResetRetryCount();
            mRetryArray->Clear();

            String strArray[] = configStr->Split(",");
            For (Int32 i = 0; i < strArray.length; i++) {
                If (VDBG) Log("configure: strArray[" + i + "]='" + strArray[i] + "'");
                Pair<Boolean, Integer> value;
                String splitStr[] = strArray[i].Split("=", 2);
                splitStr[0] = splitStr[0].Trim();
                If (VDBG) Log("configure: splitStr[0]='" + splitStr[0] + "'");
                If (splitStr.length > 1) {
                    splitStr[1] = splitStr[1].Trim();
                    If (VDBG) Log("configure: splitStr[1]='" + splitStr[1] + "'");
                    If (TextUtils->Equals(splitStr[0], "default_randomization")) {
                        value = ParseNonNegativeInt(splitStr[0], splitStr[1]);
                        If (!value.first) return FALSE;
                        defaultRandomization = value.second;
                    } else If (TextUtils->Equals(splitStr[0], "max_retries")) {
                        If (TextUtils->Equals("infinite",splitStr[1])) {
                            mRetryForever = TRUE;
                        } else {
                            value = ParseNonNegativeInt(splitStr[0], splitStr[1]);
                            If (!value.first) return FALSE;
                            mMaxRetryCount = value.second;
                        }
                    } else {
                        Rlog->E(LOG_TAG, "Unrecognized configuration name value pair: "
                                        + strArray[i]);
                        return FALSE;
                    }
                } else {
                    /**
                     * Assume a retry time with an optional randomization value
                     * following a ":"
                     */
                    splitStr = strArray[i].Split(":", 2);
                    splitStr[0] = splitStr[0].Trim();
                    RetryRec rr = new RetryRec(0, 0);
                    value = ParseNonNegativeInt("delayTime", splitStr[0]);
                    If (!value.first) return FALSE;
                    rr.mDelayTime = value.second;

                    // Check if optional randomization value present
                    If (splitStr.length > 1) {
                        splitStr[1] = splitStr[1].Trim();
                        If (VDBG) Log("configure: splitStr[1]='" + splitStr[1] + "'");
                        value = ParseNonNegativeInt("randomizationTime", splitStr[1]);
                        If (!value.first) return FALSE;
                        rr.mRandomizationTime = value.second;
                    } else {
                        rr.mRandomizationTime = defaultRandomization;
                    }
                    mRetryArray->Add(rr);
                }
            }
            If (mRetryArray->Size() > mMaxRetryCount) {
                mMaxRetryCount = mRetryArray->Size();
                If (VDBG) Log("configure: setting mMaxRetryCount=" + mMaxRetryCount);
            }
            mCurMaxRetryCount = mMaxRetryCount;
            If (VDBG) Log("configure: TRUE");
            return TRUE;
        } else {
            If (VDBG) Log("configure: FALSE it's empty");
            return FALSE;
        }
    }

    /**
     * Report whether data reconnection should be retried
     *
     * @return {@code TRUE} if the max retries has not been reached. {@code
     *         FALSE} otherwise.
     */
    public Boolean IsRetryNeeded() {
        Boolean retVal = mRetryForever || (mRetryCount < mCurMaxRetryCount);
        If (DBG) Log("isRetryNeeded: " + retVal);
        return retVal;
    }

    /**
     * Return the timer that should be used to trigger the data reconnection
     */
    public Int32 GetRetryTimer() {
        Int32 index;
        If (mRetryCount < mRetryArray->Size()) {
            index = mRetryCount;
        } else {
            index = mRetryArray->Size() - 1;
        }

        Int32 retVal;
        If ((index >= 0) && (index < mRetryArray->Size())) {
            retVal = mRetryArray->Get(index).mDelayTime + NextRandomizationTime(index);
        } else {
            retVal = 0;
        }

        If (DBG) Log("getRetryTimer: " + retVal);
        return retVal;
    }

    /**
     * @return retry count
     */
    public Int32 GetRetryCount() {
        If (DBG) Log("getRetryCount: " + mRetryCount);
        return mRetryCount;
    }

    /**
     * Increase the retry counter, does not change retry forever.
     */
    CARAPI IncreaseRetryCount() {
        mRetryCount++;
        If (mRetryCount > mCurMaxRetryCount) {
            mRetryCount = mCurMaxRetryCount;
        }
        If (DBG) Log("increaseRetryCount: " + mRetryCount);
    }

    /**
     * Set retry count to the specified value
     */
    CARAPI SetRetryCount(Int32 count) {
        mRetryCount = count;
        If (mRetryCount > mCurMaxRetryCount) {
            mRetryCount = mCurMaxRetryCount;
        }

        If (mRetryCount < 0) {
            mRetryCount = 0;
        }

        If (DBG) Log("setRetryCount: " + mRetryCount);
    }

    /**
     * Set current maximum retry count to the specified value
     */
    CARAPI SetCurMaxRetryCount(Int32 count) {
        mCurMaxRetryCount = count;

        // Make sure it's not negative
        If (mCurMaxRetryCount < 0) {
            mCurMaxRetryCount = 0;
        }

        // Make sure mRetryCount is within range
        SetRetryCount(mRetryCount);

        If (DBG) Log("setCurMaxRetryCount: " + mCurMaxRetryCount);
    }

    /**
     * Restore CurMaxRetryCount
     */
    CARAPI RestoreCurMaxRetryCount() {
        mCurMaxRetryCount = mMaxRetryCount;

        // Make sure mRetryCount is within range
        SetRetryCount(mRetryCount);
    }

    /**
     * Set retry forever to the specified value
     */
    CARAPI SetRetryForever(Boolean retryForever) {
        mRetryForever = retryForever;
        If (DBG) Log("setRetryForever: " + mRetryForever);
    }

    /**
     * Clear the data-retry counter
     */
    CARAPI ResetRetryCount() {
        mRetryCount = 0;
        If (DBG) Log("resetRetryCount: " + mRetryCount);
    }

    /**
     * Retry forever using last timeout time.
     */
    CARAPI RetryForeverUsingLastTimeout() {
        mRetryCount = mCurMaxRetryCount;
        mRetryForever = TRUE;
        If (DBG) Log("retryForeverUsingLastTimeout: " + mRetryForever + ", " + mRetryCount);
    }

    /**
     * @return TRUE if retrying forever
     */
    public Boolean IsRetryForever() {
        If (DBG) Log("isRetryForever: " + mRetryForever);
        return mRetryForever;
    }

    /**
     * Parse an integer validating the value is not negative.
     *
     * @param name
     * @param stringValue
     * @return Pair.first == TRUE if stringValue an integer >= 0
     */
    private Pair<Boolean, Integer> ParseNonNegativeInt(String name, String stringValue) {
        Int32 value;
        Pair<Boolean, Integer> retVal;
        try {
            value = Integer->ParseInt(stringValue);
            retVal = new Pair<Boolean, Integer>(ValidateNonNegativeInt(name, value), value);
        } Catch (NumberFormatException e) {
            Rlog->E(LOG_TAG, name + " bad value: " + stringValue, e);
            retVal = new Pair<Boolean, Integer>(FALSE, 0);
        }
        If (VDBG) Log("parseNonNetativeInt: " + name + ", " + stringValue + ", "
                    + retVal.first + ", " + retVal.second);
        return retVal;
    }

    /**
     * Validate an integer is >= 0 and logs an error if not
     *
     * @param name
     * @param value
     * @return Pair.first
     */
    private Boolean ValidateNonNegativeInt(String name, Int32 value) {
        Boolean retVal;
        If (value < 0) {
            Rlog->E(LOG_TAG, name + " bad value: is < 0");
            retVal = FALSE;
        } else {
            retVal = TRUE;
        }
        If (VDBG) Log("validateNonNegative: " + name + ", " + value + ", " + retVal);
        return retVal;
    }

    /**
     * Return next random number for the index
     */
    private Int32 NextRandomizationTime(Int32 index) {
        Int32 randomTime = mRetryArray->Get(index).mRandomizationTime;
        If (randomTime == 0) {
            return 0;
        } else {
            return mRng->NextInt(randomTime);
        }
    }

    private void Log(String s) {
        Rlog->D(LOG_TAG, "[RM] " + s);
    }
}
