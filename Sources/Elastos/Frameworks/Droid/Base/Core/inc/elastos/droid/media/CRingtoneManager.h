
#ifndef __ELASTOS_DROID_MEDIA_CRINGTONEMANAGER_H__
#define __ELASTOS_DROID_MEDIA_CRINGTONEMANAGER_H__

#include "_Elastos_Droid_Media_CRingtoneManager.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::App::IActivity;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CRingtoneManager)
    , public Object
    , public IRingtoneManager
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CRingtoneManager();

    virtual ~CRingtoneManager();

    /**
     * Constructs a RingtoneManager. This constructor is recommended as its
     * constructed instance manages cursor(s).
     *
     * @param activity The activity used to get a managed cursor.
     */
    CARAPI constructor(
        /* [in] */ IActivity* activity);

    /**
     * Constructs a RingtoneManager. The instance constructed by this
     * constructor will not manage the cursor(s), so the client should handle
     * this itself.
     *
     * @param context The context to used to get a cursor.
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Sets which type(s) of ringtones will be listed by this.
     *
     * @param type The type(s), one or more of {@link #TYPE_RINGTONE},
     *            {@link #TYPE_NOTIFICATION}, {@link #TYPE_ALARM},
     *            {@link #TYPE_ALL}.
     * @see #EXTRA_RINGTONE_TYPE
     */
    CARAPI SetType(
        /* [in] */ Int32 type);

    /**
     * Infers the playback stream type based on what type of ringtones this
     * manager is returning.
     *
     * @return The stream type.
     */
    CARAPI InferStreamType(
        /* [out] */ Int32* result);

    /**
     * Whether retrieving another {@link Ringtone} will stop playing the
     * previously retrieved {@link Ringtone}.
     * <p>
     * If this is false, make sure to {@link Ringtone#stop()} any previous
     * ringtones to free resources.
     *
     * @param stopPreviousRingtone If TRUE, the previously retrieved
     *            {@link Ringtone} will be stopped.
     */
    CARAPI SetStopPreviousRingtone(
        /* [in] */ Boolean stopPreviousRingtone);

    /**
     * @see #setStopPreviousRingtone(boolean)
     */
    CARAPI GetStopPreviousRingtone(
        /* [out] */ Boolean* result);

    /**
     * Stops playing the last {@link Ringtone} retrieved from this.
     */
    CARAPI StopPreviousRingtone();

    /**
     * Returns whether DRM ringtones will be included.
     *
     * @return Whether DRM ringtones will be included.
     * @see #setIncludeDrm(Boolean)
     * Obsolete - always returns false
     * @deprecated DRM ringtones are no longer supported
     */
    //@Deprecated
    CARAPI GetIncludeDrm(
        /* [out] */ Boolean* result);

    /**
     * Sets whether to include DRM ringtones.
     *
     * @param includeDrm Whether to include DRM ringtones.
     * Obsolete - no longer has any effect
     * @deprecated DRM ringtones are no longer supported
     */
    //@Deprecated
    CARAPI SetIncludeDrm(
        /* [in] */ Boolean includeDrm);

    /**
     * Returns a {@link Cursor} of all the ringtones available. The returned
     * cursor will be the same cursor returned each time this method is called,
     * so do not {@link Cursor#close()} the cursor. The cursor can be
     * {@link Cursor#deactivate()} safely.
     * <p>
     * If {@link RingtoneManager#RingtoneManager(Activity)} was not used, the
     * caller should manage the returned cursor through its activity's life
     * cycle to prevent leaking the cursor.
     *
     * @return A {@link Cursor} of all the ringtones available.
     * @see #ID_COLUMN_INDEX
     * @see #TITLE_COLUMN_INDEX
     * @see #URI_COLUMN_INDEX
     */
    CARAPI GetCursor(
        /* [out] */ ICursor** result);

    /**
     * Gets a {@link Ringtone} for the ringtone at the given position in the
     * {@link Cursor}.
     *
     * @param position The position (in the {@link Cursor}) of the ringtone.
     * @return A {@link Ringtone} pointing to the ringtone.
     */
    CARAPI GetRingtone(
        /* [in] */ Int32 position,
        /* [out] */ IRingtone** result);

    /**
     * Gets a {@link Uri} for the ringtone at the given position in the {@link Cursor}.
     *
     * @param position The position (in the {@link Cursor}) of the ringtone.
     * @return A {@link Uri} pointing to the ringtone.
     */
    CARAPI GetRingtoneUri(
        /* [in] */ Int32 position,
        /* [out] */ IUri** result);

    /**
     * Gets the position of a {@link Uri} within this {@link RingtoneManager}.
     *
     * @param ringtoneUri The {@link Uri} to retreive the position of.
     * @return The position of the {@link Uri}, or -1 if it cannot be found.
     */
    CARAPI GetRingtonePosition(
        /* [in] */ IUri* ringtoneUri,
        /* [out] */ Int32* result);

    /**
     * Returns a valid ringtone URI. No guarantees on which it returns. If it
     * cannot find one, returns null.
     *
     * @param context The context to use for querying.
     * @return A ringtone URI, or null if one cannot be found.
     */
    static CARAPI GetValidRingtoneUri(
        /* [in] */ IContext* context,
        /* [out] */ IUri** result);

    /**
     * Returns a {@link Ringtone} for a given sound URI.
     * <p>
     * If the given URI cannot be opened for any reason, this method will
     * attempt to fallback on another sound. If it cannot find any, it will
     * return null.
     *
     * @param context A context used to query.
     * @param ringtoneUri The {@link Uri} of a sound or ringtone.
     * @return A {@link Ringtone} for the given URI, or null.
     */
    static CARAPI GetRingtone(
        /* [in] */ IContext* context,
        /* [in] */ IUri* ringtoneUri,
        /* [out] */ IRingtone** result);

    /**
     * Gets the current default sound's {@link Uri}. This will give the actual
     * sound {@link Uri}, instead of using this, most clients can use
     * {@link System#DEFAULT_RINGTONE_URI}.
     *
     * @param context A context used for querying.
     * @param type The type whose default sound should be returned. One of
     *            {@link #TYPE_RINGTONE}, {@link #TYPE_NOTIFICATION}, or
     *            {@link #TYPE_ALARM}.
     * @return A {@link Uri} pointing to the default sound for the sound type.
     * @see #setActualDefaultRingtoneUri(Context, int, Uri)
     */
    static CARAPI GetActualDefaultRingtoneUri(
        /* [in] */ IContext* context,
        /* [in] */ Int32 type,
        /* [out] */ IUri** result);

    /**
     * Sets the {@link Uri} of the default sound for a given sound type.
     *
     * @param context A context used for querying.
     * @param type The type whose default sound should be set. One of
     *            {@link #TYPE_RINGTONE}, {@link #TYPE_NOTIFICATION}, or
     *            {@link #TYPE_ALARM}.
     * @param ringtoneUri A {@link Uri} pointing to the default sound to set.
     * @see #getActualDefaultRingtoneUri(Context, int)
     */
    static CARAPI SetActualDefaultRingtoneUri(
        /* [in] */ IContext* context,
        /* [in] */ Int32 type,
        /* [in] */ IUri* ringtoneUri);

    /**
     * Returns whether the given {@link Uri} is one of the default ringtones.
     *
     * @param ringtoneUri The ringtone {@link Uri} to be checked.
     * @return Whether the {@link Uri} is a default.
     */
    static CARAPI IsDefault(
        /* [in] */ IUri* ringtoneUri,
        /* [out] */ Boolean* result);

    /**
     * Returns the type of a default {@link Uri}.
     *
     * @param defaultRingtoneUri The default {@link Uri}. For example,
     *            {@link System#DEFAULT_RINGTONE_URI},
     *            {@link System#DEFAULT_NOTIFICATION_URI}, or
     *            {@link System#DEFAULT_ALARM_ALERT_URI}.
     * @return The type of the defaultRingtoneUri, or -1.
     */
    static CARAPI GetDefaultType(
        /* [in] */ IUri* defaultRingtoneUri,
        /* [out] */ Int32* result);

    /**
     * Returns the {@link Uri} for the default ringtone of a particular type.
     * Rather than returning the actual ringtone's sound {@link Uri}, this will
     * return the symbolic {@link Uri} which will resolved to the actual sound
     * when played.
     *
     * @param type The ringtone type whose default should be returned.
     * @return The {@link Uri} of the default ringtone for the given type.
     */
    static CARAPI GetDefaultUri(
        /* [in] */ Int32 type,
        /* [out] */ IUri** result);

    /**
     * Returns the {@link Uri} for the static default ringtone.
     * Rather than returning the actual ringtone's sound {@link Uri}, this will
     * return the default system ringtone. When actual ringtone is not valid
     * in media provider, default system ringtone is the one to rollback to.
     *
     * @return The {@link Uri} of the default system ringtone.
     * @hide
     */
    static CARAPI GetStaticDefaultRingtoneUri(
        /* [in] */ IContext* context,
        /* [out] */ IUri** result);

    /**
     * Returns the subscription ID of {@link Uri}.
     *
     * @param defaultRingtoneUri The default {@link Uri}. For example,
     *            {@link System#DEFAULT_RINGTONE_URI},
     *            {@link System#DEFAULT_RINGTONE_URI_2}, or
     *            {@link System#DEFAULT_RINGTONE_URI_3}.
     * @return The Subscription ID of the defaultRingtoneUri, or -1.
     * @hide
     */
    static CARAPI GetDefaultRingtoneSubIdByUri(
        /* [in] */ IUri* defaultRingtoneUri,
        /* [out] */ Int32* result);

    /**
     * Gets the actual default sound's {@link Uri}. This will give the actual
     * sound {@link Uri}, instead of using this, most clients can use
     * {@link System#DEFAULT_RINGTONE_URI}.
     *
     * @param subId The Subscription ID.
     * @return A {@link Uri} pointing to the default sound for the sound type.
     * @hide
     */
    static CARAPI GetDefaultRingtoneUriBySubId(
        /* [in] */ Int32 subId,
        /* [out] */ IUri** result);

    /**
     * Gets the current default sound's {@link Uri}. This will give the actual
     * sound {@link Uri}, instead of using this, most clients can use
     * {@link System#DEFAULT_RINGTONE_URI}.
     *
     * @param context A context used for querying.
     * @param subId The Subscription ID.
     * @return A {@link Uri} pointing to the default sound for the sound type.
     * @hide
     */
    static CARAPI GetActualRingtoneUriBySubId(
        /* [in] */ IContext* context,
        /* [in] */ Int32 subId,
        /* [out] */ IUri** result);

    /**
     * Sets the {@link Uri} of the default sound for a given sound type.
     *
     * @param context A context used for querying.
     * @param subId The Subscription ID.
     * @param ringtoneUri A {@link Uri} pointing to the default sound to set.
     * @hide
     */
    static CARAPI SetActualRingtoneUriBySubId(
        /* [in] */ IContext* context,
        /* [in] */ Int32 subId,
        /* [in] */ IUri* ringtoneUri);

