
#include "Elastos.CoreLibrary.External.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/app/CProfileGroup.h"
#include "elastos/droid/media/CRingtoneManager.h"
#include "elastos/droid/net/Uri.h"
#include "elastos/droid/os/CParcelUuid.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Media::CRingtoneManager;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::IParcelUuid;
using Elastos::Droid::Os::CParcelUuid;
using Elastos::Droid::Text::TextUtils;
using Elastos::IO::ICloseable;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

const String CProfileGroup::TAG("CProfileGroup");

CAR_INTERFACE_IMPL_2(CProfileGroup, Object, IProfileGroup, IParcelable)

CAR_OBJECT_IMPL(CProfileGroup)

CProfileGroup::CProfileGroup()
    : mNameResId(0)
    , mSoundMode(ProfileGroupMode_DEFAULT)
    , mRingerMode(ProfileGroupMode_DEFAULT)
    , mVibrateMode(ProfileGroupMode_DEFAULT)
    , mLightsMode(ProfileGroupMode_DEFAULT)
    , mDefaultGroup(FALSE)
    , mDirty(FALSE)
{
    CRingtoneManager::GetDefaultUri(IRingtoneManager::TYPE_NOTIFICATION, (IUri**)&mSoundOverride);
    CRingtoneManager::GetDefaultUri(IRingtoneManager::TYPE_RINGTONE, (IUri**)&mRingerOverride);
}

ECode CProfileGroup::constructor()
{
    return NOERROR;
}

ECode CProfileGroup::constructor(
    /* [in] */ IUUID* uuid,
    /* [in] */ Boolean defaultGroup)
{
    return constructor(String(NULL), uuid, defaultGroup);
}

ECode CProfileGroup::constructor(
    /* [in] */ const String& name,
    /* [in] */ IUUID* uuid,
    /* [in] */ Boolean defaultGroup)
{
    mName = name;
    mUuid = uuid;
    if (mUuid == NULL) {
        AutoPtr<IUUIDHelper> helper;
        CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
        helper->RandomUUID((IUUID**)&mUuid);
    }
    mDefaultGroup = defaultGroup;
    mDirty = uuid == NULL;
    return NOERROR;
}

ECode CProfileGroup::Matches(
    /* [in] */ INotificationGroup* group,
    /* [in] */ Boolean defaultGroup,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoPtr<IUUID> uuid;
    group->GetUuid((IUUID**)&uuid);
    Boolean isEqual;
    if (mUuid->Equals(uuid.Get(), &isEqual), isEqual) {
        *result = TRUE;
        return NOERROR;
    }

    /* fallback matches for backwards compatibility */
    Boolean matches = FALSE;

    /* fallback attempt 1: match name */
    String name;
    if (!mName.IsNull() && (group->GetName(&name), mName.Equals(name))) {
        matches = TRUE;
    /* fallback attempt 2: match for the 'defaultGroup' flag to match the wildcard group */
    }
    else if (mDefaultGroup && defaultGroup) {
        matches = TRUE;
    }

    if (!matches) {
        *result = FALSE;
        return NOERROR;
    }

    mName = NULL;
    mUuid = NULL;
    group->GetUuid((IUUID**)&mUuid);
    mDirty = TRUE;

    *result = TRUE;
    return NOERROR;
}

ECode CProfileGroup::GetUuid(
    /* [out] */ IUUID** uuid)
{
    VALIDATE_NOT_NULL(uuid);
    *uuid = mUuid;
    REFCOUNT_ADD(*uuid)
    return NOERROR;
}

ECode CProfileGroup::IsDefaultGroup(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDefaultGroup;
    return NOERROR;
}

/** @hide */
ECode CProfileGroup::IsDirty(
    /* [out] */ Boolean* dirty)
{
    VALIDATE_NOT_NULL(dirty);
    *dirty = mDirty;
    return NOERROR;
}

/** @hide */
ECode CProfileGroup::SetSoundOverride(
    /* [in] */ IUri* sound)
{
    mSoundOverride = sound;
    mDirty = TRUE;
    return NOERROR;
}

