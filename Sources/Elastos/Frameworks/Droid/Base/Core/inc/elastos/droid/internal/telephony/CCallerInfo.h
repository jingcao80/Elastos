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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFO_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFO_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Telephony.h"
#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos_Droid_Internal_Telephony_CCallerInfo.h"
#include "elastos/droid/ext/frameworkdef.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::IUri;
using Elastos::Utility::ILocale;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CCallerInfo)
    , public Object
    , public ICallerInfo
{
public:
    CCallerInfo();

    virtual ~CCallerInfo();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI IsEmergencyNumber(
        /* [out] */ Boolean* result);

    CARAPI IsVoiceMailNumber(
        /* [out] */ Boolean* result);

    CARAPI MarkAsEmergency(
        /* [in] */ IContext* context,
        /* [out] */ ICallerInfo** callerInfo);

    CARAPI MarkAsVoiceMail(
        /* [out] */ ICallerInfo** callerInfo);

    CARAPI MarkAsVoiceMail(
        /* [in] */ Int64 subId,
        /* [out] */ ICallerInfo** callerInfo);

    CARAPI UpdateGeoDescription(
        /* [in] */ IContext* context,
        /* [in] */ const String& fallbackNumber);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI SetName(
        /* [in] */ const String& name);

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetPhoneNumber(
        /* [in] */ const String& phoneNumber);

    CARAPI GetPhoneNumber(
        /* [out] */ String* phoneNumber);

    CARAPI SetNormalizedNumber(
        /* [in] */ const String& normalizedNumber);

    CARAPI GetNormalizedNumber(
        /* [out] */ String* normalizedNumber);

    CARAPI SetGeoDescription(
        /* [in] */ const String& geoDescription);

    CARAPI GetGeoDescription(
        /* [out] */ String* geoDescription);

    CARAPI SetCnapName(
        /* [in] */ const String& cnapName);

    CARAPI GetCnapName(
        /* [out] */ String* cnapName);

    CARAPI SetNumberPresentation(
        /* [in] */ Int32 numberPresentation);

    CARAPI GetNumberPresentation(
        /* [out] */ Int32* numberPresentation);

    CARAPI SetNamePresentation(
        /* [in] */ Int32 namePresentation);

    CARAPI GetNamePresentation(
        /* [out] */ Int32* namePresentation);

    CARAPI SetContactExists(
        /* [in] */ Boolean contactExists);

    CARAPI GetContactExists(
        /* [out] */ Boolean* contactExists);

    CARAPI SetPhoneLabel(
        /* [in] */ const String& phoneLabel);

    CARAPI GetPhoneLabel(
        /* [out] */ String* phoneLabel);

    CARAPI SetNumberType(
        /* [in] */ Int32 numberType);

    CARAPI GetNumberType(
        /* [out] */ Int32* numberType);

    CARAPI SetNumberLabel(
        /* [in] */ const String& numberLabel);

    CARAPI GetNumberLabel(
        /* [out] */ String* numberLabel);

    CARAPI SetPhotoResource(
        /* [in] */ Int32 photoResource);

    CARAPI GetPhotoResource(
        /* [out] */ Int32* photoResource);

    CARAPI SetContactIdOrZero(
        /* [in] */ Int64 contactIdOrZero);

    CARAPI GetContactIdOrZero(
        /* [out] */ Int64* contactIdOrZero);

    CARAPI SetNeedUpdate(
        /* [in] */ Boolean needUpdate);

    CARAPI GetNeedUpdate(
        /* [out] */ Boolean* needUpdate);

    CARAPI SetContactRefUri(
        /* [in] */ IUri* contactRefUri);

    CARAPI GetContactRefUri(
        /* [out] */ IUri** contactRefUri);

    CARAPI SetLookupKey(
        /* [in] */ const String& lookupKey);

    CARAPI GetLookupKey(
        /* [out] */ String* lookupKey);

    CARAPI SetContactDisplayPhotoUri(
        /* [in] */ IUri* contactDisplayPhotoUri);

    CARAPI GetContactDisplayPhotoUri(
        /* [out] */ IUri** contactDisplayPhotoUri);

    CARAPI SetContactRingtoneUri(
        /* [in] */ IUri* contactRingtoneUri);

    CARAPI GetContactRingtoneUri(
        /* [out] */ IUri** contactRingtoneUri);

    CARAPI SetShouldSendToVoicemail(
        /* [in] */ Boolean shouldSendToVoicemail);

    CARAPI GetShouldSendToVoicemail(
        /* [out] */ Boolean* shouldSendToVoicemail);

    CARAPI SetCachedPhoto(
        /* [in] */ IDrawable* cachedPhoto);

    CARAPI GetCachedPhoto(
        /* [out] */ IDrawable** cachedPhoto);

    CARAPI SetCachedPhotoIcon(
        /* [in] */ IBitmap* cachedPhotoIcon);

    CARAPI GetCachedPhotoIcon(
        /* [out] */ IBitmap** cachedPhotoIcon);

    CARAPI SetIsCachedPhotoCurrent(
        /* [in] */ Boolean isCachedPhotoCurrent);

    CARAPI GetIsCachedPhotoCurrent(
        /* [out] */ Boolean* isCachedPhotoCurrent);

    /**
     * getCallerInfo given a Cursor.
     * @param context the context used to retrieve string constants
     * @param contactRef the URI to attach to this CallerInfo object
     * @param cursor the first object in the cursor is used to build the CallerInfo object.
     * @return the CallerInfo which contains the caller id for the given
     * number. The returned CallerInfo is null if no number is supplied.
     */
    static CARAPI GetCallerInfo(
        /* [in] */ IContext* ctx,
        /* [in] */ IUri* contactRef,
        /* [in] */ ICursor* cursor,
        /* [out] */ ICallerInfo** callerInfo);

    /**
     * getCallerInfo given a URI, look up in the call-log database
     * for the uri unique key.
     * @param context the context used to get the ContentResolver
     * @param contactRef the URI used to lookup caller id
     * @return the CallerInfo which contains the caller id for the given
     * number. The returned CallerInfo is null if no number is supplied.
     */
    static CARAPI GetCallerInfo(
        /* [in] */ IContext* ctx,
        /* [in] */ IUri* contactRef,
        /* [out] */ ICallerInfo** callerInfo);

    /**
     * getCallerInfo given a phone number, look up in the call-log database
     * for the matching caller id info.
     * @param context the context used to get the ContentResolver
     * @param number the phone number used to lookup caller id
     * @return the CallerInfo which contains the caller id for the given
     * number. The returned CallerInfo is null if no number is supplied. If
     * a matching number is not found, then a generic caller info is returned,
     * with all relevant fields empty or null.
     */
    static CARAPI GetCallerInfo(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& number,
        /* [out] */ ICallerInfo** callerInfo);

    /**
     * getCallerInfo given a phone number and subscription, look up in the call-log database
     * for the matching caller id info.
     * @param context the context used to get the ContentResolver
     * @param number the phone number used to lookup caller id
     * @param subId the subscription for checking for if voice mail number or not
     * @return the CallerInfo which contains the caller id for the given
     * number. The returned CallerInfo is null if no number is supplied. If
     * a matching number is not found, then a generic caller info is returned,
     * with all relevant fields empty or null.
     */
    static CARAPI GetCallerInfo(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& number,
        /* [in] */ Int64 subId,
        /* [out] */ ICallerInfo** callerInfo);

    /**
     * Performs another lookup if previous lookup fails and it's a SIP call
     * and the peer's username is all numeric. Look up the username as it
     * could be a PSTN number in the contact database.
     *
     * @param context the query context
     * @param number the original phone number, could be a SIP URI
     * @param previousResult the result of previous lookup
     * @return previousResult if it's not the case
     */
    static CARAPI DoSecondaryLookupIfNecessary(
        /* [in] */ IContext* ctx,
        /* [in] */ const String& number,
        /* [in] */ ICallerInfo* previousResult,
        /* [out] */ ICallerInfo** callerInfo);

    static CARAPI_(String) GetCurrentCountryIso(
        /* [in] */ IContext* context);

private:
    static CARAPI Normalize(
        /* [in] */ const String& s,
        /* [out] */ String* res);

    /**
     * Returns the column index to use to find the "person_id" field in
     * the specified cursor, based on the contact URI that was originally
     * queried.
     *
     * This is a helper function for the getCallerInfo() method that takes
     * a Cursor.  Looking up the person_id is nontrivial (compared to all
     * the other CallerInfo fields) since the column we need to use
     * depends on what query we originally ran.
     *
     * Watch out: be sure to not do any database access in this method, since
     * it's run from the UI thread (see comments below for more info.)
     *
     * @return the columnIndex to use (with cursor.getLong()) to get the
     * person_id, or -1 if we couldn't figure out what colum to use.
     *
     * TODO: Add a unittest for this method.  (This is a little tricky to
     * test, since we'll need a live contacts database to test against,
     * preloaded with at least some phone numbers and SIP addresses.  And
     * we'll probably have to hardcode the column indexes we expect, so
     * the test might break whenever the contacts schema changes.  But we
     * can at least make sure we handle all the URI patterns we claim to,
     * and that the mime types match what we expect...)
     */
    static CARAPI GetColumnIndexForPersonId(
        /* [in] */ IUri* contactRef,
        /* [in] */ ICursor* cursor,
        /* [out] */ Int32* res);

    /**
     * @return a geographical description string for the specified number.
     * @see com.android.i18n.phonenumbers.PhoneNumberOfflineGeocoder
     */
    static CARAPI_(String) GetGeoDescription(
        /* [in] */ IContext* context,
        /* [in] */ const String& number);

    /**
     * @return The ISO 3166-1 two letters country code of the country the user
     *         is in.
     */
    static CARAPI_(String) GetCurrentCountryIso(
        /* [in] */ IContext* context,
        /* [in] */ ILocale* locale);

public:
    String mName;
    String mPhoneNumber;
    String mNormalizedNumber;
    String mGeoDescription;

    String mCnapName;
    Int32 mNumberPresentation;
    Int32 mNamePresentation;
    Boolean mContactExists;

    String mPhoneLabel;
    /* Split up the phoneLabel into number type and label name */
    Int32  mNumberType;
    String mNumberLabel;

    Int32 mPhotoResource;

    // Contact ID, which will be 0 if a contact comes from the corp CP2.
    Int64 mContactIdOrZero;
    Boolean mNeedUpdate;
    AutoPtr<IUri> mContactRefUri;
    String mLookupKey;

    /**
     * Contact display photo URI.  If a contact has no display photo but a thumbnail, it'll be
     * the thumbnail URI instead.
     */
    AutoPtr<IUri> mContactDisplayPhotoUri;

    // fields to hold individual contact preference data,
    // including the send to voicemail flag and the ringtone
    // uri reference.
    AutoPtr<IUri> mContactRingtoneUri;
    Boolean mShouldSendToVoicemail;

    /**
    * Drawable representing the caller image.  This is essentially
    * a cache for the image data tied into the connection /
    * callerinfo object.
    *
    * This might be a high resolution picture which is more suitable
    * for full-screen image view than for smaller icons used in some
    * kinds of notifications.
    *
    * The {@link #isCachedPhotoCurrent} flag indicates if the image
    * data needs to be reloaded.
    */
    AutoPtr<IDrawable> mCachedPhoto;
    /**
    * Bitmap representing the caller image which has possibly lower
    * resolution than {@link #cachedPhoto} and thus more suitable for
    * icons (like notification icons).
    *
    * In usual cases this is just down-scaled image of {@link #cachedPhoto}.
    * If the down-scaling fails, this will just become null.
    *
    * The {@link #isCachedPhotoCurrent} flag indicates if the image
    * data needs to be reloaded.
    */
    AutoPtr<IBitmap> mCachedPhotoIcon;
    /**
    * Boolean which indicates if {@link #cachedPhoto} and
    * {@link #cachedPhotoIcon} is fresh enough. If it is false,
    * those images aren't pointing to valid objects.
    */
    Boolean mIsCachedPhotoCurrent;

private:
    static const String TAG;
    static const Boolean VDBG;

    Boolean mIsEmergency;
    Boolean mIsVoiceMail;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFO_H__
