
#include <Elastos.CoreLibrary.Text.h>
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/CNotification.h"
#include "elastos/droid/app/CPendingIntentHelper.h"
#include "elastos/droid/app/CPendingIntent.h"
#include "elastos/droid/app/CNotificationAction.h"
#include "elastos/droid/app/CNotificationBuilder.h"
#include "elastos/droid/app/CNotificationBuilderRemoteViews.h"
#include "elastos/droid/os/UserHandle.h"
#include "elastos/droid/os/CParcel.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/media/CAudioAttributes.h"
#include "elastos/droid/media/CAudioAttributesBuilder.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/widget/CRemoteViews.h"
#include "elastos/droid/R.h"
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CParcel;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Media::CAudioAttributes;
using Elastos::Droid::Media::CAudioAttributesBuilder;
using Elastos::Droid::Media::IAudioAttributesBuilder;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Widget::CRemoteViews;
using Elastos::Core::ISystem;
using Elastos::Core::CSystem;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Text::INumberFormatHelper;
using Elastos::Text::CNumberFormatHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

//==========================================================================
// CNotification::BuilderRemoteViews
//==========================================================================
CAR_INTERFACE_IMPL(CNotification::BuilderRemoteViews, RemoteViews, INotificationBuilderRemoteViews)

CNotification::BuilderRemoteViews::BuilderRemoteViews()
{}

ECode CNotification::BuilderRemoteViews::constructor()
{
    return RemoteViews::constructor();
}

ECode CNotification::BuilderRemoteViews::constructor(
    /* [in] */ IApplicationInfo* appInfo,
    /* [in] */ Int32 layoutId)
{
    return RemoteViews::constructor(appInfo, layoutId);
}

// @Override
ECode CNotification::BuilderRemoteViews::Clone(
    /* [out] */ IRemoteViews** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<IInterface> obj;
    Clone((IInterface**)&obj);
    *result = IRemoteViews::Probe(obj);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CNotification::BuilderRemoteViews::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)

    AutoPtr<IParcel> parcel;
    CParcel::New((IParcel**)&parcel);
    WriteToParcel(parcel);
    parcel->SetDataPosition(0);

    AutoPtr<INotificationBuilderRemoteViews> brv;
    CNotificationBuilderRemoteViews::New((INotificationBuilderRemoteViews**)&brv);
    IParcelable::Probe(brv)->ReadFromParcel(parcel);
    *obj = brv.Get();
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

//==========================================================================
// CNotification
//==========================================================================

static AutoPtr<IAudioAttributes> InitAUDIO_ATTRIBUTES_DEFAULT()
{
    AutoPtr<IAudioAttributes> attrs;
    AutoPtr<IAudioAttributesBuilder> builder;
    CAudioAttributesBuilder::NewByFriend((CAudioAttributesBuilder**)&builder);

    builder->SetContentType(IAudioAttributes::CONTENT_TYPE_SONIFICATION);
    builder->SetUsage(IAudioAttributes::USAGE_NOTIFICATION);
    builder->Build((IAudioAttributes**)&attrs);

    return attrs;
}

const String CNotification::TAG("Notification");

const Int32 CNotification::MAX_CHARSEQUENCE_LENGTH = 5120;//5 * 1024;

const AutoPtr<IAudioAttributes> CNotification::AUDIO_ATTRIBUTES_DEFAULT = InitAUDIO_ATTRIBUTES_DEFAULT();

CAR_INTERFACE_IMPL_2(CNotification, Object, INotification, IParcelable)

CAR_OBJECT_IMPL(CNotification)

CNotification::CNotification()
    : mWhen(0)
    , mIcon(0)
    , mIconLevel(0)
    , mNumber(0)
    , mAudioStreamType(INotification::STREAM_DEFAULT)
    , mLedARGB(0)
    , mLedOnMS(0)
    , mLedOffMS(0)
    , mDefaults(0)
    , mFlags(0)
    , mPriority(0)
{
    mAudioAttributes = AUDIO_ATTRIBUTES_DEFAULT;
    CBundle::New((IBundle**)&mExtras);
}

CNotification::~CNotification()
{}

ECode CNotification::constructor()
{
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mWhen);
    mPriority = INotification::PRIORITY_DEFAULT;

    return NOERROR;
}