ECode CProfileGroup::GetSoundOverride(
    /* [out] */ IUri** sound)
{
    VALIDATE_NOT_NULL(sound);
    *sound = mSoundOverride;
    REFCOUNT_ADD(*sound);
    return NOERROR;
}

/** @hide */
ECode CProfileGroup::SetRingerOverride(
    /* [in] */ IUri* ringer)
{
    mRingerOverride = ringer;
    mDirty = TRUE;
    return NOERROR;
}

ECode CProfileGroup::GetRingerOverride(
    /* [out] */ IUri** ringer)
{
    VALIDATE_NOT_NULL(ringer);
    *ringer = mRingerOverride;
    REFCOUNT_ADD(*ringer);
    return NOERROR;
}

/** @hide */
ECode CProfileGroup::SetSoundMode(
    /* [in] */ ProfileGroupMode soundMode)
{
    mSoundMode = soundMode;
    mDirty = TRUE;
    return NOERROR;
}

ECode CProfileGroup::GetSoundMode(
    /* [out] */ ProfileGroupMode* soundMode)
{
    VALIDATE_NOT_NULL(soundMode);
    *soundMode = mSoundMode;
    return NOERROR;
}

/** @hide */
ECode CProfileGroup::SetRingerMode(
    /* [in] */ ProfileGroupMode ringerMode)
{
    mRingerMode = ringerMode;
    mDirty = TRUE;
    return NOERROR;
}

ECode CProfileGroup::GetRingerMode(
    /* [out] */ ProfileGroupMode* ringerMode)
{
    VALIDATE_NOT_NULL(ringerMode);
    *ringerMode = mRingerMode;
    return NOERROR;
}

/** @hide */
ECode CProfileGroup::SetVibrateMode(
    /* [in] */ ProfileGroupMode vibrateMode)
{
    mVibrateMode = vibrateMode;
    mDirty = TRUE;
    return NOERROR;
}

ECode CProfileGroup::GetVibrateMode(
    /* [out] */ ProfileGroupMode* vibrateMode)
{
    VALIDATE_NOT_NULL(vibrateMode);
    *vibrateMode = mVibrateMode;
    return NOERROR;
}

/** @hide */
ECode CProfileGroup::SetLightsMode(
    /* [in] */ ProfileGroupMode lightsMode)
{
    mLightsMode = lightsMode;
    mDirty = TRUE;
    return NOERROR;
}

ECode CProfileGroup::GetLightsMode(
    /* [out] */ ProfileGroupMode* lightsMode)
{
    VALIDATE_NOT_NULL(lightsMode);
    *lightsMode = mLightsMode;
    return NOERROR;
}

ECode CProfileGroup::ApplyOverridesToNotification(
    /* [in] */ INotification* notification)
{
    switch (mSoundMode) {
        case ProfileGroupMode_OVERRIDE:
            notification->SetSound(mSoundOverride);
            break;
        case ProfileGroupMode_SUPPRESS: {
            Int32 defaults;
            notification->GetDefaults(&defaults);
            defaults &= ~INotification::DEFAULT_SOUND;
            notification->SetDefaults(defaults);
            notification->SetSound(NULL);
            break;
        }
        case ProfileGroupMode_DEFAULT:
            break;
    }
    switch (mVibrateMode) {
        case ProfileGroupMode_OVERRIDE: {
            Int32 defaults;
            notification->GetDefaults(&defaults);
            defaults |= INotification::DEFAULT_VIBRATE;
            notification->SetDefaults(defaults);
            break;
        }
        case ProfileGroupMode_SUPPRESS: {
            Int32 defaults;
            notification->GetDefaults(&defaults);
            defaults &= ~INotification::DEFAULT_VIBRATE;
            notification->SetDefaults(defaults);
            notification->SetVibrate(NULL);
            break;
        }
        case ProfileGroupMode_DEFAULT:
            break;
    }
    switch (mLightsMode) {
        case ProfileGroupMode_OVERRIDE: {
            Int32 defaults;
            notification->GetDefaults(&defaults);
            defaults |= INotification::DEFAULT_LIGHTS;
            notification->SetDefaults(defaults);
            break;
        }
        case ProfileGroupMode_SUPPRESS: {
            Int32 defaults;
            notification->GetDefaults(&defaults);
            defaults &= ~INotification::DEFAULT_VIBRATE;
            notification->SetDefaults(defaults);
            Int32 flags;
            notification->GetFlags(&flags);
            flags &= ~INotification::FLAG_SHOW_LIGHTS;
            notification->SetFlags(flags);
            break;
        }
        case ProfileGroupMode_DEFAULT:
            break;
    }
    return NOERROR;
}

