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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/R.h"
#include "elastos/droid/app/CNotification.h"
#include "elastos/droid/app/CNotificationAction.h"
#include "elastos/droid/app/CNotificationBuilder.h"
#include "elastos/droid/app/CNotificationBuilderRemoteViews.h"
#include "elastos/droid/app/NotificationStyle.h"
#include "elastos/droid/graphics/CBitmap.h"
#include "elastos/droid/graphics/CCanvas.h"
#include "elastos/droid/internal/utility/NotificationColorUtil.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/CUserHandle.h"
#include "elastos/droid/os/SystemClock.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/MathUtils.h"
#include "elastos/droid/widget/CRemoteViews.h"
#include <Elastos.CoreLibrary.Text.h>
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/core/ClassLoader.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Droid::Graphics::BitmapConfig_ARGB_8888;
using Elastos::Droid::Graphics::CBitmap;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::PorterDuffMode_MULTIPLY;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP;
using Elastos::Droid::Internal::Utility::NotificationColorUtil;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Os::CUserHandle;
using Elastos::Droid::Os::IUserHandle;
using Elastos::Droid::Os::SystemClock;
using Elastos::Droid::R;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::Widget::CRemoteViews;
using Elastos::Core::ClassLoader;
using Elastos::Core::CoreUtils;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;
using Elastos::Text::CNumberFormatHelper;
using Elastos::Text::INumberFormatHelper;

