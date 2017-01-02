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
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/CNotificationMediaStyle.h"
#include "elastos/droid/app/CNotificationBuilder.h"
#include "elastos/droid/app/CNotification.h"
#include "elastos/droid/widget/CRemoteViews.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::Graphics::PorterDuffMode_SRC_ATOP;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::CRemoteViews;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

const Int32 CNotificationMediaStyle::MAX_MEDIA_BUTTONS_IN_COMPACT = 3;
const Int32 CNotificationMediaStyle::MAX_MEDIA_BUTTONS = 5;

CAR_INTERFACE_IMPL(CNotificationMediaStyle, NotificationStyle, INotificationMediaStyle)

CAR_OBJECT_IMPL(CNotificationMediaStyle)

CNotificationMediaStyle::CNotificationMediaStyle()
{}

CNotificationMediaStyle::~CNotificationMediaStyle()
{}

ECode CNotificationMediaStyle::constructor()
{
    return NOERROR;
}

ECode CNotificationMediaStyle::constructor(
    /* [in] */ INotificationBuilder* builder)
{
    NotificationStyle::SetBuilder(builder);
    return NOERROR;
}

ECode CNotificationMediaStyle::SetShowActionsInCompactView(
    /* [in] */ ArrayOf<Int32>* actions)
{
    mActionsToShowInCompact = actions;
    return NOERROR;
}

ECode CNotificationMediaStyle::SetMediaSession(
    /* [in] */ IMediaSessionToken* token)
{
    mToken = token;
    return NOERROR;
}

ECode CNotificationMediaStyle::BuildStyled(
    /* [in] */ INotification* wip)
{
    NotificationStyle::BuildStyled(wip);
    String category;
    wip->GetCategory(&category);
    if (category.IsNull()) {
        wip->SetCategory(INotification::CATEGORY_TRANSPORT);
    }

    return NOERROR;
}

ECode CNotificationMediaStyle::PopulateContentView(
    /* [in] */ INotification* wip)
{
    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    return builder->SetBuilderContentView(wip, MakeMediaContentView());
}

ECode CNotificationMediaStyle::PopulateBigContentView(
    /* [in] */ INotification* wip)
{
    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    return builder->SetBuilderBigContentView(wip, MakeMediaBigContentView());
}

ECode CNotificationMediaStyle::AddExtras(
    /* [in] */ IBundle* extras)
{
    NotificationStyle::AddExtras(extras);

    if (mToken != NULL) {
        extras->PutParcelable(INotification::EXTRA_MEDIA_SESSION,
            IParcelable::Probe(mToken));
    }
    if (mActionsToShowInCompact != NULL) {
        extras->PutInt32Array(INotification::EXTRA_COMPACT_ACTIONS, mActionsToShowInCompact);
    }
    return NOERROR;
}

ECode CNotificationMediaStyle::RestoreFromExtras(
    /* [in] */ IBundle* extras)
{
    NotificationStyle::RestoreFromExtras(extras);

    Boolean bval;
    extras->ContainsKey(INotification::EXTRA_MEDIA_SESSION, &bval);
    if (bval) {
        AutoPtr<IParcelable> p;
        extras->GetParcelable(INotification::EXTRA_MEDIA_SESSION, (IParcelable**)&p);
        mToken = IMediaSessionToken::Probe(p);
    }
    extras->ContainsKey(INotification::EXTRA_COMPACT_ACTIONS, &bval);
    if (bval) {
        mActionsToShowInCompact = NULL;
        extras->GetInt32Array(INotification::EXTRA_COMPACT_ACTIONS,
            (ArrayOf<Int32>**)&mActionsToShowInCompact);
    }
    return NOERROR;
}

Boolean CNotificationMediaStyle::HasProgress()
{
    return FALSE;
}