Boolean CProfileGroup::ValidateOverrideUri(
    /* [in] */ IContext* context,
    /* [in] */ IUri* uri)
{
    Boolean result;
    if (CRingtoneManager::IsDefault(uri, &result), result) {
        return true;
    }
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    AutoPtr<ICursor> cursor;
    resolver->Query(uri, NULL, String(NULL), NULL, String(NULL), (ICursor**)&cursor);
    Boolean valid = FALSE;

    if (cursor != NULL) {
        cursor->MoveToFirst(&valid);
        ICloseable::Probe(cursor)->Close();
    }
    return valid;
}

void CProfileGroup::ValidateOverrideUris(
    /* [in] */ IContext* context)
{
    if (!ValidateOverrideUri(context, mSoundOverride)) {
        mSoundOverride = NULL;
        CRingtoneManager::GetDefaultUri(IRingtoneManager::TYPE_NOTIFICATION, (IUri**)&mSoundOverride);
        mSoundMode = ProfileGroupMode_DEFAULT;
        mDirty = TRUE;
    }
    if (!ValidateOverrideUri(context, mRingerOverride)) {
        mRingerOverride = NULL;
        CRingtoneManager::GetDefaultUri(IRingtoneManager::TYPE_RINGTONE, (IUri**)&mRingerOverride);
        mRingerMode = ProfileGroupMode_DEFAULT;
        mDirty = TRUE;
    }
}

ECode CProfileGroup::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mName);
    AutoPtr<IParcelUuid> parcelUuid;
    CParcelUuid::New(mUuid, (IParcelUuid**)&parcelUuid);
    dest->WriteInterfacePtr(parcelUuid);
    dest->WriteBoolean(mDefaultGroup);
    dest->WriteBoolean(mDirty);
    dest->WriteInterfacePtr(mSoundOverride);
    dest->WriteInterfacePtr(mRingerOverride);

    dest->WriteInt32(mSoundMode);
    dest->WriteInt32(mRingerMode);
    dest->WriteInt32(mVibrateMode);
    dest->WriteInt32(mLightsMode);
    return NOERROR;
}

ECode CProfileGroup::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mName);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    IParcelUuid::Probe(obj)->GetUuid((IUUID**)&mUuid);
    source->ReadBoolean(&mDefaultGroup);
    source->ReadBoolean(&mDirty);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mSoundOverride = IUri::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mRingerOverride = IUri::Probe(obj);

    source->ReadInt32(&mSoundMode);
    source->ReadInt32(&mRingerMode);
    source->ReadInt32(&mVibrateMode);
    source->ReadInt32(&mLightsMode);
    return NOERROR;
}