namespace Elastos {
namespace Droid {
namespace App {

static const String TAG("CNotificationBuilder");
const Int32 CNotificationBuilder::MAX_ACTION_BUTTONS = 3;
const Float CNotificationBuilder::LARGE_TEXT_SCALE = 1.3f;
const Boolean CNotificationBuilder::DBG = FALSE;

const String CNotificationBuilder::EXTRA_REBUILD_CONTEXT_APPLICATION_INFO("android.rebuild.applicationInfo");

const Boolean CNotificationBuilder::STRIP_AND_REBUILD = TRUE;

CAR_INTERFACE_IMPL(CNotificationBuilder, Object, INotificationBuilder)

CAR_OBJECT_IMPL(CNotificationBuilder)

CNotificationBuilder::CNotificationBuilder()
    : mWhen(0)
    , mSmallIcon(0)
    , mSmallIconLevel(0)
    , mNumber(0)
    , mAudioStreamType(0)
    , mLedArgb(0)
    , mLedOnMs(0)
    , mLedOffMs(0)
    , mDefaults(0)
    , mFlags(0)
    , mProgressMax(0)
    , mProgress(0)
    , mProgressIndeterminate(FALSE)
    , mPriority(0)
    , mUseChronometer(FALSE)
    , mShowWhen(TRUE)
    , mVisibility(INotification::VISIBILITY_PRIVATE)
    , mColor(INotification::COLOR_DEFAULT)
    , mOriginatingUserId(0)
    , mHasThreeLines(FALSE)
{
    CBundle::New((IBundle**)&mRebuildBundle);
}

CNotificationBuilder::~CNotificationBuilder()
{
}

ECode CNotificationBuilder::constructor(
    /* [in] */ IContext *context)
{
    /*
     * Important compatibility note!
     * Some apps out in the wild create a Notification.Builder in their Activity subclass
     * constructor for later use. At this point Activities - themselves subclasses of
     * ContextWrapper - do not have their inner Context populated yet. This means that
     * any calls to Context methods from within this constructor can cause NPEs in existing
     * apps. Any data populated from mContext should therefore be populated lazily to
     * preserve compatibility.
     */
    mContext = context;

    // Set defaults to match the defaults of a Notification
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    system->GetCurrentTimeMillis(&mWhen);
    mAudioStreamType = INotification::STREAM_DEFAULT;
    mAudioAttributes = CNotification::AUDIO_ATTRIBUTES_DEFAULT;
    mPriority = INotification::PRIORITY_DEFAULT;

    AutoPtr<IApplicationInfo> ai;
    context->GetApplicationInfo((IApplicationInfo**)&ai);
    Int32 version;
    ai->GetTargetSdkVersion(&version);

    mColorUtil = version < Build::VERSION_CODES::LOLLIPOP ?
        NotificationColorUtil::GetInstance(mContext) : NULL;
    return NOERROR;
}

ECode CNotificationBuilder::constructor(
    /* [in] */ IContext* context,
    /* [in] */ INotification* n)
{
    FAIL_RETURN(constructor(context))
    mRebuildNotification = n;
    RestoreFromNotification(n);

    AutoPtr<INotificationStyle> style;
    AutoPtr<IBundle> extras;
    n->GetExtras((IBundle**)&extras);
    String templateClass;
    extras->GetString(INotification::EXTRA_TEMPLATE, &templateClass);
    if (!TextUtils::IsEmpty(templateClass)) {
        AutoPtr<IClassInfo> classInfo = GetNotificationStyleClass(templateClass);
        if (classInfo == NULL) {
            Logger::D(TAG, "Unknown style class: %s", templateClass.string());
            return NOERROR;
        }

        AutoPtr<IInterface> obj;
        ECode ec = classInfo->CreateObject((IInterface**)&obj);
        if (FAILED(ec)) {
            Logger::E(TAG, "Could not create Style %s", templateClass.string());
            return ec;
        }
        style = INotificationStyle::Probe(obj);
        style->RestoreFromExtras(extras);
    }
    if (style != NULL) {
        SetStyle(style);
    }
    return NOERROR;
}

ECode CNotificationBuilder::SetWhen(
    /* [in] */ Int64 when)
{
    mWhen = when;
    return NOERROR;
}

ECode CNotificationBuilder::SetShowWhen(
    /* [in] */ Boolean show)
{
    mShowWhen = show;
    return NOERROR;
}

ECode CNotificationBuilder::SetUsesChronometer(
    /* [in] */ Boolean b)
{
    mUseChronometer = b;
    return NOERROR;
}

ECode CNotificationBuilder::SetSmallIcon(
    /* [in] */ Int32 icon)
{
    mSmallIcon = icon;
    return NOERROR;
}

ECode CNotificationBuilder::SetSmallIcon(
    /* [in] */ Int32 icon,
    /* [in] */ Int32 level)
{
    mSmallIcon = icon;
    mSmallIconLevel = level;
    return NOERROR;
}

ECode CNotificationBuilder::SetContentTitle(
    /* [in] */ ICharSequence *title)
{
    mContentTitle = CNotification::SafeCharSequence(title);
    return NOERROR;
}

ECode CNotificationBuilder::SetContentText(
    /* [in] */ ICharSequence *text)
{
    mContentText = CNotification::SafeCharSequence(text);
    return NOERROR;
}

ECode CNotificationBuilder::SetSubText(
    /* [in] */ ICharSequence *text)
{
    mSubText = CNotification::SafeCharSequence(text);
    return NOERROR;
}

ECode CNotificationBuilder::SetNumber(
    /* [in] */ Int32 number)
{
    mNumber = number;
    return NOERROR;
}

ECode CNotificationBuilder::SetContentInfo(
    /* [in] */ ICharSequence *info)
{
    mContentInfo = CNotification::SafeCharSequence(info);
    return NOERROR;
}

ECode CNotificationBuilder::SetProgress(
    /* [in] */ Int32 max,
    /* [in] */ Int32 progress,
    /* [in] */ Boolean indeterminate)
{
    mProgressMax = max;
    mProgress = progress;
    mProgressIndeterminate = indeterminate;
    return NOERROR;
}

ECode CNotificationBuilder::SetContent(
    /* [in] */ IRemoteViews *views)
{
    mContentView = views;
    return NOERROR;
}

ECode CNotificationBuilder::SetContentIntent(
    /* [in] */ IPendingIntent *intent)
{
    mContentIntent = intent;
    return NOERROR;
}

ECode CNotificationBuilder::SetDeleteIntent(
    /* [in] */ IPendingIntent *intent)
{
    mDeleteIntent = intent;
    return NOERROR;
}

ECode CNotificationBuilder::SetFullScreenIntent(
    /* [in] */ IPendingIntent *intent,
    /* [in] */ Boolean highPriority)
{
    mFullScreenIntent = intent;
    SetFlag(INotification::FLAG_HIGH_PRIORITY, highPriority);
    return NOERROR;
}

ECode CNotificationBuilder::SetTicker(
    /* [in] */ ICharSequence *tickerText)
{
    mTickerText = CNotification::SafeCharSequence(tickerText);
    return NOERROR;
}

ECode CNotificationBuilder::SetTicker(
    /* [in] */ ICharSequence *tickerText,
    /* [in] */ IRemoteViews *views)
{
    mTickerText = CNotification::SafeCharSequence(tickerText);
    mTickerView = views; // we'll save it for you anyway
    return NOERROR;
}

ECode CNotificationBuilder::SetLargeIcon(
    /* [in] */ IBitmap *icon)
{
    mLargeIcon = icon;
    return NOERROR;
}

ECode CNotificationBuilder::SetSound(
    /* [in] */ IUri *sound)
{
    mSound = sound;
    mAudioAttributes = CNotification::AUDIO_ATTRIBUTES_DEFAULT;
    return NOERROR;
}

ECode CNotificationBuilder::SetSound(
    /* [in] */ IUri *sound,
    /* [in] */ Int32 streamType)
{
    mSound = sound;
    mAudioStreamType = streamType;
    return NOERROR;
}

ECode CNotificationBuilder::SetSound(
    /* [in] */ IUri *sound,
    /* [in] */ IAudioAttributes* audioAttributes)
{
    mSound = sound;
    mAudioAttributes = audioAttributes;
    return NOERROR;
}

ECode CNotificationBuilder::SetVibrate(
    /* [in] */ ArrayOf<Int64> *pattern)
{
    mVibrate = pattern;
    return NOERROR;
}

ECode CNotificationBuilder::SetLights(
    /* [in] */ Int32 argb,
    /* [in] */ Int32 onMs,
    /* [in] */ Int32 offMs)
{
    mLedArgb = argb;
    mLedOnMs = onMs;
    mLedOffMs = offMs;
    return NOERROR;
}

ECode CNotificationBuilder::SetOngoing(
    /* [in] */ Boolean ongoing)
{
    SetFlag(INotification::FLAG_ONGOING_EVENT, ongoing);
    return NOERROR;
}

ECode CNotificationBuilder::SetOnlyAlertOnce(
    /* [in] */ Boolean onlyAlertOnce)
{
    SetFlag(INotification::FLAG_ONLY_ALERT_ONCE, onlyAlertOnce);
    return NOERROR;
}

ECode CNotificationBuilder::SetAutoCancel(
    /* [in] */ Boolean autoCancel)
{
    SetFlag(INotification::FLAG_AUTO_CANCEL, autoCancel);
    return NOERROR;
}

ECode CNotificationBuilder::SetLocalOnly(
    /* [in] */ Boolean localOnly)
{
    SetFlag(INotification::FLAG_LOCAL_ONLY, localOnly);
    return NOERROR;
}

ECode CNotificationBuilder::SetDefaults(
    /* [in] */ Int32 defaults)
{
    mDefaults = defaults;
    return NOERROR;
}

ECode CNotificationBuilder::SetPriority(
    /* [in] */ Int32 pri)
{
    mPriority = pri;
    return NOERROR;
}

ECode CNotificationBuilder::SetGroup(
    /* [in] */ const String& groupKey)
{
    mGroupKey = groupKey;
    return NOERROR;
}

ECode CNotificationBuilder::SetGroupSummary(
    /* [in] */ Boolean isGroupSummary)
{
    SetFlag(INotification::FLAG_GROUP_SUMMARY, isGroupSummary);
    return NOERROR;
}

ECode CNotificationBuilder::SetSortKey(
    /* [in] */ const String& sortKey)
{
    mSortKey = sortKey;
    return NOERROR;
}

ECode CNotificationBuilder::SetCategory(
    /* [in] */ const String& category)
{
    mCategory = category;
    return NOERROR;
}

ECode CNotificationBuilder::AddPerson(
    /* [in] */ const String& person)
{
    mPeople.PushBack(person);
    return NOERROR;
}

ECode CNotificationBuilder::AddExtras(
    /* [in] */ IBundle * extras)
{
    if (extras != NULL) {
        mExtras->PutAll(extras);
    }
    return NOERROR;
}

ECode CNotificationBuilder::SetExtras(
    /* [in] */ IBundle *extras)
{
    mExtras = extras;
    return NOERROR;
}

ECode CNotificationBuilder::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras)
    *extras = mExtras;
    REFCOUNT_ADD(*extras)
    return NOERROR;
}

ECode CNotificationBuilder::AddAction(
    /* [in] */ Int32 icon,
    /* [in] */ ICharSequence *title,
    /* [in] */ IPendingIntent *intent)
{
    AutoPtr<INotificationAction> action;
    CNotificationAction::New(icon, CNotification::SafeCharSequence(title), intent, (INotificationAction**)&action);
    mActions.PushBack(action);
    return NOERROR;
}

ECode CNotificationBuilder::AddAction(
    /* [in] */ INotificationAction* action)
{
    mActions.PushBack(action);
    return NOERROR;
}

ECode CNotificationBuilder::SetStyle(
    /* [in] */ INotificationStyle *style)
{
    if (mStyle.Get() != style) {
        mStyle = style;
        if (mStyle != NULL) {
            mStyle->SetBuilder(this);
        }
    }
    return NOERROR;
}

ECode CNotificationBuilder::SetVisibility(
    /* [in] */ Int32 visibility)
{
    mVisibility = visibility;
    return NOERROR;
}

ECode CNotificationBuilder::SetPublicVersion(
    /* [in] */ INotification* n)
{
    mPublicVersion = n;
    return NOERROR;
}

ECode CNotificationBuilder::Extend(
    /* [in] */ INotificationExtender* extender)
{
    extender->Extend(this);
    return NOERROR;
}

ECode CNotificationBuilder::SetColor(
    /* [in] */ Int32 argb)
{
    mColor = argb;
    return NOERROR;
}

AutoPtr<IDrawable> CNotificationBuilder::GetProfileBadgeDrawable()
{
    // Note: This assumes that the current user can read the profile badge of the
    // originating user.
    AutoPtr<IPackageManager> mgr;
    mContext->GetPackageManager((IPackageManager**)&mgr);
    AutoPtr<IUserHandle> user;
    CUserHandle::New(mOriginatingUserId, (IUserHandle**)&user);
    AutoPtr<IDrawable> drawable;
    mgr->GetUserBadgeForDensity(user, 0, (IDrawable**)&drawable);
    return drawable;
}

AutoPtr<IBitmap> CNotificationBuilder::GetProfileBadge()
{
    AutoPtr<IDrawable> badge = GetProfileBadgeDrawable();
    if (badge == NULL) {
        return NULL;
    }
    Int32 size;
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    res->GetDimensionPixelSize(
        R::dimen::notification_badge_size, &size);
    AutoPtr<IBitmap> bitmap;
    CBitmap::CreateBitmap(size, size, BitmapConfig_ARGB_8888, (IBitmap**)&bitmap);
    AutoPtr<ICanvas> canvas;
    CCanvas::New(bitmap, (ICanvas**)&canvas);
    badge->SetBounds(0, 0, size, size);
    badge->Draw(canvas);
    return bitmap;
}

Boolean CNotificationBuilder::AddProfileBadge(
    /* [in] */ IRemoteViews* contentView,
    /* [in] */ Int32 resId)
{
    AutoPtr<IBitmap> profileBadge = GetProfileBadge();

    contentView->SetViewVisibility(R::id::profile_badge_large_template, IView::GONE);
    contentView->SetViewVisibility(R::id::profile_badge_line2, IView::GONE);
    contentView->SetViewVisibility(R::id::profile_badge_line3, IView::GONE);

    if (profileBadge != NULL) {
        contentView->SetImageViewBitmap(resId, profileBadge);
        contentView->SetViewVisibility(resId, IView::VISIBLE);

        // Make sure Line 3 is visible. As badge will be here if there
        // is no text to display.
        if (resId == R::id::profile_badge_line3) {
            contentView->SetViewVisibility(R::id::line3, IView::VISIBLE);
        }
        return TRUE;
    }
    return FALSE;
}

ECode CNotificationBuilder::ShrinkLine3Text(
    /* [in] */ IRemoteViews* contentView)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 subTextSize;
    res->GetDimensionPixelSize(
        R::dimen::notification_subtext_size, &subTextSize);
    return contentView->SetTextViewTextSize(
        R::id::text, ITypedValue::COMPLEX_UNIT_PX, subTextSize);
}