AutoPtr<IRemoteViews> CNotificationMediaStyle::GenerateMediaActionButton(
    /* [in] */ INotificationAction* action)
{
    AutoPtr<IPendingIntent> pi;
    action->GetActionIntent((IPendingIntent**)&pi);
    Int32 icon;
    action->GetIcon(&icon);
    AutoPtr<ICharSequence> title;
    action->GetTitle((ICharSequence**)&title);

    Boolean tombstone = (pi == NULL);

    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    String pkgName;
    builder->mContext->GetPackageName(&pkgName);

    AutoPtr<IRemoteViews> button;
    CRemoteViews::New(pkgName, R::layout::notification_material_media_action, (IRemoteViews**)&button);
    button->SetImageViewResource(R::id::action0, icon);
    button->SetDrawableParameters(R::id::action0, FALSE, -1,
        0xFFFFFFFF, PorterDuffMode_SRC_ATOP, -1);
    if (!tombstone) {
        button->SetOnClickPendingIntent(R::id::action0, pi);
    }
    button->SetContentDescription(R::id::action0, title);
    return button;
}

AutoPtr<IRemoteViews> CNotificationMediaStyle::MakeMediaContentView()
{
    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    AutoPtr<IRemoteViews> view = builder->ApplyStandardTemplate(
        R::layout::notification_template_material_media, FALSE /* hasProgress */);

    using Elastos::Core::Math;

    Int32 numActions = builder->mActions.GetSize();
    Int32 N = mActionsToShowInCompact == NULL
            ? 0
            : Math::Min(mActionsToShowInCompact->GetLength(), MAX_MEDIA_BUTTONS_IN_COMPACT);
    if (N > 0) {
        view->RemoveAllViews(R::id::media_actions);
        for (Int32 i = 0; i < N; i++) {
            if (i >= numActions) {
                Logger::E("CNotificationMediaStyle",
                    "setShowActionsInCompactView: action %d out of bounds (max %d)",
                    i, numActions - 1);
                return NULL;
            }

            AutoPtr<INotificationAction> action = builder->mActions[(*mActionsToShowInCompact)[i]];
            AutoPtr<IRemoteViews> button = GenerateMediaActionButton(action);
            view->AddView(R::id::media_actions, button);
        }
    }
    StyleText(view);
    HideRightIcon(view);
    return view;
}

AutoPtr<IRemoteViews> CNotificationMediaStyle::MakeMediaBigContentView()
{
    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    Int32 actionCount = Elastos::Core::Math::Min(
        (Int32)builder->mActions.GetSize(), MAX_MEDIA_BUTTONS);
    AutoPtr<IRemoteViews> big = builder->ApplyStandardTemplate(
        GetBigLayoutResource(actionCount), FALSE /* hasProgress */);

    if (actionCount > 0) {
        big->RemoveAllViews(R::id::media_actions);
        for (Int32 i = 0; i < actionCount; i++) {
            AutoPtr<IRemoteViews> button = GenerateMediaActionButton(builder->mActions[i]);
            big->AddView(R::id::media_actions, button);
        }
    }
    StyleText(big);
    HideRightIcon(big);
    ApplyTopPadding(big);
    big->SetViewVisibility(R::id::progress, IView::GONE);
    return big;
}

Int32 CNotificationMediaStyle::GetBigLayoutResource(
    /* [in] */ Int32 actionCount)
{
    if (actionCount <= 3) {
        return R::layout::notification_template_material_big_media_narrow;
    } else {
        return R::layout::notification_template_material_big_media;
    }
}

void CNotificationMediaStyle::HideRightIcon(
    /* [in] */ IRemoteViews* contentView)
{
    contentView->SetViewVisibility(R::id::right_icon, IView::GONE);
}

void CNotificationMediaStyle::StyleText(
    /* [in] */ IRemoteViews* contentView)
{
    CNotificationBuilder* builder = (CNotificationBuilder*)mBuilder.Get();
    AutoPtr<IResources> res;
    builder->mContext->GetResources((IResources**)&res);
    Int32 primaryColor, secondaryColor;
    res->GetColor(R::color::notification_media_primary_color, &primaryColor);
    res->GetColor(R::color::notification_media_secondary_color, &secondaryColor);
    contentView->SetTextColor(R::id::title, primaryColor);
    if (builder->ShowsTimeOrChronometer()) {
        if (builder->mUseChronometer) {
            contentView->SetTextColor(R::id::chronometer, secondaryColor);
        } else {
            contentView->SetTextColor(R::id::time, secondaryColor);
        }
    }
    contentView->SetTextColor(R::id::text2, secondaryColor);
    contentView->SetTextColor(R::id::text, secondaryColor);
    contentView->SetTextColor(R::id::info, secondaryColor);
}

} // namespace App
} // namespace Droid
} // namespace Elastos
