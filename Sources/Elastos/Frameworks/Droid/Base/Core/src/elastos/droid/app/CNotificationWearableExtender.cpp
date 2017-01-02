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

#include "Elastos.Droid.View.h"
#include "elastos/droid/app/CNotificationWearableExtender.h"
#include "elastos/droid/app/CNotification.h"
#include "elastos/droid/os/CBundle.h"

using Elastos::Droid::Os::CBundle;
using Elastos::Droid::View::IGravity;
using Elastos::Utility::CArrayList;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::CCollections;
using Elastos::Core::EIID_ICloneable;

namespace Elastos {
namespace Droid {
namespace App {


/** Notification extra which contains wearable extensions */
const String CNotificationWearableExtender::EXTRA_WEARABLE_EXTENSIONS("android.wearable.EXTENSIONS");

// Keys within EXTRA_WEARABLE_OPTIONS for wearable options.
const String CNotificationWearableExtender::KEY_ACTIONS("actions");
const String CNotificationWearableExtender::KEY_FLAGS("flags");
const String CNotificationWearableExtender::KEY_DISPLAY_INTENT("displayIntent");
const String CNotificationWearableExtender::KEY_PAGES("pages");
const String CNotificationWearableExtender::KEY_BACKGROUND("background");
const String CNotificationWearableExtender::KEY_CONTENT_ICON("contentIcon");
const String CNotificationWearableExtender::KEY_CONTENT_ICON_GRAVITY("contentIconGravity");
const String CNotificationWearableExtender::KEY_CONTENT_ACTION_INDEX("contentActionIndex");
const String CNotificationWearableExtender::KEY_CUSTOM_SIZE_PRESET("customSizePreset");
const String CNotificationWearableExtender::KEY_CUSTOM_CONTENT_HEIGHT("customContentHeight");
const String CNotificationWearableExtender::KEY_GRAVITY("gravity");

// Flags bitwise-ored to mFlags
const Int32 CNotificationWearableExtender::FLAG_CONTENT_INTENT_AVAILABLE_OFFLINE = 0x1;
const Int32 CNotificationWearableExtender::FLAG_HINT_HIDE_ICON = 1 << 1;
const Int32 CNotificationWearableExtender::FLAG_HINT_SHOW_BACKGROUND_ONLY = 1 << 2;
const Int32 CNotificationWearableExtender::FLAG_START_SCROLL_BOTTOM = 1 << 3;

// Default value for flags integer
const Int32 CNotificationWearableExtender::DEFAULT_FLAGS = FLAG_CONTENT_INTENT_AVAILABLE_OFFLINE;
const Int32 CNotificationWearableExtender::DEFAULT_CONTENT_ICON_GRAVITY = IGravity::END;
const Int32 CNotificationWearableExtender::DEFAULT_GRAVITY = IGravity::BOTTOM;

CAR_INTERFACE_IMPL_3(CNotificationWearableExtender, Object, INotificationWearableExtender, INotificationExtender, ICloneable)

CAR_OBJECT_IMPL(CNotificationWearableExtender)

CNotificationWearableExtender::CNotificationWearableExtender()
    : mFlags(DEFAULT_FLAGS)
    , mContentIcon(0)
    , mContentIconGravity(DEFAULT_CONTENT_ICON_GRAVITY)
    , mContentActionIndex(INotificationWearableExtender::UNSET_ACTION_INDEX)
    , mCustomSizePreset(INotificationWearableExtender::SIZE_DEFAULT)
    , mCustomContentHeight(0)
    , mGravity(DEFAULT_GRAVITY)
{
    CArrayList::New((IArrayList**)&mActions);
    CArrayList::New((IArrayList**)&mPages);
}

ECode CNotificationWearableExtender::constructor()
{
    return NOERROR;
}

ECode CNotificationWearableExtender::constructor(
    /* [in] */ INotification* notif)
{
    AutoPtr<IBundle> wearableBundle;
    AutoPtr<IBundle> extras;
    notif->GetExtras((IBundle**)&extras);
    extras->GetBundle(EXTRA_WEARABLE_EXTENSIONS, (IBundle**)&wearableBundle);
    if (wearableBundle != NULL) {
        AutoPtr<IArrayList> actions;
        wearableBundle->GetParcelableArrayList(KEY_ACTIONS, (IArrayList**)&actions);
        if (actions != NULL) {
            mActions->AddAll(ICollection::Probe(actions));
        }

        wearableBundle->GetInt32(KEY_FLAGS, DEFAULT_FLAGS, &mFlags);
        AutoPtr<IParcelable> p;
        wearableBundle->GetParcelable(KEY_DISPLAY_INTENT, (IParcelable**)&p);
        mDisplayIntent = IPendingIntent::Probe(p);

        AutoPtr<IArrayList> pages;
        wearableBundle->GetParcelableArrayList(KEY_PAGES, (IArrayList**)&pages);
        if (actions != NULL) {
            mPages->AddAll(ICollection::Probe(pages));
        }

        p = NULL;
         wearableBundle->GetParcelable(KEY_BACKGROUND, (IParcelable**)&p);
        mBackground = IBitmap::Probe(mBackground);
        wearableBundle->GetInt32(KEY_CONTENT_ICON, &mContentIcon);
        wearableBundle->GetInt32(KEY_CONTENT_ICON_GRAVITY,
            DEFAULT_CONTENT_ICON_GRAVITY, &mContentIconGravity);
        wearableBundle->GetInt32(KEY_CONTENT_ACTION_INDEX,
            UNSET_ACTION_INDEX, &mContentActionIndex);
        wearableBundle->GetInt32(KEY_CUSTOM_SIZE_PRESET,
            SIZE_DEFAULT, &mCustomSizePreset);
        wearableBundle->GetInt32(KEY_CUSTOM_CONTENT_HEIGHT, &mCustomContentHeight);
        wearableBundle->GetInt32(KEY_GRAVITY, DEFAULT_GRAVITY, &mGravity);
    }
    return NOERROR;
}

ECode CNotificationWearableExtender::Extend(
    /* [in] */ INotificationBuilder* builder)
{
    AutoPtr<IBundle> wearableBundle;
    CBundle::New((IBundle**)&wearableBundle);

    Boolean bval;
    mActions->IsEmpty(&bval);
    if (!bval) {
        wearableBundle->PutParcelableArrayList(KEY_ACTIONS, mActions);
    }
    if (mFlags != DEFAULT_FLAGS) {
        wearableBundle->PutInt32(KEY_FLAGS, mFlags);
    }
    if (mDisplayIntent != NULL) {
        wearableBundle->PutParcelable(KEY_DISPLAY_INTENT, IParcelable::Probe(mDisplayIntent));
    }
    mPages->IsEmpty(&bval);
    if (!bval) {
        wearableBundle->PutParcelableArrayList(KEY_PAGES, mPages);
    }
    if (mBackground != NULL) {
        wearableBundle->PutParcelable(KEY_BACKGROUND, IParcelable::Probe(mBackground));
    }
    if (mContentIcon != 0) {
        wearableBundle->PutInt32(KEY_CONTENT_ICON, mContentIcon);
    }
    if (mContentIconGravity != DEFAULT_CONTENT_ICON_GRAVITY) {
        wearableBundle->PutInt32(KEY_CONTENT_ICON_GRAVITY, mContentIconGravity);
    }
    if (mContentActionIndex != UNSET_ACTION_INDEX) {
        wearableBundle->PutInt32(KEY_CONTENT_ACTION_INDEX, mContentActionIndex);
    }
    if (mCustomSizePreset != SIZE_DEFAULT) {
        wearableBundle->PutInt32(KEY_CUSTOM_SIZE_PRESET, mCustomSizePreset);
    }
    if (mCustomContentHeight != 0) {
        wearableBundle->PutInt32(KEY_CUSTOM_CONTENT_HEIGHT, mCustomContentHeight);
    }
    if (mGravity != DEFAULT_GRAVITY) {
        wearableBundle->PutInt32(KEY_GRAVITY, mGravity);
    }

    AutoPtr<IBundle> extras;
    builder->GetExtras((IBundle**)&extras);
    extras->PutBundle(EXTRA_WEARABLE_EXTENSIONS, wearableBundle);
    return NOERROR;
}

ECode CNotificationWearableExtender::Clone(
    /* [out] */ IInterface** obj)
{
    VALIDATE_NOT_NULL(obj)
    AutoPtr<CNotificationWearableExtender> that;
    CNotificationWearableExtender::NewByFriend((CNotificationWearableExtender**)&that);
    CArrayList::New(ICollection::Probe(mActions), (IArrayList**)&that->mActions);
    that->mFlags = this->mFlags;
    that->mDisplayIntent = this->mDisplayIntent;
    CArrayList::New(ICollection::Probe(mPages), (IArrayList**)&that->mPages);
    that->mBackground = this->mBackground;
    that->mContentIcon = this->mContentIcon;
    that->mContentIconGravity = this->mContentIconGravity;
    that->mContentActionIndex = this->mContentActionIndex;
    that->mCustomSizePreset = this->mCustomSizePreset;
    that->mCustomContentHeight = this->mCustomContentHeight;
    that->mGravity = this->mGravity;

    *obj = TO_IINTERFACE(that);
    REFCOUNT_ADD(*obj)
    return NOERROR;
}

ECode CNotificationWearableExtender::AddAction(
    /* [in] */ INotificationAction* action)
{
    mActions->Add(action);
    return NOERROR;
}

ECode CNotificationWearableExtender::AddActions(
    /* [in] */ IList* actions)
{
    mActions->AddAll(ICollection::Probe(actions));
    return NOERROR;
}

ECode CNotificationWearableExtender::ClearActions()
{
    mActions->Clear();
    return NOERROR;
}

ECode CNotificationWearableExtender::GetActions(
    /* [out] */ IList** actions)
{
    VALIDATE_NOT_NULL(actions)
    *actions = IList::Probe(mActions);
    REFCOUNT_ADD(*actions)
    return NOERROR;
}

ECode CNotificationWearableExtender::SetDisplayIntent(
    /* [in] */ IPendingIntent* intent)
{
    mDisplayIntent = intent;
    return NOERROR;
}

ECode CNotificationWearableExtender::GetDisplayIntent(
    /* [out] */ IPendingIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = mDisplayIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CNotificationWearableExtender::AddPage(
    /* [in] */ INotification* page)
{
    mPages->Add(page);
    return NOERROR;
}

ECode CNotificationWearableExtender::AddPages(
    /* [in] */ IList* pages)
{
    mPages->AddAll(ICollection::Probe(pages));
    return NOERROR;
}

ECode CNotificationWearableExtender::ClearPages()
{
    mPages->Clear();
    return NOERROR;
}

ECode CNotificationWearableExtender::GetPages(
    /* [out] */ IList** pages)
{
    VALIDATE_NOT_NULL(pages)
    *pages = IList::Probe(mPages);
    REFCOUNT_ADD(*pages)
    return NOERROR;
}

ECode CNotificationWearableExtender::SetBackground(
    /* [in] */ IBitmap* background)
{
    mBackground = background;
    return NOERROR;
}

ECode CNotificationWearableExtender::GetBackground(
    /* [out] */ IBitmap** background)
{
    VALIDATE_NOT_NULL(background)
    *background = mBackground;
    REFCOUNT_ADD(*background)
    return NOERROR;
}

ECode CNotificationWearableExtender::SetContentIcon(
    /* [in] */ Int32 icon)
{
    mContentIcon = icon;
    return NOERROR;
}

ECode CNotificationWearableExtender::GetContentIcon(
    /* [out] */ Int32* icon)
{
    VALIDATE_NOT_NULL(icon)
    *icon = mContentIcon;
    return NOERROR;
}

ECode CNotificationWearableExtender::SetContentIconGravity(
    /* [in] */ Int32 contentIconGravity)
{
    mContentIconGravity = contentIconGravity;
    return NOERROR;
}

ECode CNotificationWearableExtender::GetContentIconGravity(
    /* [out] */ Int32* contentIconGravity)
{
    VALIDATE_NOT_NULL(contentIconGravity)
    *contentIconGravity = mContentIconGravity;
    return NOERROR;
}

ECode CNotificationWearableExtender::SetContentAction(
    /* [in] */ Int32 actionIndex)
{
    mContentActionIndex = actionIndex;
    return NOERROR;
}

ECode CNotificationWearableExtender::GetContentAction(
    /* [out] */ Int32* actionIndex)
{
    VALIDATE_NOT_NULL(actionIndex)
    *actionIndex = mContentActionIndex;
    return NOERROR;
}

ECode CNotificationWearableExtender::SetGravity(
    /* [in] */ Int32 gravity)
{
    mGravity = gravity;
    return NOERROR;
}

ECode CNotificationWearableExtender::GetGravity(
    /* [out] */ Int32* gravity)
{
    VALIDATE_NOT_NULL(gravity)
    *gravity = mGravity;
    return NOERROR;
}

ECode CNotificationWearableExtender::SetCustomSizePreset(
    /* [in] */ Int32 sizePreset)
{
    mCustomSizePreset = sizePreset;
    return NOERROR;
}

ECode CNotificationWearableExtender::GetCustomSizePreset(
    /* [out] */ Int32* sizePreset)
{
    VALIDATE_NOT_NULL(sizePreset)
    *sizePreset = mCustomSizePreset;
    return NOERROR;
}

ECode CNotificationWearableExtender::SetCustomContentHeight(
    /* [in] */ Int32 height)
{
    mCustomContentHeight = height;
    return NOERROR;
}

ECode CNotificationWearableExtender::GetCustomContentHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height)
    *height = mCustomContentHeight;
    return NOERROR;
}

ECode CNotificationWearableExtender::SetStartScrollBottom(
    /* [in] */ Boolean startScrollBottom)
{
    SetFlag(FLAG_START_SCROLL_BOTTOM, startScrollBottom);
    return NOERROR;
}

ECode CNotificationWearableExtender::GetStartScrollBottom(
    /* [out] */ Boolean* startScrollBottom)
{
    VALIDATE_NOT_NULL(startScrollBottom)
    *startScrollBottom = (mFlags & FLAG_START_SCROLL_BOTTOM) != 0;
    return NOERROR;
}

ECode CNotificationWearableExtender::SetContentIntentAvailableOffline(
    /* [in] */ Boolean contentIntentAvailableOffline)
{
    SetFlag(FLAG_CONTENT_INTENT_AVAILABLE_OFFLINE, contentIntentAvailableOffline);
    return NOERROR;
}

ECode CNotificationWearableExtender::GetContentIntentAvailableOffline(
    /* [out] */ Boolean* contentIntentAvailableOffline)
{
    VALIDATE_NOT_NULL(contentIntentAvailableOffline)
    *contentIntentAvailableOffline = (mFlags & FLAG_CONTENT_INTENT_AVAILABLE_OFFLINE) != 0;
    return NOERROR;
}

ECode CNotificationWearableExtender::SetHintHideIcon(
    /* [in] */ Boolean hintHideIcon)
{
    SetFlag(FLAG_HINT_HIDE_ICON, hintHideIcon);
    return NOERROR;
}

ECode CNotificationWearableExtender::GetHintHideIcon(
    /* [out] */ Boolean* hintHideIcon)
{
    VALIDATE_NOT_NULL(hintHideIcon)
    *hintHideIcon = (mFlags & FLAG_HINT_HIDE_ICON) != 0;
    return NOERROR;
}

ECode CNotificationWearableExtender::SetHintShowBackgroundOnly(
    /* [in] */ Boolean hintShowBackgroundOnly)
{
    SetFlag(FLAG_HINT_SHOW_BACKGROUND_ONLY, hintShowBackgroundOnly);
    return NOERROR;
}

ECode CNotificationWearableExtender::GetHintShowBackgroundOnly(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = (mFlags & FLAG_HINT_SHOW_BACKGROUND_ONLY) != 0;
    return NOERROR;
}

void CNotificationWearableExtender::SetFlag(
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

} // namespace App
} // namespace Droid
} // namespace Elastos