ECode CNotificationBuilder::UnshrinkLine3Text(
    /* [in] */ IRemoteViews* contentView)
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 regularTextSize;
    res->GetDimensionPixelSize(
        R::dimen::notification_text_size, &regularTextSize);
    return contentView->SetTextViewTextSize(
        R::id::text, ITypedValue::COMPLEX_UNIT_PX, regularTextSize);
}

ECode CNotificationBuilder::ResetStandardTemplate(
    /* [in] */ IRemoteViews* contentView)
{
    RemoveLargeIconBackground(contentView);
    contentView->SetViewPadding(R::id::icon, 0, 0, 0, 0);
    contentView->SetImageViewResource(R::id::icon, 0);
    contentView->SetInt32(R::id::icon, String("SetBackgroundResource"), 0);
    contentView->SetViewVisibility(R::id::right_icon, IView::GONE);
    contentView->SetInt32(R::id::right_icon, String("SetBackgroundResource"), 0);
    contentView->SetImageViewResource(R::id::right_icon, 0);
    contentView->SetImageViewResource(R::id::icon, 0);
    contentView->SetTextViewText(R::id::title, NULL);
    contentView->SetTextViewText(R::id::text, NULL);
    UnshrinkLine3Text(contentView);
    contentView->SetTextViewText(R::id::text2, NULL);
    contentView->SetViewVisibility(R::id::text2, IView::GONE);
    contentView->SetViewVisibility(R::id::info, IView::GONE);
    contentView->SetViewVisibility(R::id::time, IView::GONE);
    contentView->SetViewVisibility(R::id::line3, IView::GONE);
    contentView->SetViewVisibility(R::id::overflow_divider, IView::GONE);
    contentView->SetViewVisibility(R::id::progress, IView::GONE);
    contentView->SetViewVisibility(R::id::chronometer, IView::GONE);
    contentView->SetViewVisibility(R::id::time, IView::GONE);
    return NOERROR;
}

AutoPtr<IRemoteViews> CNotificationBuilder::ApplyStandardTemplate(
    /* [in] */ Int32 resId)
{
    return ApplyStandardTemplate(resId, TRUE /* hasProgress */);
}

ECode CNotificationBuilder::GetNotification(
    /* [out] */ INotification **notification)
{
    VALIDATE_NOT_NULL(notification);
    return Build(notification);
}

ECode CNotificationBuilder::Build(
    /* [out] */ INotification **notification)
{
    VALIDATE_NOT_NULL(notification);
    *notification = NULL;

    mContext->GetUserId(&mOriginatingUserId);
    mHasThreeLines = HasThreeLines();

    AutoPtr<INotification> n;
    BuildUnstyled((INotification**)&n);

    if (mStyle != NULL) {
        mStyle->BuildStyled(n);
    }

    AutoPtr<IBundle> extras;
    n->GetExtras((IBundle**)&extras);
    if (mExtras != NULL) {
        extras->PutAll(mExtras);
    }

    Int32 size;
    mRebuildBundle->GetSize(&size);
    if (size > 0) {
        extras->PutAll(mRebuildBundle);
        mRebuildBundle->Clear();
    }

    PopulateExtras(extras);
    if (mStyle != NULL) {
        mStyle->AddExtras(extras);
    }

    mHasThreeLines = FALSE;
    *notification = n;
    REFCOUNT_ADD(*notification)
    return NOERROR;
}

ECode CNotificationBuilder::BuildInto(
    /* [in] */ INotification* n)
{
    AutoPtr<INotification> notification;
    Build((INotification**)&notification);
    notification->CloneInto(n, TRUE);
    return NOERROR;
}

ECode CNotificationBuilder::GetSubText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = mSubText;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

ECode CNotificationBuilder::GetContentText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = mContentText;
    REFCOUNT_ADD(*text);
    return NOERROR;
}

Int32 CNotificationBuilder::GetBaseLayoutResource()
{
    return R::layout::notification_template_material_base;
}

Int32 CNotificationBuilder::GetBigBaseLayoutResource()
{
    return R::layout::notification_template_material_big_base;
}

Int32 CNotificationBuilder::GetBigPictureLayoutResource()
{
    return R::layout::notification_template_material_big_picture;
}

Int32 CNotificationBuilder::GetBigTextLayoutResource()
{
    return R::layout::notification_template_material_big_text;
}

Int32 CNotificationBuilder::GetInboxLayoutResource()
 {
    return R::layout::notification_template_material_inbox;
}

Int32 CNotificationBuilder::GetActionLayoutResource()
{
    return R::layout::notification_material_action;
}

Int32 CNotificationBuilder::GetActionTombstoneLayoutResource()
{
    return R::layout::notification_material_action_tombstone;
}