private:
    static CARAPI_(AutoPtr<IUri>) GetUriFromCursor(
        /* [in] */ ICursor* cursor);

    static CARAPI_(AutoPtr<IUri>) GetValidRingtoneUriFromCursorAndClose(
        /* [in] */ IContext* context,
        /* [in] */ ICursor* cursor);

    CARAPI_(AutoPtr<ICursor>) GetInternalRingtones();

    CARAPI_(AutoPtr<ICursor>) GetMediaRingtones();

    CARAPI_(void) SetFilterColumnsList(
        /* [in] */ Int32 type);

    /**
     * Constructs a where clause that consists of at least one column being 1
     * (TRUE). This is used to find all matching sounds for the given sound
     * types (ringtone, notifications, etc.)
     *
     * @param columns The columns that must be TRUE.
     * @return The where clause.
     */
    static CARAPI_(String) ConstructBooleanTrueWhereClause(
        /* [in] */ List<String>& columns);

    CARAPI_(AutoPtr<ICursor>) Query(
        /* [in] */ IUri* uri,
        /* [in] */ ArrayOf<String>* projection,
        /* [in] */ const String& selection,
        /* [in] */ ArrayOf<String>* selectionArgs,
        /* [in] */ const String& sortOrder);

    /**
     * Returns a {@link Ringtone} for a given sound URI on the given stream
     * type. Normally, if you change the stream type on the returned
     * {@link Ringtone}, it will re-create the {@link MediaPlayer}. This is just
     * an optimized route to avoid that.
     *
     * @param streamType The stream type for the ringtone, or -1 if it should
     *            not be set (and the default used instead).
     * @see #getRingtone(Context, Uri)
     */
    static CARAPI_(AutoPtr<IRingtone>) GetRingtone(
        /* [in] */ IContext* context,
        /* [in] */ IUri* ringtoneUri,
        /* [in] */ Int32 streamType);

    static CARAPI_(String) GetSettingForType(
        /* [in] */ Int32 type);

