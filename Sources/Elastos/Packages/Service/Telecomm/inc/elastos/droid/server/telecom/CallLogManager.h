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

#ifndef __ELASTOS_DROID_SERVER_TELECOM_CALLLOGMANAGER_H__
#define __ELASTOS_DROID_SERVER_TELECOM_CALLLOGMANAGER_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include "elastos/droid/server/telecom/Call.h"
#include "elastos/droid/server/telecom/CallsManagerListenerBase.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/os/AsyncTask.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::ICallerInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Telecom::IPhoneAccountHandle;
using Elastos::Core::IInteger64;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * Helper class that provides functionality to write information about calls and their associated
 * caller details to the call log. All logging activity will be performed asynchronously in a
 * background thread to avoid blocking on the main thread.
 */
class CallLogManager
    : public CallsManagerListenerBase
{
private:
    /**
     * Parameter object to hold the arguments to add a call in the call log DB.
     */
    class AddCallArgs
        : public Object
    {
    public:
        /**
         * @param callerInfo Caller details.
         * @param number The phone number to be logged.
         * @param presentation Number presentation of the phone number to be logged.
         * @param callType The type of call (e.g INCOMING_TYPE). @see
         *     {@link android.provider.CallLog} for the list of values.
         * @param features The features of the call (e.g. FEATURES_VIDEO). @see
         *     {@link android.provider.CallLog} for the list of values.
         * @param creationDate Time when the call was created (milliseconds since epoch).
         * @param durationInMillis Duration of the call (milliseconds).
         * @param dataUsage Data usage in bytes, or null if not applicable.
         */
        AddCallArgs(
            /* [in] */ IContext* context,
            /* [in] */ ICallerInfo* callerInfo,
            /* [in] */ const String& number,
            /* [in] */ Int32 presentation,
            /* [in] */ Int32 callType,
            /* [in] */ Int32 features,
            /* [in] */ IPhoneAccountHandle* accountHandle,
            /* [in] */ Int64 creationDate,
            /* [in] */ Int64 durationInMillis,
            /* [in] */ IInteger64* dataUsage);

        // Since the members are accessed directly, we don't use the
        // mXxxx notation.
        AutoPtr<IContext> mContext;

        AutoPtr<ICallerInfo> mCallerInfo;

        const String mNumber;

        Int32 mPresentation;

        Int32 mCallType;

        Int32 mFeatures;

        AutoPtr<IPhoneAccountHandle> mAccountHandle;

        Int64 mTimestamp;

        Int32 mDurationInSec;

        AutoPtr<IInteger64> mDataUsage;
    };

    /**
     * Helper AsyncTask to access the call logs database asynchronously since database operations
     * can take a long time depending on the system's load. Since it extends AsyncTask, it uses
     * its own thread pool.
     */
    class LogCallAsyncTask
        : public AsyncTask
    {
    public:
        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* callList,
            /* [out, callee] */ IInterface** result);

        /**
         * Performs a simple sanity check to make sure the call was written in the database.
         * Typically there is only one result per call so it is easy to identify which one failed.
         */
        // @Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);
    };

public:
    CARAPI constructor(
        /* [in] */ IContext* context);

    // @Override
    CARAPI OnCallStateChanged(
        /* [in] */ ICall* call,
        /* [in] */ Int32 oldState,
        /* [in] */ Int32 newState);

    /**
     * Logs a call to the call log based on the {@link Call} object passed in.
     *
     * @param call The call object being logged
     * @param callLogType The type of call log entry to log this call as. See:
     *     {@link android.provider.CallLog.Calls#INCOMING_TYPE}
     *     {@link android.provider.CallLog.Calls#OUTGOING_TYPE}
     *     {@link android.provider.CallLog.Calls#MISSED_TYPE}
     */
    CARAPI LogCall(
        /* [in] */ ICall* call,
        /* [in] */ Int32 callLogType);

    /**
     * Adds the call defined by the parameters in the provided AddCallArgs to the CallLogProvider
     * using an AsyncTask to avoid blocking the main thread.
     *
     * @param args Prepopulated call details.
     * @return A handle to the AsyncTask that will add the call to the call log asynchronously.
     */
    CARAPI LogCallAsync(
        /* [in] */ AddCallArgs* args,
        /* [out] */ AsyncTask** result);

private:
    /**
     * Inserts a call into the call log, based on the parameters passed in.
     *
     * @param callerInfo Caller details.
     * @param number The number the call was made to or from.
     * @param presentation
     * @param callType The type of call.
     * @param features The features of the call.
     * @param start The start time of the call, in milliseconds.
     * @param duration The duration of the call, in milliseconds.
     * @param dataUsage The data usage for the call, null if not applicable.
     */
    CARAPI LogCall(
        /* [in] */ ICallerInfo* callerInfo,
        /* [in] */ const String& number,
        /* [in] */ Int32 presentation,
        /* [in] */ Int32 callType,
        /* [in] */ Int32 features,
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [in] */ Int64 start,
        /* [in] */ Int64 duration,
        /* [in] */ IInteger64* dataUsage);

    /**
     * Based on the video state of the call, determines the call features applicable for the call.
     *
     * @param videoState The video state.
     * @return The call features.
     */
    static CARAPI GetCallFeatures(
        /* [in] */ Int32 videoState,
        /* [out] */ Int32* result);

    /**
     * Retrieve the phone number from the call, and then process it before returning the
     * actual number that is to be logged.
     *
     * @param call The phone connection.
     * @return the phone number to be logged.
     */
    CARAPI GetLogNumber(
        /* [in] */ ICall* call,
        /* [out] */ String* result);

    /**
     * Gets the presentation from the {@link Call}.
     *
     * TODO: There needs to be a way to pass information from
     * Connection.getNumberPresentation() into a {@link Call} object. Until then, always return
     * PhoneConstants.PRESENTATION_ALLOWED. On top of that, we might need to introduce
     * getNumberPresentation to the ContactInfo object as well.
     *
     * @param call The call object to retrieve caller details from.
     * @return The number presentation constant to insert into the call logs.
     */
    CARAPI GetPresentation(
        /* [in] */ ICall* call,
        /* [out] */ Int32* result);

    CARAPI SendAddCallBroadcast(
        /* [in] */ Int32 callType,
        /* [in] */ Int64 duration);

private:
    static const String TAG;

    AutoPtr<IContext> mContext;

    static const String ACTION_CALLS_TABLE_ADD_ENTRY;

    static const String PERMISSION_PROCESS_CALLLOG_INFO;

    static const String CALL_TYPE;

    static const String CALL_DURATION;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_CALLLOGMANAGER_H__