ECode CNotificationBuilder::ApplyStandardTemplateWithActions(
    /* [in] */ Int32 layoutId,
    /* [out] */ IRemoteViews** remoteViews)
{
    VALIDATE_NOT_NULL(remoteViews);
    *remoteViews = NULL;

    AutoPtr<IRemoteViews> big = ApplyStandardTemplate(layoutId);

    ResetStandardTemplateWithActions(big);

    Int32 N = mActions.GetSize();
    if (N > 0) {
        big->SetViewVisibility(R::id::actions, IView::VISIBLE);
        big->SetViewVisibility(R::id::action_divider, IView::VISIBLE);
        if (N > MAX_ACTION_BUTTONS) N = MAX_ACTION_BUTTONS;

        for (Int32 i = 0; i < N; i++) {
            AutoPtr<IRemoteViews> button = GenerateActionButton(mActions[i]);
            big->AddView(R::id::actions, button);
        }
    }

    *remoteViews = big;
    REFCOUNT_ADD(*remoteViews);
    return NOERROR;
}

void CNotificationBuilder::SetFlag(
    /* [in] */ Int32 mask,
    /* [in] */ Boolean value)
{
    if (value) {
        mFlags |= mask;
    }
    else {
        mFlags &= ~mask;
    }
}

AutoPtr<IRemoteViews> CNotificationBuilder::ApplyStandardTemplate(
    /* [in] */ Int32 resId,
    /* [in] */ Boolean hasProgress)
{
    AutoPtr<IApplicationInfo> ai;
    mContext->GetApplicationInfo((IApplicationInfo**)&ai);
    AutoPtr<IRemoteViews> contentView;
    CNotificationBuilderRemoteViews::New(ai, resId, (IRemoteViews**)&contentView);

    ResetStandardTemplate(contentView);

    Boolean showLine3 = FALSE;
    Boolean showLine2 = FALSE;
    Boolean contentTextInLine2 = FALSE;

    if (mLargeIcon != NULL) {
        contentView->SetImageViewBitmap(R::id::icon, mLargeIcon);
        ProcessLargeLegacyIcon(mLargeIcon, contentView);
        contentView->SetImageViewResource(R::id::right_icon, mSmallIcon);
        contentView->SetViewVisibility(R::id::right_icon, IView::VISIBLE);
        ProcessSmallRightIcon(mSmallIcon, contentView);
    }
    else { // small icon at left
        contentView->SetImageViewResource(R::id::icon, mSmallIcon);
        contentView->SetViewVisibility(R::id::icon, IView::VISIBLE);
        ProcessSmallIconAsLarge(mSmallIcon, contentView);
    }

    if (mContentTitle != NULL) {
        contentView->SetTextViewText(R::id::title, ProcessLegacyText(mContentTitle));
    }

    if (mContentText != NULL) {
        contentView->SetTextViewText(R::id::text, ProcessLegacyText(mContentText));
        showLine3 = TRUE;
    }

    if (mContentInfo != NULL) {
        contentView->SetTextViewText(R::id::info, ProcessLegacyText(mContentInfo));
        contentView->SetViewVisibility(R::id::info, IView::VISIBLE);
        showLine3 = TRUE;
    }
    else if (mNumber > 0) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Int32 tooBig;
        res->GetInteger(R::integer::status_bar_notification_info_maxnum, &tooBig);

        if (mNumber > tooBig) {
            AutoPtr<ICharSequence> cs;
            String str;
            res->GetString(R::string::status_bar_notification_info_overflow, &str);
            CString::New(str, (ICharSequence**)&cs);
            contentView->SetTextViewText(R::id::info, ProcessLegacyText(cs));
        }
        else {
            AutoPtr<INumberFormatHelper> helper;
            CNumberFormatHelper::AcquireSingleton((INumberFormatHelper**)&helper);
            AutoPtr<INumberFormat> f;
            helper->GetIntegerInstance((INumberFormat**)&f);

            AutoPtr<ICharSequence> cs;
            String str;
            f->Format((Int64)mNumber, &str);
            CString::New(str, (ICharSequence**)&cs);
            contentView->SetTextViewText(R::id::info, ProcessLegacyText(cs));
        }
        contentView->SetViewVisibility(R::id::info, IView::VISIBLE);
        showLine3 = TRUE;
    }
    else {
        contentView->SetViewVisibility(R::id::info, IView::GONE);
    }

    // Need to show three lines?
    if (mSubText != NULL) {
        contentView->SetTextViewText(R::id::text, mSubText);
        if (mContentText != NULL) {
            contentView->SetTextViewText(R::id::text2, mContentText);
            contentView->SetViewVisibility(R::id::text2, IView::VISIBLE);
            showLine2 = TRUE;
        }
        else {
            contentView->SetViewVisibility(R::id::text2, IView::GONE);
        }
    }
    else {
        contentView->SetViewVisibility(R::id::text2, IView::GONE);
        if (hasProgress && (mProgressMax != 0 || mProgressIndeterminate)) {
            contentView->SetViewVisibility(R::id::progress, IView::VISIBLE);
            contentView->SetProgressBar(
                R::id::progress, mProgressMax, mProgress, mProgressIndeterminate);
            showLine2 = TRUE;
        }
        else {
            contentView->SetViewVisibility(R::id::progress, IView::GONE);
        }
    }

    if (showLine2) {
        // need to shrink all the type to make sure everything fits
        ShrinkLine3Text(contentView);
    }

    if (ShowsTimeOrChronometer()) {
        if (mUseChronometer) {
            AutoPtr<ISystem> system;
            Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
            contentView->SetViewVisibility(R::id::chronometer, IView::VISIBLE);
            Int64 millis;
            system->GetCurrentTimeMillis(&millis);
            contentView->SetInt64(R::id::chronometer, String("SetBase"),
                mWhen + (SystemClock::GetElapsedRealtime() - millis));
            contentView->SetBoolean(R::id::chronometer, String("SetStarted"), TRUE);
        }
        else {
            contentView->SetViewVisibility(R::id::time, IView::VISIBLE);
            contentView->SetInt64(R::id::time, String("SetTime"), mWhen);
        }
    }

    // Adjust padding depending on line count and font size.
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    AutoPtr<IConfiguration> config;
    res->GetConfiguration((IConfiguration**)&config);
    Float fontScale;
    config->GetFontScale(&fontScale);
    contentView->SetViewPadding(R::id::line1, 0,
        CalculateTopPadding(mContext, mHasThreeLines, fontScale), 0, 0);

    // We want to add badge to first line of text.
    Boolean addedBadge = AddProfileBadge(contentView,
        contentTextInLine2 ? R::id::profile_badge_line2 : R::id::profile_badge_line3);
    // If we added the badge to line 3 then we should show line 3.
    if (addedBadge && !contentTextInLine2) {
        showLine3 = TRUE;
    }

    // Note getStandardView may hide line 3 again.
    contentView->SetViewVisibility(R::id::line3, showLine3 ? IView::VISIBLE : IView::GONE);
    contentView->SetViewVisibility(R::id::overflow_divider, showLine3 ? IView::VISIBLE : IView::GONE);
    return contentView;
}

Boolean CNotificationBuilder::ShowsTimeOrChronometer()
{
    return mWhen != 0 && mShowWhen;
}

Boolean CNotificationBuilder::HasThreeLines()
{
    Boolean contentTextInLine2 = mSubText != NULL && mContentText != NULL;
    Boolean hasProgress = mStyle == NULL || ((NotificationStyle*)mStyle.Get())->HasProgress();
    // If we have content text in line 2, badge goes into line 2, or line 3 otherwise
    Boolean badgeInLine3 = GetProfileBadgeDrawable() != NULL && !contentTextInLine2;
    Boolean hasLine3 = mContentText != NULL || mContentInfo != NULL || mNumber > 0
            || badgeInLine3;
    Boolean hasLine2 = (mSubText != NULL && mContentText != NULL) ||
            (hasProgress && mSubText == NULL
                    && (mProgressMax != 0 || mProgressIndeterminate));
    return hasLine2 && hasLine3;
}

