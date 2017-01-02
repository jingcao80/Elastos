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

#ifndef __ELASTOS_DROID_PROVIDER_CCALLS_H__
#define __ELASTOS_DROID_PROVIDER_CCALLS_H__

#include "_Elastos_Droid_Provider_CCalls.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Telephony::ICallerInfo;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Telecom::IPhoneAccountHandle;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CCalls)
    , public Singleton
    , public ICalls
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * The content:// style URL for this table
     */
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * The content:// style URL for filtering this table on phone numbers
     */
    CARAPI GetCONTENT_FILTER_URI(
        /* [out] */ IUri** uri);

    /**
     * Content uri with {@link #ALLOW_VOICEMAILS_PARAM_KEY} set. This can directly be used to
     * access call log entries that includes voicemail records.
     *
     * @hide
     */
    CARAPI GetCONTENT_URI_WITH_VOICEMAIL(
        /* [out] */ IUri** uri);

    /**
     * Adds a call to the call log.
     *
     * @param ci the CallerInfo object to get the target contact from.  Can be null
     * if the contact is unknown.
     * @param context the context used to get the ContentResolver
     * @param number the phone number to be added to the calls db
     * @param presentation enum value from PhoneConstants.PRESENTATION_xxx, which
     *        is set by the network and denotes the number presenting rules for
     *        "allowed", "payphone", "restricted" or "unknown"
     * @param callType enumerated values for "incoming", "outgoing", or "missed"
     * @param features features of the call (e.g. Video).
     * @param accountHandle The accountHandle object identifying the provider of the call
     * @param start time stamp for the call in milliseconds
     * @param duration call duration in seconds
     * @param dataUsage data usage for the call in bytes, null if data usage was not tracked for
     *                  the call.
     * @result The URI of the call log entry belonging to the user that made or received this
     *        call.
     * {@hide}
     */
    CARAPI AddCall(
        /* [in] */ ICallerInfo* ci,
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ Int32 presentation,
        /* [in] */ Int32 callType,
        /* [in] */ Int32 features,
        /* [in] */ IPhoneAccountHandle* accountHandle,
        /* [in] */ Int64 start,
        /* [in] */ Int32 duration,
        /* [in] */ Int64 dataUsage,
        /* [out] */ IUri** uri);

    /**
     * Adds a call to the call log.
     *
     * @param ci the CallerInfo object to get the target contact from.  Can be null
     * if the contact is unknown.
     * @param context the context used to get the ContentResolver
     * @param number the phone number to be added to the calls db
     * @param presentation enum value from PhoneConstants.PRESENTATION_xxx, which
     *        is set by the network and denotes the number presenting rules for
     *        "allowed", "payphone", "restricted" or "unknown"
     * @param callType enumerated values for "incoming", "outgoing", or "missed"
     * @param features features of the call (e.g. Video).
     * @param accountHandle The accountHandle object identifying the provider of the call
     * @param start time stamp for the call in milliseconds
     * @param duration call duration in seconds
     * @param dataUsage data usage for the call in bytes, null if data usage was not tracked for
     *                  the call.
     * @param addForAllUsers If true, the call is added to the call log of all currently
     *        running users. The caller must have the MANAGE_USERS permission if this is true.
     *
     * @result The URI of the call log entry belonging to the user that made or received this
     *        call.
     * {@hide}
     */
    CARAPI AddCall(
        /* [in] */ ICallerInfo* ci,
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ Int32 presentation,
        /* [in] */ Int32 callType,
        /* [in] */ Int32 features,
        /* [in] */ IPhoneAccountHandle* handle,
        /* [in] */ Int64 start,
        /* [in] */ Int32 duration,
        /* [in] */ Int64 dataUsage,
        /* [in] */ Boolean addForAllUsers,
        /* [out] */ IUri** uri);

    /**
     * Adds a call to the call log.
     *
     * @param ci the CallerInfo object to get the target contact from.  Can be null
     * if the contact is unknown.
     * @param context the context used to get the ContentResolver
     * @param number the phone number to be added to the calls db
     * @param presentation enum value from PhoneConstants.PRESENTATION_xxx, which
     *        is set by the network and denotes the number presenting rules for
     *        "allowed", "payphone", "restricted" or "unknown"
     * @param callType enumerated values for "incoming", "outgoing", or "missed"
     * @param features features of the call (e.g. Video).
     * @param accountHandle The accountHandle object identifying the provider of the call
     * @param start time stamp for the call in milliseconds
     * @param duration call duration in seconds
     * @param dataUsage data usage for the call in bytes, null if data usage was not tracked for
     *                  the call.
     * @param addForAllUsers If true, the call is added to the call log of all currently
     *        running users. The caller must have the MANAGE_USERS permission if this is true.
     * @param durationType valid value is 0 or 1.
     *
     * @result The URI of the call log entry belonging to the user that made or received this
     *        call.
     * {@hide}
     */
    CARAPI AddCall(
        /* [in] */ ICallerInfo* ci,
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ Int32 presentation,
        /* [in] */ Int32 callType,
        /* [in] */ Int32 features,
        /* [in] */ IPhoneAccountHandle* handle,
        /* [in] */ Int64 start,
        /* [in] */ Int32 duration,
        /* [in] */ Int64 dataUsage,
        /* [in] */ Boolean addForAllUsers,
        /* [in] */ Int32 durationType,
        /* [out] */ IUri** uri);

    /**
     * Query the call log database for the last dialed number.
     * @param context Used to get the content resolver.
     * @return The last phone number dialed (outgoing) or an empty
     * string if none exist yet.
     */
    CARAPI GetLastOutgoingCall(
        /* [in] */ IContext* context,
        /* [out] */ String* call);

private:
    static CARAPI AddEntryAndRemoveExpiredEntries(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [in] */ IContentValues* values,
        /* [out] */ IUri** ret);

    static CARAPI_(void) UpdateDataUsageStatForData(
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& dataId);

    static CARAPI_(void) UpdateNormalizedNumber(
        /* [in] */ IContext* context,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ const String& dataId,
        /* [in] */ const String& number);

    static CARAPI_(String) GetCurrentCountryIso(
        /* [in] */ IContext* context);

public:
    static AutoPtr<IUri> CONTENT_URI;

    static AutoPtr<IUri> CONTENT_FILTER_URI;

    static AutoPtr<IUri> CONTENT_URI_WITH_VOICEMAIL;

    static const Int32 MIN_DURATION_FOR_NORMALIZED_NUMBER_UPDATE_MS;
};

} //Provider
} //Droid
} //Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCALLS_H__
