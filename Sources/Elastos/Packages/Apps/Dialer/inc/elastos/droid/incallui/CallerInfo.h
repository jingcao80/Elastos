
#ifndef __ELASTOS_DROID_INCALLUI_CALLERINFO_H__
#define __ELASTOS_DROID_INCALLUI_CALLERINFO_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Proxy class for getting and setting the audio mode.
 */
class CallerInfo
    : Object
    , ICallerInfo
{
public:
    CallerInfo();

    CAR_INTERFACE_DECL()

    /**
     * getCallerInfo given a Cursor.
     * @param context the context used to retrieve string constants
     * @param contactRef the URI to attach to this CallerInfo object
     * @param cursor the first object in the cursor is used to build the CallerInfo object.
     * @return the CallerInfo which contains the caller id for the given
     * number. The returned CallerInfo is null if no number is supplied.
     */
    static CARAPI_(AutoPtr<CallerInfo>) GetCallerInfo(
        /* [in] */ IContext* context,
        /* [in] */ IUri* contactRef,
        /* [in] */ ICursor* cursor);

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
    static CARAPI_(AutoPtr<CallerInfo>) DoSecondaryLookupIfNecessary(
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
        /* [in] */ CallerInfo* previousResult);

    // Accessors

    /**
     * @return true if the caller info is an emergency number.
     */
    CARAPI_(Boolean) IsEmergencyNumber();

    /**
     * @return true if the caller info is a voicemail number.
     */
    CARAPI_(Boolean) IsVoiceMailNumber();

    /**
     * Mark this CallerInfo as an emergency call.
     * @param context To lookup the localized 'Emergency Number' string.
     * @return this instance.
     */
    // TODO: Note we're setting the phone number here (refer to
    // javadoc comments at the top of CallerInfo class) to a localized
    // string 'Emergency Number'. This is pretty bad because we are
    // making UI work here instead of just packaging the data. We
    // should set the phone number to the dialed number and name to
    // 'Emergency Number' and let the UI make the decision about what
    // should be displayed.
    /* package */
    CARAPI_(AutoPtr<CallerInfo>) MarkAsEmergency(
        /* [in] */ IContext* context);

    /**
     * Mark this CallerInfo as a voicemail call. The voicemail label
     * is obtained from the telephony manager. Caller must hold the
     * READ_PHONE_STATE permission otherwise the phoneNumber will be
     * set to null.
     * @return this instance.
     */
    // TODO: As in the emergency number handling, we end up writing a
    // string in the phone number field.
    /* package */
    CARAPI_(AutoPtr<CallerInfo>) MarkAsVoiceMail(
        /* [in] */ IContext* context);

    /**
     * Updates this CallerInfo's geoDescription field, based on the raw
     * phone number in the phoneNumber field.
     *
     * (Note that the various getCallerInfo() methods do *not* set the
     * geoDescription automatically; you need to call this method
     * explicitly to get it.)
     *
     * @param context the context used to look up the current locale / country
     * @param fallbackNumber if this CallerInfo's phoneNumber field is empty,
     *        this specifies a fallback number to use instead.
     */
    CARAPI_(void) UpdateGeoDescription(
        /* [in] */ IContext* context,
        /* [in] */ const String& fallbackNumber);

    /**
     * @return a string debug representation of this instance.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    /**
     * getCallerInfo given a URI, look up in the call-log database
     * for the uri unique key.
     * @param context the context used to get the ContentResolver
     * @param contactRef the URI used to lookup caller id
     * @return the CallerInfo which contains the caller id for the given
     * number. The returned CallerInfo is null if no number is supplied.
     */
    static CARAPI_(AutoPtr<CallerInfo>) GetCallerInfo(
        /* [in] */ IContext* context,
        /* [in] */ IUri* contactRef);

    static CARAPI_(String) Normalize(
        /* [in] */ const String& s);

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
    static CARAPI_(Int32) GetColumnIndexForPersonId(
        /* [in] */ IUri* contactRef,
        /* [in] */ ICursor* cursor);

    /**
     * @return a geographical description string for the specified number.
     * @see com.android.i18n.phonenumbers.PhoneNumberOfflineGeocoder
     */
    static CARAPI_(String) GetGeoDescription(
        /* [in] */ IContext* context,
        /* [in] */ const String& number);

public:
    /**
     * Please note that, any one of these member variables can be null,
     * and any accesses to them should be prepared to handle such a case.
     *
     * Also, it is implied that phoneNumber is more often populated than
     * name is, (think of calls being dialed/received using numbers where
     * names are not known to the device), so phoneNumber should serve as
     * a dependable fallback when name is unavailable.
     *
     * One other detail here is that this CallerInfo object reflects
     * information found on a connection, it is an OUTPUT that serves
     * mainly to display information to the user.  In no way is this object
     * used as input to make a connection, so we can choose to display
     * whatever human-readable text makes sense to the user for a
     * connection.  This is especially relevant for the phone number field,
     * since it is the one field that is most likely exposed to the user.
     *
     * As an example:
     *   1. User dials "911"
     *   2. Device recognizes that this is an emergency number
     *   3. We use the "Emergency Number" string instead of "911" in the
     *     phoneNumber field.
     *
     * What we're really doing here is treating phoneNumber as an essential
     * field here, NOT name.  We're NOT always guaranteed to have a name
     * for a connection, but the number should be displayable.
     */
    String mName;
    String mPhoneNumber;
    String mNormalizedNumber;
    String mForwardingNumber;
    String mGeoDescription;

    String mCnapName;
    Int32 mNumberPresentation;
    Int32 mNamePresentation;
    Boolean mContactExists;

    String mPhoneLabel;
    /* Split up the phoneLabel into number type and label name */
    Int32 mNumberType;
    String mNumberLabel;

    Int32 mPhotoResource;

    // Contact ID, which will be 0 if a contact comes from the corp CP2.
    Int64 mContactIdOrZero;
    String mLookupKeyOrNull;
    Boolean mNeedUpdate;
    AutoPtr<IUri> mContactRefUri;

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

    Boolean mIsEmergency;
    Boolean mIsVoiceMail;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CALLERINFO_H__