Int32 CNotificationBuilder::CalculateTopPadding(
    /* [in] */ IContext* ctx,
    /* [in] */ Boolean HasThreeLines,
    /* [in] */ Float fontScale)
{
    AutoPtr<IResources> res;
    ctx->GetResources((IResources**)&res);

    Int32 padding, largePadding;
    res->GetDimensionPixelSize(HasThreeLines
        ? R::dimen::notification_top_pad_narrow
        : R::dimen::notification_top_pad, &padding);
    res->GetDimensionPixelSize(HasThreeLines
        ? R::dimen::notification_top_pad_large_text_narrow
        : R::dimen::notification_top_pad_large_text, &largePadding);
    Float largeFactor = (MathUtils::Constrain(fontScale, 1.0f, LARGE_TEXT_SCALE) - 1.0f)
            / (LARGE_TEXT_SCALE - 1.0f);

    // Linearly interpolate the padding between large and normal with the font scale ranging
    // from 1f to LARGE_TEXT_SCALE
    return Elastos::Core::Math::Round((1 - largeFactor) * padding + largeFactor * largePadding);
}

ECode CNotificationBuilder::ResetStandardTemplateWithActions(
    /* [in] */ IRemoteViews* big)
{
    big->SetViewVisibility(R::id::actions, IView::GONE);
    big->SetViewVisibility(R::id::action_divider, IView::GONE);
    big->RemoveAllViews(R::id::actions);
    return NOERROR;
}

AutoPtr<IRemoteViews> CNotificationBuilder::MakeContentView()
{
    if (mContentView != NULL) {
        return mContentView;
    }
    else {
        return ApplyStandardTemplate(GetBaseLayoutResource());
    }
}

AutoPtr<IRemoteViews> CNotificationBuilder::MakeTickerView()
{
    if (mTickerView != NULL) {
        return mTickerView;
    }
    return NULL; // tickers are not created by default anymore
}

AutoPtr<IRemoteViews> CNotificationBuilder::MakeBigContentView()
{
    if (mActions.IsEmpty()) return NULL;
    AutoPtr<IRemoteViews> rv;
    ApplyStandardTemplateWithActions(GetBigBaseLayoutResource(), (IRemoteViews**)&rv);
    return rv;
}

AutoPtr<IRemoteViews> CNotificationBuilder::MakeHeadsUpContentView()
{
    if (mActions.IsEmpty()) return NULL;
    AutoPtr<IRemoteViews> rv;
    ApplyStandardTemplateWithActions(GetBigBaseLayoutResource(), (IRemoteViews**)&rv);
    return rv;
}

AutoPtr<IRemoteViews> CNotificationBuilder::GenerateActionButton(
    /* [in] */ INotificationAction * action)
{
    assert(action != NULL);

    AutoPtr<IPendingIntent> actionIntent;
    action->GetActionIntent((IPendingIntent**)&actionIntent);
    Boolean tombstone = (actionIntent == NULL);
    AutoPtr<IRemoteViews> button;
    String name;
    mContext->GetPackageName(&name);

    CRemoteViews::New(name,
        tombstone ? GetActionTombstoneLayoutResource() : GetActionLayoutResource(),
        (IRemoteViews**)&button);

    Int32 icon;
    action->GetIcon(&icon);
    AutoPtr<ICharSequence> title;
    action->GetTitle((ICharSequence**)&title);
    button->SetTextViewCompoundDrawablesRelative(R::id::action0, icon, 0, 0, 0);
    button->SetTextViewText(R::id::action0, ProcessLegacyText(title));
    if (!tombstone) {
        button->SetOnClickPendingIntent(R::id::action0, actionIntent);
    }
    button->SetContentDescription(R::id::action0, title);
    ProcessLegacyAction(action, button);
    return button;
}

Boolean CNotificationBuilder::IsLegacy()
{
    return mColorUtil != NULL;
}

ECode CNotificationBuilder::ProcessLegacyAction(
    /* [in] */ INotificationAction* action,
    /* [in] */ IRemoteViews* button)
{
    Int32 icon;
    action->GetIcon(&icon);
    Boolean bval;
    if (!IsLegacy() || (mColorUtil->IsGrayscaleIcon(mContext, icon, &bval), bval)) {
        AutoPtr<IResources> res;
        mContext->GetResources((IResources**)&res);
        Int32 color;
        res->GetColor(R::color::notification_action_color_filter, &color);
        button->SetTextViewCompoundDrawablesRelativeColorFilter(R::id::action0, 0,
            color,
            PorterDuffMode_MULTIPLY);
    }
    return NOERROR;
}

AutoPtr<ICharSequence> CNotificationBuilder::ProcessLegacyText(
    /* [in] */ ICharSequence* charSequence)
{
    if (IsLegacy()) {
        AutoPtr<ICharSequence> seq;
        mColorUtil->InvertCharSequenceColors(charSequence, (ICharSequence**)&seq);
        return seq;
    }

    return charSequence;
}

ECode CNotificationBuilder::ProcessSmallIconAsLarge(
    /* [in] */ Int32 largeIconId,
    /* [in] */ IRemoteViews* contentView)
{
    Boolean bval;
    if (!IsLegacy() || (mColorUtil->IsGrayscaleIcon(mContext, largeIconId, &bval), bval)) {
        ApplyLargeIconBackground(contentView);
    }
    return NOERROR;
}

ECode CNotificationBuilder::ProcessLargeLegacyIcon(
    /* [in] */ IBitmap* largeIcon,
    /* [in] */ IRemoteViews* contentView)
{
    Boolean result;
    if (IsLegacy() && (mColorUtil->IsGrayscaleIcon(largeIcon, &result), result)) {
        ApplyLargeIconBackground(contentView);
    }
    else {
        RemoveLargeIconBackground(contentView);
    }
    return NOERROR;
}

ECode CNotificationBuilder::ApplyLargeIconBackground(
    /* [in] */ IRemoteViews* contentView)
{
    contentView->SetInt32(R::id::icon, String("SetBackgroundResource"),
        R::drawable::notification_icon_legacy_bg);

    contentView->SetDrawableParameters(
        R::id::icon, TRUE, -1, ResolveColor(),
        PorterDuffMode_SRC_ATOP, -1);

    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    Int32 padding;
    res->GetDimensionPixelSize(
        R::dimen::notification_large_icon_circle_padding, &padding);
    contentView->SetViewPadding(
        R::id::icon, padding, padding, padding, padding);
    return NOERROR;
}

ECode CNotificationBuilder::RemoveLargeIconBackground(
    /* [in] */ IRemoteViews* contentView)
{
    return contentView->SetInt32(R::id::icon, String("SetBackgroundResource"), 0);
}