ECode CNotification::constructor(
    /* [in] */ IContext* context,
    /* [in] */ Int32 icon,
    /* [in] */ ICharSequence* tickerText,
    /* [in] */ Int64 when,
    /* [in] */ ICharSequence* contentTitle,
    /* [in] */ ICharSequence* contentText,
    /* [in] */ IIntent* contentIntent)
{
    mWhen = when;
    mIcon = icon;
    mTickerText = tickerText;

    AutoPtr<IPendingIntentHelper> helper;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    AutoPtr<IPendingIntent> pendingIntent;
    helper->GetActivity(context, 0, contentIntent, 0, (IPendingIntent**)&pendingIntent);
    return SetLatestEventInfo(context, contentTitle, contentText, pendingIntent);
}

ECode CNotification::constructor(
    /* [in] */ Int32 icon,
    /* [in] */ ICharSequence* tickerText,
    /* [in] */ Int64 when)
{
    mIcon = icon;
    mTickerText = tickerText;
    mWhen = when;

    return NOERROR;
}

ECode CNotification::GetWhen(
    /* [out] */ Int64* when)
{
    VALIDATE_NOT_NULL(when);
    *when = mWhen;
    return NOERROR;
}

ECode CNotification::SetWhen(
    /* [in] */ Int64 when)
{
    mWhen = when;
    return NOERROR;
}

ECode CNotification::GetIcon(
    /* [out] */ Int32* icon)
{
    VALIDATE_NOT_NULL(icon);
    *icon = mIcon;
    return NOERROR;
}

ECode CNotification::SetIcon(
    /* [in] */ Int32 icon)
{
    mIcon = icon;
    return NOERROR;
}

ECode CNotification::GetIconLevel(
    /* [out] */ Int32 * iconLevel)
{
    VALIDATE_NOT_NULL(iconLevel);
    *iconLevel = mIconLevel;
    return NOERROR;
}

ECode CNotification::SetIconLevel(
    /* [in] */ Int32 iconLevel)
{
    mIconLevel = iconLevel;
    return NOERROR;
}

ECode CNotification::GetNumber(
    /* [out] */ Int32 * number)
{
    VALIDATE_NOT_NULL(number);
    *number = mNumber;
    return NOERROR;
}

ECode CNotification::SetNumber(
    /* [in] */ Int32 number)
{
    mNumber = number;
    return NOERROR;
}

