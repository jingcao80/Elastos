
#ifndef __ELASTOS_DROID_MEDIA_CRINGTONEMANAGERHELPER_H__
#define __ELASTOS_DROID_MEDIA_CRINGTONEMANAGERHELPER_H__

#include "_Elastos_Droid_Media_CRingtoneManagerHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Media.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CRingtoneManagerHelper)
    , public Singleton
    , public IRingtoneManagerHelper
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Returns a valid ringtone URI. No guarantees on which it returns. If it
     * cannot find one, returns null.
     *
     * @param context The context to use for querying.
     * @return A ringtone URI, or null if one cannot be found.
     */
    CARAPI GetValidRingtoneUri(
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
    CARAPI GetRingtone(
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
    CARAPI GetActualDefaultRingtoneUri(
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
    CARAPI SetActualDefaultRingtoneUri(
        /* [in] */ IContext* context,
        /* [in] */ Int32 type,
        /* [in] */ IUri* ringtoneUri);

    /**
     * Returns whether the given {@link Uri} is one of the default ringtones.
     *
     * @param ringtoneUri The ringtone {@link Uri} to be checked.
     * @return Whether the {@link Uri} is a default.
     */
    CARAPI IsDefault(
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
    CARAPI GetDefaultType(
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
    CARAPI GetDefaultUri(
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
    CARAPI GetStaticDefaultRingtoneUri(
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
    CARAPI GetDefaultRingtoneSubIdByUri(
        /* [in] */ IUri* defaultRingtoneUri,
        /* [out] */ Int32* result);

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
    CARAPI GetActualRingtoneUriBySubId(
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
    CARAPI SetActualRingtoneUriBySubId(
        /* [in] */ IContext* context,
        /* [in] */ Int32 subId,
        /* [in] */ IUri* ringtoneUri);

    /**
     * Gets the actual default sound's {@link Uri}. This will give the actual
     * sound {@link Uri}, instead of using this, most clients can use
     * {@link System#DEFAULT_RINGTONE_URI}.
     *
     * @param subId The Subscription ID.
     * @return A {@link Uri} pointing to the default sound for the sound type.
     * @hide
     */
    CARAPI GetDefaultRingtoneUriBySubId(
        /* [in] */ Int32 subId,
        /* [out] */ IUri** result);
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CRINGTONEMANAGERHELPER_H__