ECode CNotificationBuilder::ProcessSmallRightIcon(
    /* [in] */ Int32 smallIconDrawableId,
    /* [in] */ IRemoteViews* contentView)
{
    Boolean bval;
    if (!IsLegacy() || (mColorUtil->IsGrayscaleIcon(mContext, smallIconDrawableId, &bval))) {
        contentView->SetDrawableParameters(R::id::right_icon, FALSE, -1,
            0xFFFFFFFF, PorterDuffMode_SRC_ATOP, -1);

        contentView->SetInt32(R::id::right_icon,
            String("SetBackgroundResource"),
            R::drawable::notification_icon_legacy_bg);

        contentView->SetDrawableParameters(
            R::id::right_icon, TRUE, -1,
            ResolveColor(), PorterDuffMode_SRC_ATOP, -1);
    }
    return NOERROR;
}

Int32 CNotificationBuilder::SanitizeColor()
{
    if (mColor != INotification::COLOR_DEFAULT) {
        mColor |= 0xFF000000; // no alpha for custom colors
    }
    return mColor;
}

Int32 CNotificationBuilder::ResolveColor()
{
    AutoPtr<IResources> res;
    mContext->GetResources((IResources**)&res);
    if (mColor == INotification::COLOR_DEFAULT) {
        Int32 color;
        res->GetColor(R::color::notification_icon_bg_color, &color);
        return color;
    }
    return mColor;
}

ECode CNotificationBuilder::BuildUnstyled(
    /* [out] */ INotification** notification)
{
    VALIDATE_NOT_NULL(notification);
    *notification = NULL;

    AutoPtr<INotification> n;
    CNotification::New((INotification**)&n);
    n->SetWhen(mWhen);
    n->SetIcon(mSmallIcon);
    n->SetIconLevel(mSmallIconLevel);
    n->SetNumber(mNumber);

    n->SetColor(SanitizeColor());

    SetBuilderContentView(n, MakeContentView());

    n->SetContentIntent(mContentIntent);
    n->SetDeleteIntent(mDeleteIntent);
    n->SetFullScreenIntent(mFullScreenIntent);
    n->SetTickerText(mTickerText);
    n->SetLargeIcon(mLargeIcon);
    n->SetSound(mSound);
    n->SetAudioStreamType(mAudioStreamType);
    n->SetAudioAttributes(mAudioAttributes);
    n->SetVibrate(mVibrate);
    n->SetLedARGB(mLedArgb);
    n->SetLedOnMS(mLedOnMs);
    n->SetLedOffMS(mLedOffMs);
    n->SetDefaults(mDefaults);

    AutoPtr<IRemoteViews> rv = MakeTickerView();
    n->SetTickerView(rv);
    rv = MakeContentView();
    n->SetContentView(rv);
    rv = MakeBigContentView();
    n->SetBigContentView(rv);

    Int32 flag = mFlags;
    n->SetFlags(flag);

    SetBuilderBigContentView(n, MakeBigContentView());
    SetBuilderHeadsUpContentView(n, MakeHeadsUpContentView());

    if (mLedOnMs != 0 || mLedOffMs != 0) {
        flag |= INotification::FLAG_SHOW_LIGHTS;
    }
    if ((mDefaults & INotification::DEFAULT_LIGHTS) != 0) {
        flag |= INotification::FLAG_SHOW_LIGHTS;
    }
    n->SetFlags(flag);

    n->SetCategory(mCategory);
    n->SetGroup(mGroupKey);
    n->SetSortKey(mSortKey);
    n->SetPriority(mPriority);

    AutoPtr<ArrayOf<INotificationAction*> > actions;
    if (mActions.IsEmpty() == FALSE) {
        actions = ArrayOf<INotificationAction*>::Alloc(mActions.GetSize());
        List<AutoPtr<INotificationAction> >::Iterator it = mActions.Begin();
        for(Int32 i = 0; it != mActions.End(); it++) {
            actions->Set(i++, *it);
        }
        n->SetActions(actions);
    }
    else {
        n->SetActions(NULL);
    }

    n->SetVisibility(mVisibility);

    if (mPublicVersion != NULL) {
        AutoPtr<INotification> ntfc;
        CNotification::New((INotification**)&ntfc);
        n->SetPublicVersion(ntfc);
        AutoPtr<INotification> pv;
        n->GetPublicVersion((INotification**)&pv);
        mPublicVersion->CloneInto(pv, TRUE);
    }
    // Note: If you're adding new fields, also update restoreFromNotitification().

    *notification = n;
    REFCOUNT_ADD(*notification);
    return NOERROR;
}

ECode CNotificationBuilder::PopulateExtras(
    /* [in] */ IBundle* extras)
{
    // Store original information used in the construction of this object
    extras->PutInt32(INotification::EXTRA_ORIGINATING_USERID, mOriginatingUserId);
    AutoPtr<IApplicationInfo> ai;
    mContext->GetApplicationInfo((IApplicationInfo**)&ai);
    extras->PutParcelable(EXTRA_REBUILD_CONTEXT_APPLICATION_INFO, IParcelable::Probe(ai));
    extras->PutCharSequence(INotification::EXTRA_TITLE, mContentTitle);
    extras->PutCharSequence(INotification::EXTRA_TEXT, mContentText);
    extras->PutCharSequence(INotification::EXTRA_SUB_TEXT, mSubText);
    extras->PutCharSequence(INotification::EXTRA_INFO_TEXT, mContentInfo);
    extras->PutInt32(INotification::EXTRA_SMALL_ICON, mSmallIcon);
    extras->PutInt32(INotification::EXTRA_PROGRESS, mProgress);
    extras->PutInt32(INotification::EXTRA_PROGRESS_MAX, mProgressMax);
    extras->PutBoolean(INotification::EXTRA_PROGRESS_INDETERMINATE, mProgressIndeterminate);
    extras->PutBoolean(INotification::EXTRA_SHOW_CHRONOMETER, mUseChronometer);
    extras->PutBoolean(INotification::EXTRA_SHOW_WHEN, mShowWhen);
    if (mLargeIcon != NULL) {
        extras->PutParcelable(INotification::EXTRA_LARGE_ICON, IParcelable::Probe(mLargeIcon));
    }
    if (!mPeople.IsEmpty()) {
        AutoPtr<ArrayOf<String> > array = ArrayOf<String>::Alloc(mPeople.GetSize());
        List<String>::Iterator it;
        Int32 i = 0;
        for (it = mPeople.Begin(); it != mPeople.End(); ++it) {
            array->Set(i++, *it);
        }
        extras->PutStringArray(INotification::EXTRA_PEOPLE, array);
    }
    // NOTE: If you're adding new extras also update RestoreFromNotification().
    return NOERROR;
}

