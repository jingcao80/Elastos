
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFO_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_CCALLERINFO_H__

#include "_Elastos_Droid_Internal_Telephony_CCallerInfo.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Net::IUri;
//using Elastos::Droid::Internal::Telephony::ICallerInfo;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

CarClass(CCallerInfo)
{
public:
    CARAPI constructor();

    CARAPI IsEmergencyNumber(
        /* [out] */ Boolean* result);

    CARAPI IsVoiceMailNumber(
        /* [out] */ Boolean* result);

    CARAPI UpdateGeoDescription(
        /* [in] */ IContext* context,
        /* [in] */ const String& fallbackNumber);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI MarkAsEmergency(
        /* [in] */ IContext* context,
        /* [out] */ ICallerInfo** callerInfo);

    /**
     * Mark this CallerInfo as a voicemail call. The voicemail label
     * is obtained from the telephony manager. Caller must hold the
     * READ_PHONE_STATE permission otherwise the phoneNumber will be
     * set to null.
     * @return this instance.
     */
    // TODO: As in the emergency number handling, we end up writing a
    // string in the phone number field.
    CARAPI MarkAsVoiceMail(
        /* [out] */ ICallerInfo** callerInfo);

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
        /* [in] */ IContext* context,
        /* [in] */ const String& number,
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

    CARAPI GetContactExists(
        /* [out] */ Boolean* contactExists);

    CARAPI SetContactExists(
        /* [in] */ Boolean contactExists);

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

    CARAPI SetPerson_id(
        /* [in] */ Int64 person_id);

    CARAPI GetPerson_id(
        /* [out] */ Int64* person_id);

    CARAPI GetNeedUpdate(
        /* [out] */ Boolean* needUpdate);

    CARAPI SetNeedUpdate(
        /* [in] */ Boolean needUpdate);

    CARAPI SetContactRefUri(
        /* [in] */ IUri* contactRefUri);

    CARAPI GetContactRefUri(
        /* [out] */ IUri** contactRefUri);

    CARAPI SetContactRingtoneUri(
        /* [in] */ IUri* contactRingtoneUri);

    CARAPI GetContactRingtoneUri(
        /* [out] */ IUri** contactRingtoneUri);

    CARAPI GetShouldSendToVoicemail(
        /* [out] */ Boolean* shouldSendToVoicemail);

    CARAPI SetShouldSendToVoicemail(
        /* [in] */ Boolean shouldSendToVoicemail);

    CARAPI SetCachedPhoto(
        /* [in] */ IDrawable* cachedPhoto);

    CARAPI GetCachedPhoto(
        /* [out] */ IDrawable** cachedPhoto);

    CARAPI SetCachedPhotoIcon(
        /* [in] */ IBitmap* cachedPhotoIcon);

    CARAPI GetCachedPhotoIcon(
        /* [out] */ IBitmap** cachedPhotoIcon);

    CARAPI GetIsCachedPhotoCurrent(
        /* [out] */ Boolean* isCachedPhotoCurrent);

    CARAPI SetIsCachedPhotoCurrent(
        /* [in] */ Boolean isCachedPhotoCurrent);

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
    Int64 mPerson_id;
    Boolean mNeedUpdate;
    AutoPtr<IUri> mContactRefUri;

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