ECode CProfileGroup::GetXmlString(
    /* [in] */ IStringBuilder* builder,
    /* [in] */ IContext* context)
{
    builder->Append(String("<profileGroup uuid=\""));
    String str;
    mUuid->ToString(&str);
    builder->Append(TextUtils::HtmlEncode(str));
    if (!mName.IsNull()) {
        builder->Append(String("\" name=\""));
        builder->Append(mName);
    }
    builder->Append(String("\" default=\""));
    Boolean result;
    IsDefaultGroup(&result);
    builder->Append(result);
    builder->Append(String("\">\n<sound>"));
    IObject::Probe(mSoundOverride)->ToString(&str);
    builder->Append(TextUtils::HtmlEncode(str));
    builder->Append(String("</sound>\n<ringer>"));
    IObject::Probe(mRingerOverride)->ToString(&str);
    builder->Append(TextUtils::HtmlEncode(str));
    builder->Append(String("</ringer>\n<soundMode>"));
    builder->Append(mSoundMode);
    builder->Append(String("</soundMode>\n<ringerMode>"));
    builder->Append(mRingerMode);
    builder->Append(String("</ringerMode>\n<vibrateMode>"));
    builder->Append(mVibrateMode);
    builder->Append(String("</vibrateMode>\n<lightsMode>"));
    builder->Append(mLightsMode);
    builder->Append(String("</lightsMode>\n</profileGroup>\n"));
    mDirty = FALSE;
    return NOERROR;
}

static ProfileGroupMode ValueOf(const String& modeStr)
{
    if (modeStr.Equals("SUPPRESS")) return ProfileGroupMode_SUPPRESS;
    else if (modeStr.Equals("DEFAULT")) return ProfileGroupMode_DEFAULT;
    else if (modeStr.Equals("OVERRIDE")) return ProfileGroupMode_OVERRIDE;
    else return ProfileGroupMode_DEFAULT;
}

ECode CProfileGroup::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IProfileGroup** group)
{
    VALIDATE_NOT_NULL(group);

    String name;
    xpp->GetAttributeValue(String(NULL), String("name"), &name);
    AutoPtr<IUUID> uuid;
    String value;
    xpp->GetAttributeValue(String(NULL), String("uuid"), &value);

    if (!value.IsNull()) {
        // try {
        AutoPtr<IUUIDHelper> helper;
        CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
        ECode ec = helper->FromString(value, (IUUID**)&uuid);
        if (FAILED(ec)) {
            Logger::W(TAG, "UUID not recognized for %s, using new one.", name.string());
        }
        // } catch (IllegalArgumentException e) {
        //     Log.w(TAG, "UUID not recognized for " + name + ", using new one.");
        // }
    }

    xpp->GetAttributeValue(String(NULL), String("default"), &value);
    Boolean defaultGroup = TextUtils::Equals(value, String("true"));

    AutoPtr<CProfileGroup> profileGroup;
    CProfileGroup::NewByFriend(name, uuid, defaultGroup, (CProfileGroup**)&profileGroup);
    Int32 event;
    xpp->Next(&event);
    while (event != IXmlPullParser::END_TAG || (xpp->GetName(&name), !name.Equals("profileGroup"))) {
        if (event == IXmlPullParser::START_TAG) {
            xpp->GetName(&name);
            if (name.Equals("sound")) {
                String next;
                xpp->NextText(&next);
                AutoPtr<IUri> uri;
                Uri::Parse(next, (IUri**)&uri);
                profileGroup->SetSoundOverride(uri);
            }
            else if (name.Equals("ringer")) {
                String next;
                xpp->NextText(&next);
                AutoPtr<IUri> uri;
                Uri::Parse(next, (IUri**)&uri);
                profileGroup->SetRingerOverride(uri);
            }
            else if (name.Equals("soundMode")) {
                String next;
                xpp->NextText(&next);
                profileGroup->SetSoundMode(ValueOf(next));
            }
            else if (name.Equals("ringerMode")) {
                String next;
                xpp->NextText(&next);
                profileGroup->SetRingerMode(ValueOf(next));
            }
            else if (name.Equals("vibrateMode")) {
                String next;
                xpp->NextText(&next);
                profileGroup->SetVibrateMode(ValueOf(next));
            }
            else if (name.Equals("lightsMode")) {
                String next;
                xpp->NextText(&next);
                profileGroup->SetLightsMode(ValueOf(next));
            }
        }
        xpp->Next(&event);
    }

    /* we just loaded from XML, no need to save */
    profileGroup->mDirty = FALSE;

    *group = profileGroup;
    REFCOUNT_ADD(*group);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