ECode CNotificationBuilder::StripForDelivery(
    /* [in] */ INotification* n)
{
    if (!STRIP_AND_REBUILD) {
        return NOERROR;
    }

    AutoPtr<IBundle> extras;
    n->GetExtras((IBundle**)&extras);
    String templateClass;
    extras->GetString(INotification::EXTRA_TEMPLATE, &templateClass);
    // Only strip views for known Styles because we won't know how to
    // re-create them otherwise.
    AutoPtr<IClassInfo> classInfo = GetNotificationStyleClass(templateClass);
    Boolean stripViews = TextUtils::IsEmpty(templateClass) || classInfo != NULL;

    Boolean isStripped = FALSE;

    AutoPtr<IBitmap> largeIcon;
    n->GetLargeIcon((IBitmap**)&largeIcon);
    Boolean bval;
    if (largeIcon != NULL && (extras->ContainsKey(INotification::EXTRA_LARGE_ICON, &bval), bval)) {
        // TODO: Would like to check for equality here, but if the notification
        // has been cloned, we can't.
        n->SetLargeIcon(NULL);
        extras->PutBoolean(INotificationBuilder::EXTRA_REBUILD_LARGE_ICON, TRUE);
        isStripped = TRUE;
    }
    // Get rid of unmodified BuilderRemoteViews.

    AutoPtr<IRemoteViews> contentView;
    n->GetContentView((IRemoteViews**)&contentView);
    Int32 vac, sn;
    extras->GetInt32(INotificationBuilder::EXTRA_REBUILD_CONTENT_VIEW_ACTION_COUNT, -1, &vac);
    if (stripViews && INotificationBuilderRemoteViews::Probe(contentView)) {
        contentView->GetSequenceNumber(&sn);
        if (vac == sn) {
            n->SetContentView(NULL);
            extras->PutBoolean(INotificationBuilder::EXTRA_REBUILD_CONTENT_VIEW, TRUE);
            extras->Remove(INotificationBuilder::EXTRA_REBUILD_CONTENT_VIEW_ACTION_COUNT);
            isStripped = TRUE;
        }
    }

    AutoPtr<IRemoteViews> bigContentView;
    n->GetBigContentView((IRemoteViews**)&bigContentView);
    extras->GetInt32(INotificationBuilder::EXTRA_REBUILD_BIG_CONTENT_VIEW_ACTION_COUNT, -1, &vac);
    if (stripViews && INotificationBuilderRemoteViews::Probe(bigContentView)) {
        bigContentView->GetSequenceNumber(&sn);
        if (vac == sn) {
            n->SetBigContentView(NULL);
            extras->PutBoolean(INotificationBuilder::EXTRA_REBUILD_BIG_CONTENT_VIEW, TRUE);
            extras->Remove(INotificationBuilder::EXTRA_REBUILD_BIG_CONTENT_VIEW_ACTION_COUNT);
            isStripped = TRUE;
        }
    }


    AutoPtr<IRemoteViews> headsUpContentView;
    n->GetHeadsUpContentView((IRemoteViews**)&headsUpContentView);
    extras->GetInt32(INotificationBuilder::EXTRA_REBUILD_HEADS_UP_CONTENT_VIEW_ACTION_COUNT, -1, &vac);
    if (stripViews && INotificationBuilderRemoteViews::Probe(headsUpContentView)) {
        headsUpContentView->GetSequenceNumber(&sn);
        if (vac == sn) {
            n->SetHeadsUpContentView(NULL);
            extras->PutBoolean(INotificationBuilder::EXTRA_REBUILD_HEADS_UP_CONTENT_VIEW, TRUE);
            extras->Remove(INotificationBuilder::EXTRA_REBUILD_HEADS_UP_CONTENT_VIEW_ACTION_COUNT);
            isStripped = TRUE;
        }
    }

    if (isStripped) {
        extras->PutBoolean(INotificationBuilder::EXTRA_NEEDS_REBUILD, TRUE);
    }
    return NOERROR;
}

AutoPtr<INotification> CNotificationBuilder::Rebuild(
    /* [in] */ IContext* context,
    /* [in] */ INotification* n)
{
    AutoPtr<IBundle> extras;
    n->GetExtras((IBundle**)&extras);
    Boolean bval;
    extras->GetBoolean(INotificationBuilder::EXTRA_NEEDS_REBUILD, &bval);
    if (!bval) return n;
    extras->Remove(INotificationBuilder::EXTRA_NEEDS_REBUILD);

    // Re-create notification context so we can access app resources.
    AutoPtr<IParcelable> p;
    extras->GetParcelable(
        EXTRA_REBUILD_CONTEXT_APPLICATION_INFO, (IParcelable**)&p);
    AutoPtr<IApplicationInfo> applicationInfo = IApplicationInfo::Probe(p);
    AutoPtr<IContext> builderContext;
    // try {
    ECode ec = context->CreateApplicationContext(applicationInfo,
        IContext::CONTEXT_RESTRICTED, (IContext**)&builderContext);
    if (ec == (ECode)E_NAME_NOT_FOUND_EXCEPTION) {
        Logger::E(TAG, "ApplicationInfo %s not found", TO_CSTR(applicationInfo));
        builderContext = context;  // try with our context
    }

    AutoPtr<CNotificationBuilder> b;
    CNotificationBuilder::NewByFriend(builderContext, n, (CNotificationBuilder**)&b);
    AutoPtr<INotification> result;
    b->Rebuild((INotification**)&result);
    return result;
}