private:
    static const String TAG; // = "RingtoneManager";

    // Make sure the column ordering and then ..._COLUMN_INDEX are in sync
    static AutoPtr< ArrayOf<String> > INTERNAL_COLUMNS;
        /* = new String[] {
            MediaStore.Audio.Media._ID,
            MediaStore.Audio.Media.TITLE,
            "\"" + MediaStore.Audio.Media.INTERNAL_CONTENT_URI + "\"",
            MediaStore.Audio.Media.TITLE_KEY
        };*/

    static AutoPtr< ArrayOf<String> > MEDIA_COLUMNS;
        /* = new String[] {
            MediaStore.Audio.Media._ID,
            MediaStore.Audio.Media.TITLE,
            "\"" + MediaStore.Audio.Media.EXTERNAL_CONTENT_URI + "\"",
            MediaStore.Audio.Media.TITLE_KEY
        };*/

    AutoPtr<IActivity> mActivity;

    AutoPtr<IContext> mContext;

    AutoPtr<ICursor> mCursor;

    Int32 mType; // = TYPE_RINGTONE;

    /**
     * If a column (item from this list) exists in the Cursor, its value must
     * be TRUE (value of 1) for the row to be returned.
     */
    List<String> mFilterColumns; // = new ArrayList<String>();

    Boolean mStopPreviousRingtone; // = TRUE;

    AutoPtr<IRingtone> mPreviousRingtone;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CRINGTONEMANAGER_H__

