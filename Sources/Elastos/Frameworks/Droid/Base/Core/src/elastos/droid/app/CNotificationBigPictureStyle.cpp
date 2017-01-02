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

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/CNotificationBigPictureStyle.h"
#include "elastos/droid/app/CNotification.h"
#include "elastos/droid/app/CNotificationBuilder.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CNotificationBigPictureStyle, NotificationStyle, INotificationBigPictureStyle)

CAR_OBJECT_IMPL(CNotificationBigPictureStyle)

CNotificationBigPictureStyle::CNotificationBigPictureStyle()
    : mBigLargeIconSet(FALSE)
{
}

CNotificationBigPictureStyle::~CNotificationBigPictureStyle()
{
}

ECode CNotificationBigPictureStyle::constructor()
{
    return NOERROR;
}

ECode CNotificationBigPictureStyle::constructor(
    /* [in] */ INotificationBuilder* builder)
{
    return NotificationStyle::SetBuilder(builder);
}

ECode CNotificationBigPictureStyle::SetBuilder(
    /* [in] */ INotificationBuilder* builder)
{
    return NotificationStyle::SetBuilder(builder);
}

ECode CNotificationBigPictureStyle::SetBigContentTitle(
    /* [in] */ ICharSequence* title)
{
    InternalSetBigContentTitle(CNotification::SafeCharSequence(title));
    return NOERROR;
}

ECode CNotificationBigPictureStyle::SetSummaryText(
    /* [in] */ ICharSequence* cs)
{
    InternalSetSummaryText(CNotification::SafeCharSequence(cs));
    return NOERROR;
}

ECode CNotificationBigPictureStyle::BigPicture(
    /* [in] */ IBitmap* b)
{
    mPicture = b;
    return NOERROR;
}

ECode CNotificationBigPictureStyle::BigLargeIcon(
    /* [in] */ IBitmap* b)
{
    mBigLargeIconSet = TRUE;
    mBigLargeIcon = b;
    return NOERROR;
}

AutoPtr<IRemoteViews> CNotificationBigPictureStyle::MakeBigContentView()
{
    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    AutoPtr<IRemoteViews> contentView = GetStandardView(builder->GetBigPictureLayoutResource());

    contentView->SetImageViewBitmap(R::id::big_picture, mPicture);

    ApplyTopPadding(contentView);

    Boolean twoTextLines = builder->mSubText != NULL && builder->mContentText != NULL;
    builder->AddProfileBadge(contentView,
        twoTextLines ? R::id::profile_badge_line2 : R::id::profile_badge_line3);
    return contentView;
}

ECode CNotificationBigPictureStyle::AddExtras(
    /* [in] */ IBundle* extras)
{
    NotificationStyle::AddExtras(extras);

    if (mBigLargeIconSet) {
        extras->PutParcelable(INotification::EXTRA_LARGE_ICON_BIG, IParcelable::Probe(mBigLargeIcon));
    }
    extras->PutParcelable(INotification::EXTRA_PICTURE, IParcelable::Probe(mPicture));
    return NOERROR;
}


ECode CNotificationBigPictureStyle::RestoreFromExtras(
    /* [in] */ IBundle* extras)
{
    NotificationStyle::RestoreFromExtras(extras);

    AutoPtr<IParcelable> p;
    Boolean bval;
    if (extras->ContainsKey(INotification::EXTRA_LARGE_ICON_BIG, &bval)) {
        extras->GetParcelable(INotification::EXTRA_LARGE_ICON_BIG, (IParcelable**)&p);
        mBigLargeIcon = IBitmap::Probe(p);
    }

    p = NULL;
    extras->GetParcelable(INotification::EXTRA_PICTURE, (IParcelable**)&p);
    mPicture = IBitmap::Probe(p);
    return NOERROR;
}

ECode CNotificationBigPictureStyle::PopulateBigContentView(
    /* [in] */ INotification* wip)
{
    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    return builder->SetBuilderBigContentView(wip, MakeBigContentView());
}

} // namespace App
} // namespace Droid
} // namespace Elastos