ECode CNotificationBuilder::Rebuild(
    /* [out] */ INotification** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    if (mRebuildNotification == NULL) {
        Logger::E(TAG, "rebuild() only valid when in 'rebuild' mode.");
        return E_ILLEGAL_STATE_EXCEPTION;

    }
    mHasThreeLines = HasThreeLines();

    AutoPtr<IBundle> extras;
    mRebuildNotification->GetExtras((IBundle**)&extras);

    Boolean bval;
    extras->GetBoolean(INotificationBuilder::EXTRA_REBUILD_LARGE_ICON, &bval);
    if (bval) {
        AutoPtr<IParcelable> p;
        extras->GetParcelable(INotification::EXTRA_LARGE_ICON, (IParcelable**)&p);
        mRebuildNotification->SetLargeIcon(IBitmap::Probe(p));
    }
    extras->Remove(INotificationBuilder::EXTRA_REBUILD_LARGE_ICON);

    extras->GetBoolean(INotificationBuilder::EXTRA_REBUILD_CONTENT_VIEW, &bval);
    if (bval) {
        SetBuilderContentView(mRebuildNotification, MakeContentView());
        if (mStyle != NULL) {
            ((NotificationStyle*)mStyle.Get())->PopulateContentView(mRebuildNotification);
        }
    }
    extras->Remove(INotificationBuilder::EXTRA_REBUILD_CONTENT_VIEW);

    extras->GetBoolean(INotificationBuilder::EXTRA_REBUILD_BIG_CONTENT_VIEW, &bval);
    if (bval) {
        SetBuilderBigContentView(mRebuildNotification, MakeBigContentView());
        if (mStyle != NULL) {
            ((NotificationStyle*)mStyle.Get())->PopulateBigContentView(mRebuildNotification);
        }
    }
    extras->Remove(INotificationBuilder::EXTRA_REBUILD_BIG_CONTENT_VIEW);

    extras->GetBoolean(INotificationBuilder::EXTRA_REBUILD_HEADS_UP_CONTENT_VIEW, &bval);
    if (bval) {
        SetBuilderHeadsUpContentView(mRebuildNotification, MakeHeadsUpContentView());
        if (mStyle != NULL) {
            ((NotificationStyle*)mStyle.Get())->PopulateHeadsUpContentView(mRebuildNotification);
        }
    }
    extras->Remove(INotificationBuilder::EXTRA_REBUILD_HEADS_UP_CONTENT_VIEW);

    mHasThreeLines = FALSE;
    *result = mRebuildNotification;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

AutoPtr<IClassInfo> CNotificationBuilder::GetNotificationStyleClass(
    /* [in] */ const String& templateClass)
{
    AutoPtr<IClassLoader> cl = ClassLoader::GetSystemClassLoader();
    Int32 pos = String("Elastos.Droid.App.CNotification").GetLength();
    AutoPtr<ArrayOf<String> > classes = ArrayOf<String>::Alloc(4);
    (*classes)[0] = String("Elastos.Droid.App.CNotificationBigTextStyle");
    (*classes)[1] = String("Elastos.Droid.App.CNotificationBigPictureStyle");
    (*classes)[2] = String("Elastos.Droid.App.CNotificationInboxStyle");
    (*classes)[3] = String("Elastos.Droid.App.CNotificationMediaStyle");
    String key;
    for (Int32 i = 0; i < classes->GetLength(); ++i) {
        key = (*classes)[i].Substring(pos);
        if (templateClass.Contains(key)) {
            AutoPtr<IClassInfo> ci;
            cl->LoadClass((*classes)[i], (IClassInfo**)&ci);
            return ci;
        }
    }
    return NULL;
}

ECode CNotificationBuilder::SetBuilderContentView(
    /* [in] */ INotification* n,
    /* [in] */ IRemoteViews* contentView)
{
    n->SetContentView(contentView);
    if (INotificationBuilderRemoteViews::Probe(contentView) != NULL) {
        Int32 ival;
        contentView->GetSequenceNumber(&ival);
        mRebuildBundle->PutInt32(
            INotificationBuilder::EXTRA_REBUILD_CONTENT_VIEW_ACTION_COUNT, ival);
    }
    return NOERROR;
}

ECode CNotificationBuilder::SetBuilderBigContentView(
    /* [in] */ INotification* n,
    /* [in] */ IRemoteViews* bigContentView)
{
    n->SetBigContentView(bigContentView);
    if (INotificationBuilderRemoteViews::Probe(bigContentView) != NULL) {
        Int32 ival;
        bigContentView->GetSequenceNumber(&ival);
        mRebuildBundle->PutInt32(
            INotificationBuilder::EXTRA_REBUILD_BIG_CONTENT_VIEW_ACTION_COUNT, ival);
    }
    return NOERROR;
}

ECode CNotificationBuilder::SetBuilderHeadsUpContentView(
    /* [in] */ INotification* n,
    /* [in] */ IRemoteViews* headsUpContentView)
{
    n->SetHeadsUpContentView(headsUpContentView);
    if (INotificationBuilderRemoteViews::Probe(headsUpContentView) != NULL) {
        Int32 ival;
        headsUpContentView->GetSequenceNumber(&ival);
        mRebuildBundle->PutInt32(
            INotificationBuilder::EXTRA_REBUILD_HEADS_UP_CONTENT_VIEW_ACTION_COUNT, ival);
    }
    return NOERROR;
}

ECode CNotificationBuilder::RestoreFromNotification(
    /* [in] */ INotification* n)
{
    // Notification fields.
    n->GetWhen(&mWhen);
    n->GetIcon(&mSmallIcon);
    n->GetIconLevel(&mSmallIconLevel);
    n->GetNumber(&mNumber);

    n->GetColor(&mColor);

    mContentView = NULL;
    n->GetContentView((IRemoteViews**)&mContentView);

    mDeleteIntent = NULL;
    n->GetDeleteIntent((IPendingIntent**)&mDeleteIntent);
    mFullScreenIntent = NULL;
    n->GetFullScreenIntent((IPendingIntent**)&mFullScreenIntent);

    mTickerText = NULL;
    n->GetTickerText((ICharSequence**)&mTickerText);
    mTickerView = NULL;
    n->GetTickerView((IRemoteViews**)&mTickerView);
    mLargeIcon = NULL;
    n->GetLargeIcon((IBitmap**)&mLargeIcon);

    mSound = NULL;
    n->GetSound((IUri**)&mSound);
    n->GetAudioStreamType(&mAudioStreamType);
    mAudioAttributes = NULL;
    n->GetAudioAttributes((IAudioAttributes**)&mAudioAttributes);
    mVibrate = NULL;
    n->GetVibrate((ArrayOf<Int64>**)&mVibrate);

    n->GetLedARGB(&mLedArgb);
    n->GetLedOnMS(&mLedOnMs);
    n->GetLedOffMS(&mLedOffMs);
    n->GetDefaults(&mDefaults);
    n->GetFlags(&mFlags);

    n->GetCategory(&mCategory);
    n->GetGroup(&mGroupKey);
    n->GetSortKey(&mSortKey);
    n->GetPriority(&mPriority);
    mActions.Clear();
    AutoPtr<ArrayOf<INotificationAction*> > actions;
    n->GetActions((ArrayOf<INotificationAction*>**)&actions);
    if (actions != NULL) {
        for (Int32 i = 0; i < actions->GetLength(); ++i) {
            AutoPtr<INotificationAction> action = (*actions)[i];
            mActions.PushBack(action);
        }
    }
    n->GetVisibility(&mVisibility);

    mPublicVersion = NULL;
    n->GetPublicVersion((INotification**)&mPublicVersion);

    // Extras.
    AutoPtr<IBundle> extras;
    n->GetExtras((IBundle**)&extras);
    extras->GetInt32(INotification::EXTRA_ORIGINATING_USERID, &mOriginatingUserId);
    mContentTitle = NULL;
    extras->GetCharSequence(INotification::EXTRA_TITLE, (ICharSequence**)&mContentTitle);
    mContentText = NULL;
    extras->GetCharSequence(INotification::EXTRA_TEXT, (ICharSequence**)&mContentText);
    mSubText = NULL;
    extras->GetCharSequence(INotification::EXTRA_SUB_TEXT, (ICharSequence**)&mSubText);
    mContentInfo = NULL;
    extras->GetCharSequence(INotification::EXTRA_INFO_TEXT, (ICharSequence**)&mContentInfo);

    extras->GetInt32(INotification::EXTRA_SMALL_ICON, &mSmallIcon);
    extras->GetInt32(INotification::EXTRA_PROGRESS, &mProgress);
    extras->GetInt32(INotification::EXTRA_PROGRESS_MAX, &mProgressMax);
    extras->GetBoolean(INotification::EXTRA_PROGRESS_INDETERMINATE, &mProgressIndeterminate);
    extras->GetBoolean(INotification::EXTRA_SHOW_CHRONOMETER, &mUseChronometer);
    extras->GetBoolean(INotification::EXTRA_SHOW_WHEN, &mShowWhen);
    Boolean bval;
    extras->ContainsKey(INotification::EXTRA_LARGE_ICON, &bval);
    if (bval) {
        AutoPtr<IParcelable> p;
        extras->GetParcelable(INotification::EXTRA_LARGE_ICON, (IParcelable**)&p);
        mLargeIcon = IBitmap::Probe(p);
    }

    extras->ContainsKey(INotification::EXTRA_PEOPLE, &bval);
    if (bval) {
        mPeople.Clear();

        AutoPtr<ArrayOf<String> > list;
        extras->GetStringArray(INotification::EXTRA_PEOPLE, (ArrayOf<String>**)&list);
        Int32 size = list->GetLength();
        for (Int32 i = 0; i < size; ++i) {
            mPeople.PushBack((*list)[i]);
        }
    }
    return NOERROR;
}


}
}
}