ECode CNotification::GetContentIntent(
    /* [out] */ IPendingIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    *intent = mContentIntent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CNotification::SetContentIntent(
    /* [in] */ IPendingIntent* intent)
{
    mContentIntent = intent;
    return NOERROR;
}

ECode CNotification::GetDeleteIntent(
    /* [out] */ IPendingIntent** intent)
{
    VALIDATE_NOT_NULL(intent);
    *intent = mDeleteIntent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CNotification::SetDeleteIntent(
    /* [in] */ IPendingIntent* intent)
{
    mDeleteIntent = intent;
    return NOERROR;
}

ECode CNotification::GetFullScreenIntent(
    /* [out] */ IPendingIntent **intent)
{
    VALIDATE_NOT_NULL(intent);
    *intent = mFullScreenIntent;
    REFCOUNT_ADD(*intent);
    return NOERROR;
}

ECode CNotification::SetFullScreenIntent(
    /* [in] */ IPendingIntent * intent)
{
    mFullScreenIntent = intent;
    return NOERROR;
}

ECode CNotification::GetTickerText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = mTickerText;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode CNotification::SetTickerText(
    /* [in] */ ICharSequence* text)
{
    mTickerText = text;
    return NOERROR;
}

ECode CNotification::GetTickerView(
    /* [out] */ IRemoteViews **view)
{
    VALIDATE_NOT_NULL(view);
    *view = mTickerView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNotification::SetTickerView(
    /* [in] */ IRemoteViews * view)
{
    mTickerView = view;
    return NOERROR;
}

ECode CNotification::GetContentView(
    /* [out] */ IRemoteViews** view)
{
    VALIDATE_NOT_NULL(view);
    *view = mContentView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNotification::SetContentView(
    /* [in] */ IRemoteViews* view)
{
    mContentView = view;
    return NOERROR;
}

ECode CNotification::GetBigContentView(
    /* [out] */ IRemoteViews **view)
{
    VALIDATE_NOT_NULL(view);
    *view = mBigContentView;
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CNotification::SetBigContentView(
    /* [in] */ IRemoteViews * view)
{
    mBigContentView = view;
    return NOERROR;
}

ECode CNotification::GetLargeIcon(
    /* [out] */ IBitmap **bitmap)
{
    VALIDATE_NOT_NULL(bitmap);
    *bitmap = mLargeIcon;
    REFCOUNT_ADD(*bitmap);
    return NOERROR;
}

ECode CNotification::SetLargeIcon(
    /* [in] */ IBitmap * bitmap)
{
    mLargeIcon = bitmap;
    return NOERROR;
}

ECode CNotification::GetSound(
    /* [out] */ IUri** sound)
{
    VALIDATE_NOT_NULL(sound);
    *sound = mSound;
    REFCOUNT_ADD(*sound)
    return NOERROR;
}

ECode CNotification::SetSound(
    /* [in] */ IUri* sound)
{
    mSound = sound;
    return NOERROR;
}

ECode CNotification::GetAudioStreamType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mAudioStreamType;
    return NOERROR;
}

ECode CNotification::SetAudioStreamType(
    /* [in] */ Int32 type)
{
    mAudioStreamType = type;
    return NOERROR;
}

ECode CNotification::GetAudioAttributes (
    /* [out] */ IAudioAttributes** attrs)
{
    VALIDATE_NOT_NULL(attrs)
    *attrs = mAudioAttributes;
    REFCOUNT_ADD(*attrs)
    return NOERROR;
}

ECode CNotification::SetAudioAttributes (
    /* [in] */ IAudioAttributes* attrs)
{
    mAudioAttributes = attrs;
    return NOERROR;
}

ECode CNotification::GetVibrate(
    /* [out, callee] */ ArrayOf<Int64>** vibrate)
{
    VALIDATE_NOT_NULL(vibrate);
    *vibrate = mVibrate;
    REFCOUNT_ADD(*vibrate);
    return NOERROR;
}

ECode CNotification::SetVibrate(
    /* [in] */ ArrayOf<Int64>* vibrate)
{
    mVibrate = vibrate;
    return NOERROR;
}

ECode CNotification::GetLedARGB(
    /* [out] */ Int32* argb)
{
    VALIDATE_NOT_NULL(argb);
    *argb = mLedARGB;
    return NOERROR;
}

ECode CNotification::SetLedARGB(
    /* [in] */ Int32 argb)
{
    mLedARGB = argb;
    return NOERROR;
}

ECode CNotification::GetLedOnMS(
    /* [out] */ Int32* onMS)
{
    VALIDATE_NOT_NULL(onMS);
    *onMS = mLedOnMS;
    return NOERROR;
}

ECode CNotification::SetLedOnMS(
    /* [in] */ Int32 onMS)
{
    mLedOnMS = onMS;
    return NOERROR;
}

ECode CNotification::GetLedOffMS(
    /* [out] */ Int32* offMS)
{
    VALIDATE_NOT_NULL(offMS);
    *offMS = mLedOffMS;
    return NOERROR;
}

ECode CNotification::SetLedOffMS(
    /* [in] */ Int32 offMS)
{
    mLedOffMS = offMS;
    return NOERROR;
}

ECode CNotification::GetDefaults(
    /* [out] */ Int32* defaults)
{
    VALIDATE_NOT_NULL(defaults);
    *defaults = mDefaults;
    return NOERROR;
}

ECode CNotification::SetDefaults(
    /* [in] */ Int32 defaults)
{
    mDefaults = defaults;
    return NOERROR;
}

ECode CNotification::GetFlags(
    /* [out] */ Int32* flags)
{
    VALIDATE_NOT_NULL(flags);
    *flags = mFlags;
    return NOERROR;
}

ECode CNotification::SetFlags(
    /* [in] */ Int32 flags)
{
    mFlags = flags;
    return NOERROR;
}

ECode CNotification::GetPriority(
    /* [out] */ Int32 * priority)
{
    VALIDATE_NOT_NULL(priority);
    *priority = mPriority;
    return NOERROR;
}

ECode CNotification::SetPriority(
    /* [in] */ Int32 priority)
{
    mPriority = priority;
    return NOERROR;
}

ECode CNotification::GetGroup(
    /* [out] */ String* group)
{
    VALIDATE_NOT_NULL(group)
    *group = mGroupKey;
    return NOERROR;
}

ECode CNotification::SetGroup(
    /* [in] */ const String& group)
{
    mGroupKey = group;
    return NOERROR;
}

ECode CNotification::GetSortKey(
    /* [out] */ String* sortKey)
{
    VALIDATE_NOT_NULL(sortKey)
    *sortKey = mSortKey;
    return NOERROR;
}

ECode CNotification::SetSortKey(
    /* [in] */ const String& sortKey)
{
    mSortKey = sortKey;
    return NOERROR;
}

ECode CNotification::GetKind(
    /* [out, callee] */ ArrayOf<String> **kind)
{
    VALIDATE_NOT_NULL(kind);
    *kind = mKind;
    REFCOUNT_ADD(*kind);
    return NOERROR;
}

ECode CNotification::SetKind(
    /* [in] */ ArrayOf<String> *kind)
{
    mKind = kind;
    return NOERROR;
}

ECode CNotification::GetExtras(
    /* [out, callee] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras);
    *extras = mExtras;
    REFCOUNT_ADD(*extras)
    return NOERROR;
}

ECode CNotification::SetExtras(
    /* [in] */ IBundle* extras)
{
    mExtras = extras;
    return NOERROR;
}

ECode CNotification::GetActions(
    /* [out, callee] */ ArrayOf<INotificationAction*>** actions)
{
    VALIDATE_NOT_NULL(actions);
    *actions = mActions;
    REFCOUNT_ADD(*actions);
    return NOERROR;
}

ECode CNotification::SetActions(
    /* [in] */ ArrayOf<INotificationAction*>* actions)
{
    mActions = actions;
    return NOERROR;
}

ECode CNotification::SetLatestEventInfo(
    /* [in] */ IContext* context,
    /* [in] */ ICharSequence* contentTitle,
    /* [in] */ ICharSequence* contentText,
    /* [in] */ IPendingIntent* contentIntent)
{
    AutoPtr<CNotificationBuilder> builder;
    CNotificationBuilder::NewByFriend(context, (CNotificationBuilder**)&builder);

    // First, ensure that key pieces of information that may have been set directly
    // are preserved
    builder->SetWhen(mWhen);
    builder->SetSmallIcon(mIcon);
    builder->SetPriority(mPriority);
    builder->SetTicker(mTickerText);
    builder->SetNumber(mNumber);
    builder->SetColor(mColor);
    builder->mFlags = mFlags;
    builder->SetSound(mSound, mAudioStreamType);
    builder->SetDefaults(mDefaults);
    builder->SetVibrate(mVibrate);

    // now apply the latestEventInfo fields
    if (contentTitle != NULL) {
        builder->SetContentTitle(contentTitle);
    }
    if (contentText != NULL) {
        builder->SetContentText(contentText);
    }
    builder->SetContentIntent(contentIntent);
    builder->BuildInto(this);

    return NOERROR;
}

ECode CNotification::GetHeadsUpContentView(
    /* [out] */ IRemoteViews** view)
{
    VALIDATE_NOT_NULL(view)
    *view = mHeadsUpContentView;
    REFCOUNT_ADD(*view)
    return NOERROR;
}

ECode CNotification::SetHeadsUpContentView(
    /* [in] */ IRemoteViews* view)
{
    mHeadsUpContentView = view;
    return NOERROR;
}

ECode CNotification::GetVisibility(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mVisibility;
    return NOERROR;
}

ECode CNotification::SetVisibility(
    /* [in] */ Int32 value)
{
    mVisibility = value;
    return NOERROR;
}

ECode CNotification::GetColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mColor;
    return NOERROR;
}

ECode CNotification::SetColor(
    /* [in] */ Int32 color)
{
    mColor = color;
    return NOERROR;
}

ECode CNotification::GetCategory(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mCategory;
    return NOERROR;
}

ECode CNotification::SetCategory(
    /* [in] */ const String& result)
{
    mCategory = result;
    return NOERROR;
}

ECode CNotification::SetPublicVersion(
    /* [in] */ INotification* value)
{
    mPublicVersion = value;
    return NOERROR;
}

ECode CNotification::GetPublicVersion(
    /* [out] */ INotification** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mPublicVersion;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CNotification::ReadFromParcel(
    /* [in] */ IParcel* parcel)
{
    VALIDATE_NOT_NULL(parcel);
    IParcelable* parcelable;
    Int32 value, version;

    parcel->ReadInt32(&version);
    parcel->ReadInt64(&mWhen);
    parcel->ReadInt32(&mIcon);
    parcel->ReadInt32(&mNumber);

    mContentIntent = NULL;
    parcel->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IInterface> obj;
        parcel->ReadInterfacePtr((Handle32*)&obj);
        mContentIntent = IPendingIntent::Probe(obj);
    }

    mDeleteIntent = NULL;
    parcel->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IInterface> obj;
        parcel->ReadInterfacePtr((Handle32*)&obj);
        mDeleteIntent = IPendingIntent::Probe(obj);
    }

    mTickerText = NULL;
    parcel->ReadInt32(&value);
    if (value != 0) {
        FAIL_RETURN(TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(parcel, (ICharSequence**)&mTickerText));
    }

    mTickerView = NULL;
    parcel->ReadInt32(&value);
    if (value != 0) {
        CRemoteViews::New((IRemoteViews**)&mTickerView);
        parcelable = IParcelable::Probe(mTickerView);
        parcelable->ReadFromParcel(parcel);
    }

    mContentView = NULL;
    parcel->ReadInt32(&value);
    if (value != 0) {
        CRemoteViews::New((IRemoteViews**)&mContentView);
        parcelable = IParcelable::Probe(mContentView);
        parcelable->ReadFromParcel(parcel);
    }

    mLargeIcon = NULL;
    parcel->ReadInt32(&value);
    if (value != 0) {
        CBitmap::New((IBitmap**)&mLargeIcon);
        parcelable = IParcelable::Probe(mLargeIcon);
        parcelable->ReadFromParcel(parcel);
    }

    parcel->ReadInt32(&mDefaults);
    parcel->ReadInt32(&mFlags);

    mSound = NULL;
    parcel->ReadInt32(&value);
    if (value != 0) {
        AutoPtr<IInterface> obj;
        parcel->ReadInterfacePtr((Handle32*)&obj);
        mSound = IUri::Probe(obj);
    }

    parcel->ReadInt32(&mAudioStreamType);
    parcel->ReadInt32(&value);
    if (value != 0) {
        CAudioAttributes::New((IAudioAttributes**)&mAudioAttributes);
        IParcelable::Probe(mAudioAttributes)->ReadFromParcel(parcel);
    }

    mVibrate = NULL;
    parcel->ReadArrayOf((Handle32*)(&mVibrate));

    parcel->ReadInt32(&mLedARGB);
    parcel->ReadInt32(&mLedOnMS);
    parcel->ReadInt32(&mLedOffMS);
    parcel->ReadInt32(&mIconLevel);

    parcel->ReadInt32(&value);
    if (value != 0) {
        CPendingIntent::New((IPendingIntent**)&mFullScreenIntent);
        parcelable = IParcelable::Probe(mFullScreenIntent);
        parcelable->ReadFromParcel(parcel);
    }

    parcel->ReadInt32(&mPriority);

    parcel->ReadString(&mCategory);
    parcel->ReadString(&mGroupKey);
    parcel->ReadString(&mSortKey);

    mExtras = Elastos::Droid::Os::CParcel::ReadBundle(parcel);// may be NULL

    mActions = NULL;
    parcel->ReadInt32(&value);
    if (value >= 0) {
        Int32 size = value;
        mActions = ArrayOf<INotificationAction*>::Alloc(size);
        for (Int32 i = 0; i < size; ++i) {
            AutoPtr<INotificationAction> nc;
            parcel->ReadInt32(&value);
            if (value != 0) {
                CNotificationAction::New((INotificationAction**)&nc);
                IParcelable::Probe(nc)->ReadFromParcel(parcel);
            }

            mActions->Set(i, nc);
        }
    }

    parcel->ReadInt32(&value);
    if (value != 0) {
        CRemoteViews::New((IRemoteViews**)&mBigContentView);
        IParcelable::Probe(mBigContentView)->ReadFromParcel(parcel);
    }

    parcel->ReadInt32(&value);
    if (value != 0) {
        CRemoteViews::New((IRemoteViews**)&mHeadsUpContentView);
        IParcelable::Probe(mHeadsUpContentView)->ReadFromParcel(parcel);
    }

    parcel->ReadInt32(&mVisibility);

    parcel->ReadInt32(&value);
    if (value != 0) {
        mPublicVersion = NULL;
        CNotification::New((INotification**)&mPublicVersion);
        IParcelable::Probe(mPublicVersion)->ReadFromParcel(parcel);
    }

    parcel->ReadInt32(&mColor);

    return NOERROR;
}

ECode CNotification::WriteToParcel(
    /* [in] */ IParcel *parcel)
{
    VALIDATE_NOT_NULL(parcel);
    IParcelable* parcelable;

    parcel->WriteInt32(1);

    parcel->WriteInt64(mWhen);
    parcel->WriteInt32(mIcon);
    parcel->WriteInt32(mNumber);

    if (mContentIntent != NULL) {
        parcel->WriteInt32(1);
        parcel->WriteInterfacePtr(mContentIntent);
    } else {
        parcel->WriteInt32(0);
    }

    if (mDeleteIntent != NULL) {
        parcel->WriteInt32(1);
        parcel->WriteInterfacePtr(mDeleteIntent);
    } else {
        parcel->WriteInt32(0);
    }

    if (mTickerText != NULL) {
        parcel->WriteInt32(1);
        TextUtils::WriteToParcel(mTickerText, parcel);
    } else {
        parcel->WriteInt32(0);
    }

    if (mTickerView != NULL) {
        parcel->WriteInt32(1);
        parcelable = IParcelable::Probe(mTickerView);
        parcelable->WriteToParcel(parcel);
    } else {
        parcel->WriteInt32(0);
    }

    if (mContentView != NULL) {
        parcel->WriteInt32(1);
        IParcelable::Probe(mContentView)->WriteToParcel(parcel);
    } else {
        parcel->WriteInt32(0);
    }

    if (mLargeIcon != NULL) {
        parcel->WriteInt32(1);
        IParcelable::Probe(mLargeIcon)->WriteToParcel(parcel);
    } else {
        parcel->WriteInt32(0);
    }

    parcel->WriteInt32(mDefaults);
    parcel->WriteInt32(mFlags);

    if (mSound != NULL) {
        parcel->WriteInt32(1);
        parcel->WriteInterfacePtr(mSound);
    } else {
        parcel->WriteInt32(0);
    }

    parcel->WriteInt32(mAudioStreamType);
    if (mAudioAttributes != NULL) {
        parcel->WriteInt32(1);
        IParcelable::Probe(mAudioAttributes)->WriteToParcel(parcel);
    } else {
        parcel->WriteInt32(0);
    }

    parcel->WriteArrayOf((Handle32)mVibrate.Get());
    parcel->WriteInt32(mLedARGB);
    parcel->WriteInt32(mLedOnMS);
    parcel->WriteInt32(mLedOffMS);
    parcel->WriteInt32(mIconLevel);

    if (mFullScreenIntent != NULL) {
        parcel->WriteInt32(1);
        IParcelable::Probe(mFullScreenIntent)->WriteToParcel(parcel);
    } else {
        parcel->WriteInt32(0);
    }

    parcel->WriteInt32(mPriority);

    parcel->WriteString(mCategory);

    parcel->WriteString(mGroupKey);

    parcel->WriteString(mSortKey);

    Elastos::Droid::Os::CParcel::WriteBundle(parcel, mExtras); // NULL ok

    if (mActions != NULL) {
        Int32 N = mActions->GetLength();
        parcel->WriteInt32(N);
        for (Int32 i = 0; i < N; ++i) {
            INotificationAction* action = (*mActions)[i];
            if (action != NULL) {
                parcel->WriteInt32(1);
                IParcelable::Probe(action)->WriteToParcel(parcel);
            }
            else {
                parcel->WriteInt32(0);
            }
        }
    }
    else {
        parcel->WriteInt32(-1);
    }


    if (mBigContentView != NULL) {
        parcel->WriteInt32(1);
        IParcelable::Probe(mBigContentView)->WriteToParcel(parcel);
    }
    else {
        parcel->WriteInt32(0);
    }

    if (mHeadsUpContentView != NULL) {
        parcel->WriteInt32(1);
        IParcelable::Probe(mHeadsUpContentView)->WriteToParcel(parcel);
    }
    else {
        parcel->WriteInt32(0);
    }

    parcel->WriteInt32(mVisibility);

    if (mPublicVersion != NULL) {
        parcel->WriteInt32(1);
        IParcelable::Probe(mPublicVersion)->WriteToParcel(parcel);
    }
    else {
        parcel->WriteInt32(0);
    }

    parcel->WriteInt32(mColor);

    return NOERROR;
}

ECode CNotification::ToString(
    /* [out] */ String* info)
{
    VALIDATE_NOT_NULL(info);

    StringBuilder sb("Notification={pri=");
    sb += mPriority;
    sb += ", contentView=";
    if (mContentView != NULL) {
        sb += Object::ToString(mContentView);
    } else {
        sb += ("NULL");
    }

    sb += ", ticker=";
    if (mTickerText != NULL) {
        sb += Object::ToString(mTickerText);;
    }
    else {
        sb += ("NULL");
    }

    sb += ", vibrate=";
    if (mVibrate != NULL && mVibrate->GetLength() > 0) {
        sb += "{";
        for (Int32 i = 0; i < mVibrate->GetLength(); i++) {
            if (i > 0) sb += ", ";
            sb += (*mVibrate)[i];
        }
        sb += "}";
    } else if ((mDefaults & DEFAULT_VIBRATE) != 0) {
        sb += "default";
    } else {
        sb += ("NULL");
    }

    sb += ", sound=";
    if (mSound != NULL) {
        sb += Object::ToString(mSound);
    } else if ((mDefaults & DEFAULT_SOUND) != 0) {
        sb += "default";
    } else {
        sb += "NULL";
    }

    sb += ", defaults=0x";
    sb += StringUtils::ToHexString(mDefaults);
    sb += ", flags=0x";
    sb += StringUtils::ToHexString(mFlags);

    sb += " color=0x";
    sb += StringUtils::ToHexString(mColor);
    if (!mCategory.IsNull()) {
        sb += " category=";
        sb += mCategory;
    }
    if (!mGroupKey.IsNull()) {
        sb += " groupKey=";
        sb += mGroupKey;
    }
    if (!mSortKey.IsNull()) {
        sb += " sortKey=";
        sb += mSortKey;
    }

    if (mActions != NULL) {
        sb += " actions=";
        sb += mActions->GetLength();
    }
    sb += " vis=";
    sb += VisibilityToString(mVisibility);
    if (mPublicVersion != NULL) {
        sb += " publicVersion=";
        sb += Object::ToString(mPublicVersion);
    }

    sb += ("}");

    *info = sb.ToString();
    return NOERROR;
}

ECode CNotification::Clone(
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    AutoPtr<INotification> that;
    CNotification::New((INotification**)&that);
    CloneInto(that, TRUE);
    *result = that;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CNotification::CloneInto(
    /* [in] */ INotification* thatObj,
    /* [in] */ Boolean heavy)
{
    CNotification* that = (CNotification*)thatObj;
    that->mWhen = this->mWhen;
    that->mIcon = this->mIcon;
    that->mNumber = this->mNumber;

    // PendingIntents are global, so there's no reason (or way) to clone them.
    that->mContentIntent = this->mContentIntent;
    that->mDeleteIntent = this->mDeleteIntent;
    that->mFullScreenIntent = this->mFullScreenIntent;

    if (this->mTickerText != NULL) {
        String str = Object::ToString(this->mTickerText);
        that->mTickerText = CoreUtils::Convert(str);
    }
    if (heavy && this->mTickerView != NULL) {
        that->mTickerText = NULL;
        this->mTickerView->Clone((IRemoteViews**)&that->mTickerText);
    }
    if (heavy && this->mContentView != NULL) {
        that->mContentView = NULL;
        this->mContentView->Clone((IRemoteViews**)&that->mContentView);
    }
    if (heavy && this->mLargeIcon != NULL) {
        that->mLargeIcon = NULL;
        CBitmap::CreateBitmap(this->mLargeIcon, (IBitmap**)&that->mLargeIcon);
    }
    that->mIconLevel = this->mIconLevel;
    that->mSound = this->mSound; // android.net.Uri is immutable
    that->mAudioStreamType = this->mAudioStreamType;
    if (this->mAudioAttributes != NULL) {
        AutoPtr<IAudioAttributesBuilder> builder;
        CAudioAttributesBuilder::New(this->mAudioAttributes, (IAudioAttributesBuilder**)&builder);
        that->mAudioAttributes = NULL;
        builder->Build((IAudioAttributes**)&that->mAudioAttributes);
    }

    if (this->mVibrate != NULL) {
        Int32 N = this->mVibrate->GetLength();
        that->mVibrate = ArrayOf<Int64>::Alloc(N);
        that->mVibrate->Copy(this->mVibrate);
    }

    that->mLedARGB = this->mLedARGB;
    that->mLedOnMS = this->mLedOnMS;
    that->mLedOffMS = this->mLedOffMS;
    that->mDefaults = this->mDefaults;

    that->mFlags = this->mFlags;

    that->mPriority = this->mPriority;

    that->mCategory = this->mCategory;

    that->mGroupKey = this->mGroupKey;

    that->mSortKey = this->mSortKey;

    if (this->mExtras != NULL) {
        that->mExtras = NULL;
        CBundle::New(this->mExtras, (IBundle**)&that->mExtras);
        Int32 size;
        ECode ec = that->mExtras->GetSize(&size); // will unparcel
        if (ec == (ECode)E_BAD_PARCELABLE_EXCEPTION) {
            Logger::E(TAG, "could not unparcel extras from notification: %s", TO_CSTR(this));
            that->mExtras = NULL;
        }
    }

    if (this->mActions != NULL) {
        that->mActions = ArrayOf<INotificationAction*>::Alloc(this->mActions->GetLength());
        for(Int32 i = 0; i < this->mActions->GetLength(); i++) {
            INotificationAction* action = (*this->mActions)[i];
            AutoPtr<IInterface> copy;
            ICloneable::Probe(action)->Clone((IInterface**)&copy);
            that->mActions->Set(i, INotificationAction::Probe(copy));
        }
    }

    if (heavy && this->mBigContentView != NULL) {
        that->mBigContentView = NULL;
        this->mBigContentView->Clone((IRemoteViews**)&that->mBigContentView);
    }

    if (heavy && this->mHeadsUpContentView != NULL) {
        that->mHeadsUpContentView = NULL;
        this->mHeadsUpContentView->Clone((IRemoteViews**)&that->mHeadsUpContentView);
    }

    that->mVisibility = this->mVisibility;

    if (this->mPublicVersion != NULL) {
        that->mPublicVersion = NULL;
        CNotification::New((INotification**)&that->mPublicVersion);
        this->mPublicVersion->CloneInto(that->mPublicVersion, heavy);
    }

    that->mColor = this->mColor;

    if (!heavy) {
        that->LightenPayload(); // will clean out extras
    }
    return NOERROR;
}

ECode CNotification::LightenPayload()
{
    mTickerView = NULL;
    mContentView = NULL;
    mBigContentView = NULL;
    mHeadsUpContentView = NULL;
    mLargeIcon = NULL;
    if (mExtras != NULL) {
        mExtras->Remove(INotification::EXTRA_LARGE_ICON);
        mExtras->Remove(INotification::EXTRA_LARGE_ICON_BIG);
        mExtras->Remove(INotification::EXTRA_PICTURE);
        mExtras->Remove(INotification::EXTRA_BIG_TEXT);
        // Prevent light notifications from being rebuilt.
        mExtras->Remove(INotificationBuilder::EXTRA_NEEDS_REBUILD);
    }
    return NOERROR;
}

AutoPtr<ICharSequence> CNotification::SafeCharSequence(
    /* [in] */ ICharSequence* cs)
{
    if (cs == NULL) return cs;

    AutoPtr<ICharSequence> result = cs;
    Int32 length;
    result->GetLength(&length);
    if (length > MAX_CHARSEQUENCE_LENGTH) {
        result = NULL;
        cs->SubSequence(0, MAX_CHARSEQUENCE_LENGTH, (ICharSequence**)&result);
    }
    if (IParcelable::Probe(cs) != NULL) {
        EGuid clsId;
        IObject::Probe(cs)->GetClassID(&clsId);
        Logger::E(TAG, "warning: %s instance is a custom Parcelable and not allowed in Notification",
            clsId.mUunm);
        String str = Object::ToString(result);
        return CoreUtils::Convert(str);
    }

    return result;
}

String CNotification::VisibilityToString(
    /* [in] */ Int32 vis)
{
    switch (vis) {
        case INotification::VISIBILITY_PRIVATE:
            return String("PRIVATE");
        case INotification::VISIBILITY_PUBLIC:
            return String("PUBLIC");
        case INotification::VISIBILITY_SECRET:
            return String("SECRET");
        default:{
            StringBuilder sb("UNKNOWN(");
            sb += vis;
            sb += ")";
            return sb.ToString();
        }
    }
    return String(NULL);
}

Boolean CNotification::IsValid()
{
    // Would like to check for icon!=0 here, too, but NotificationManagerService accepts that
    // for legacy reasons.
    Boolean bval;
    return mContentView != NULL || (mExtras->GetBoolean(INotificationBuilder::EXTRA_REBUILD_CONTENT_VIEW, &bval), bval);
}

Boolean CNotification::IsGroupSummary()
{
    return mGroupKey != NULL && (mFlags & FLAG_GROUP_SUMMARY) != 0;
}

Boolean CNotification::IsGroupChild()
{
    return mGroupKey != NULL && (mFlags & FLAG_GROUP_SUMMARY) == 0;
}

ECode CNotification::IsValid(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsValid();
    return NOERROR;
}

ECode CNotification::IsGroupSummary(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsGroupSummary();
    return NOERROR;
}

ECode CNotification::IsGroupChild(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = IsGroupChild();
    return NOERROR;
}

}
}
}
