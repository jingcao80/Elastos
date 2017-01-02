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
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/app/MediaRouteButton.h"
#include "elastos/droid/internal/app/MediaRouteDialogPresenter.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/R.h"
#include "elastos/droid/view/SoundEffectConstants.h"
#include "elastos/droid/widget/Toast.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::R;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::SoundEffectConstants;
using Elastos::Droid::View::IHapticFeedbackConstants;
using Elastos::Droid::Widget::Toast;
using Elastos::Droid::Widget::IToast;
using Elastos::Droid::Internal::App::MediaRouteDialogPresenter;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace App {

//=========================================================================
//  MediaRouteButton::MyMediaRouterCallback
//=========================================================================

MediaRouteButton::MyMediaRouterCallback::MyMediaRouterCallback()
{}

ECode MediaRouteButton::MyMediaRouterCallback::constructor(
    /* [in] */ IMediaRouteButton* provider)
{
    assert(0 && "TODO");
    // CMediaRouter::SimpleCallback::constructor();

    AutoPtr<IWeakReferenceSource> wrs = IWeakReferenceSource::Probe(provider);
    wrs->GetWeakReference((IWeakReference**)&mProviderWeak);
    return NOERROR;
}

ECode MediaRouteButton::MyMediaRouterCallback::OnRouteAdded(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CallRefreshRoute();
}

ECode MediaRouteButton::MyMediaRouterCallback::OnRouteRemoved(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CallRefreshRoute();
}

ECode MediaRouteButton::MyMediaRouterCallback::OnRouteChanged(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CallRefreshRoute();
}

ECode MediaRouteButton::MyMediaRouterCallback::OnRouteSelected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CallRefreshRoute();
}

ECode MediaRouteButton::MyMediaRouterCallback::OnRouteUnselected(
    /* [in] */ IMediaRouter* router,
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info)
{
    return CallRefreshRoute();
}

ECode MediaRouteButton::MyMediaRouterCallback::OnRouteGrouped(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ IMediaRouterRouteGroup* group,
    /* [in] */ Int32 index)
{
    return CallRefreshRoute();
}

ECode MediaRouteButton::MyMediaRouterCallback::OnRouteUngrouped(
    /* [in] */ IMediaRouter* router,
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ IMediaRouterRouteGroup* group)
{
    return CallRefreshRoute();
}

ECode MediaRouteButton::MyMediaRouterCallback::CallRefreshRoute()
{
    AutoPtr<IInterface> obj;
    mProviderWeak->Resolve(EIID_IInterface, (IInterface**)&obj);
    if (obj) {
        MediaRouteButton* mb = (MediaRouteButton*)IMediaRouteButton::Probe(obj);
        mb->RefreshRoute();
    }
    return NOERROR;
}

//=========================================================================
//  MediaRouteButton
//=========================================================================

static AutoPtr<ArrayOf<Int32> > InitArrayOf(
    /* [in] */ Int32 value)
{
    AutoPtr<ArrayOf<Int32> >  array = ArrayOf<Int32>::Alloc(1);
    array->Set(0, value);
    return array;
}

const AutoPtr<ArrayOf<Int32> > MediaRouteButton::CHECKED_STATE_SET = InitArrayOf(R::attr::state_checked);
const AutoPtr<ArrayOf<Int32> > MediaRouteButton::ACTIVATED_STATE_SET = InitArrayOf(R::attr::state_activated);


CAR_INTERFACE_IMPL(MediaRouteButton, Elastos::Droid::View::View, IMediaRouteButton)

MediaRouteButton::MediaRouteButton()
    : mRouteTypes(0)
    , mAttachedToWindow(FALSE)
    , mCheatSheetEnabled(FALSE)
    , mIsConnecting(FALSE)
    , mMinWidth(0)
    , mMinHeight(0)
{
}

MediaRouteButton::~MediaRouteButton()
{}


ECode MediaRouteButton::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode MediaRouteButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::mediaRouteButtonStyle);
}

ECode MediaRouteButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode MediaRouteButton::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    Elastos::Droid::View::View::constructor(context, attrs, defStyleAttr, defStyleRes);

    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::MEDIA_ROUTER_SERVICE, (IInterface**)&obj);
    mRouter = IMediaRouter::Probe(obj);
    AutoPtr<MyMediaRouterCallback> cb = new MyMediaRouterCallback();
    cb->constructor(this);
    mCallback = (IMediaRouterCallback*)cb.Get();

    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::MediaRouteButton);

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a);

    AutoPtr<IDrawable> drawable;
    a->GetDrawable(
        R::styleable::MediaRouteButton_externalRouteEnabledDrawable, (IDrawable**)&drawable);
    SetRemoteIndicatorDrawable(drawable);
    a->GetDimensionPixelSize(R::styleable::MediaRouteButton_minWidth, 0, &mMinWidth);
    a->GetDimensionPixelSize(R::styleable::MediaRouteButton_minHeight, 0, &mMinHeight);
    Int32 routeTypes;
    a->GetInteger(R::styleable::MediaRouteButton_mediaRouteTypes,
        IMediaRouter::ROUTE_TYPE_LIVE_AUDIO, &routeTypes);
    a->Recycle();

    SetClickable(TRUE);
    SetLongClickable(TRUE);

    SetRouteTypes(routeTypes);
    return NOERROR;
}

ECode MediaRouteButton::GetRouteTypes(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return mRouteTypes;
}

ECode MediaRouteButton::SetRouteTypes(
    /* [in] */ Int32 types)
{
    if (mRouteTypes != types) {
        if (mAttachedToWindow && mRouteTypes != 0) {
            mRouter->RemoveCallback(mCallback);
        }

        mRouteTypes = types;

        if (mAttachedToWindow && types != 0) {
            mRouter->AddCallback(types, mCallback,
                IMediaRouter::CALLBACK_FLAG_PASSIVE_DISCOVERY);
        }

        RefreshRoute();
    }
    return NOERROR;
}

ECode MediaRouteButton::SetExtendedSettingsClickListener(
    /* [in] */ IViewOnClickListener* listener)
{
    mExtendedSettingsClickListener = listener;
    return NOERROR;
}

ECode MediaRouteButton::ShowDialog()
{
    ShowDialogInternal();
    return NOERROR;
}

Boolean MediaRouteButton::ShowDialogInternal()
{
    if (!mAttachedToWindow) {
        return FALSE;
    }

    AutoPtr<IActivity> activity;
    GetActivity((IActivity**)&activity);
    AutoPtr<IDialogFragment> f = MediaRouteDialogPresenter::ShowDialogFragment(activity,
        mRouteTypes, mExtendedSettingsClickListener);
    return f != NULL;
}

ECode MediaRouteButton::GetActivity(
     /* [out] */ IActivity** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    // Gross way of unwrapping the Activity so we can get the FragmentManager
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    while (IContextWrapper::Probe(context) != NULL) {
        if (IActivity::Probe(context) != NULL) {
            *result = IActivity::Probe(context);
            REFCOUNT_ADD(*result)
            return NOERROR;
        }

        AutoPtr<IContextWrapper> cw = IContextWrapper::Probe(context);
        context = NULL;
        cw->GetBaseContext((IContext**)&context);
    }

    Logger::E("MediaRouteButton", "The MediaRouteButton's Context is not an Activity.");
    return E_ILLEGAL_STATE_EXCEPTION;
}

void MediaRouteButton::SetCheatSheetEnabled(
    /* [in] */ Boolean enable)
{
    mCheatSheetEnabled = enable;
}

ECode MediaRouteButton::PerformClick(
    /* [out] */ Boolean* result)
{
    // Send the appropriate accessibility events and call listeners
    Boolean handled;
    Elastos::Droid::View::View::PerformClick(&handled);
    if (!handled) {
        PlaySoundEffect(SoundEffectConstants::CLICK);
    }
    *result = ShowDialogInternal() || handled;
    return NOERROR;
}

ECode MediaRouteButton::PerformLongClick(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    Boolean bval;
    Elastos::Droid::View::View::PerformLongClick(&bval);
    if (bval) {
        *result = TRUE;
        return NOERROR;
    }

    if (!mCheatSheetEnabled) {
        return NOERROR;
    }

    AutoPtr<ICharSequence> contentDesc;
    GetContentDescription((ICharSequence**)&contentDesc);
    if (TextUtils::IsEmpty(contentDesc)) {
        // Don't show the cheat sheet if we have no description
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> > screenPos = ArrayOf<Int32>::Alloc(2);
    AutoPtr<IRect> displayFrame;
    CRect::New((IRect**)&displayFrame);
    GetLocationOnScreen(screenPos);
    GetWindowVisibleDisplayFrame(displayFrame);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    Int32 width, height;
    GetWidth(&width);
    GetHeight(&height);
    Int32 midy = (*screenPos)[1] + height / 2;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> dm;
    res->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Int32 screenWidth;
    dm->GetWidthPixels(&screenWidth);

    AutoPtr<IToast> cheatSheet;
    Toast::MakeText(context, contentDesc, IToast::LENGTH_SHORT, (IToast**)&cheatSheet);
    displayFrame->GetHeight(&height);
    if (midy < height) {
        // Show along the top; follow action buttons
        cheatSheet->SetGravity(IGravity::TOP | IGravity::END,
                screenWidth - (*screenPos)[0] - width / 2, height);
    } else {
        // Show along the bottom center
        cheatSheet->SetGravity(IGravity::BOTTOM | IGravity::CENTER_HORIZONTAL, 0, height);
    }
    cheatSheet->Show();
    return PerformHapticFeedback(IHapticFeedbackConstants::LONG_PRESS, result);
}

ECode MediaRouteButton::OnCreateDrawableState(
    /* [in] */ Int32 extraSpace,
    /* [out, callee] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    AutoPtr<ArrayOf<Int32> > drawableState;
    Elastos::Droid::View::View::OnCreateDrawableState(extraSpace + 1, (ArrayOf<Int32>**)&drawableState);

    // Technically we should be handling this more completely, but these
    // are implementation details here. Checked is used to express the connecting
    // drawable state and it's mutually exclusive with activated for the purposes
    // of state selection here.
    if (mIsConnecting) {
        MergeDrawableStates(drawableState, CHECKED_STATE_SET);
    }
    else if (mRemoteActive) {
        MergeDrawableStates(drawableState, ACTIVATED_STATE_SET);
    }
    *result = drawableState;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode MediaRouteButton::DrawableStateChanged()
{
    Elastos::Droid::View::View::DrawableStateChanged();

    if (mRemoteIndicator != NULL) {
        AutoPtr<ArrayOf<Int32> > myDrawableState;
        GetDrawableState((ArrayOf<Int32>**)&myDrawableState);
        Boolean bval;
        mRemoteIndicator->SetState(myDrawableState, &bval);
        Invalidate();
    }
    return NOERROR;
}

void MediaRouteButton::SetRemoteIndicatorDrawable(
    /* [in] */ IDrawable* d)
{
    if (mRemoteIndicator != NULL) {
        mRemoteIndicator->SetCallback(NULL);
        UnscheduleDrawable(mRemoteIndicator);
    }
    mRemoteIndicator = d;
    if (d != NULL) {
        AutoPtr<ArrayOf<Int32> > myDrawableState;
        GetDrawableState((ArrayOf<Int32>**)&myDrawableState);

        d->SetCallback(this);
        Boolean bval;
        d->SetState(myDrawableState, &bval);
        Int32 visibility;
        GetVisibility(&visibility);
        d->SetVisible(visibility == IView::VISIBLE, FALSE, &bval);
    }

    RefreshDrawableState();
}

Boolean MediaRouteButton::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    return Elastos::Droid::View::View::VerifyDrawable(who) || who == mRemoteIndicator;
}

ECode MediaRouteButton::JumpDrawablesToCurrentState()
{
    Elastos::Droid::View::View::JumpDrawablesToCurrentState();

    if (mRemoteIndicator != NULL) {
        mRemoteIndicator->JumpToCurrentState();
    }
    return NOERROR;
}

ECode MediaRouteButton::SetVisibility(
    /* in] */ Int32 visibility)
{
    Elastos::Droid::View::View::SetVisibility(visibility);

    if (mRemoteIndicator != NULL) {
        Int32 visibility;
        GetVisibility(&visibility);
        Boolean bval;
        mRemoteIndicator->SetVisible(visibility == IView::VISIBLE, FALSE, &bval);
    }
    return NOERROR;
}

ECode MediaRouteButton::OnAttachedToWindow()
{
    Elastos::Droid::View::View::OnAttachedToWindow();

    mAttachedToWindow = TRUE;
    if (mRouteTypes != 0) {
        mRouter->AddCallback(mRouteTypes, mCallback,
            IMediaRouter::CALLBACK_FLAG_PASSIVE_DISCOVERY);
    }
    RefreshRoute();
    return NOERROR;
}

ECode MediaRouteButton::OnDetachedFromWindow()
{
    mAttachedToWindow = FALSE;
    if (mRouteTypes != 0) {
        mRouter->RemoveCallback(mCallback);
    }

    return Elastos::Droid::View::View::OnDetachedFromWindow();
}

ECode MediaRouteButton::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    using Elastos::Droid::View::View;
    Int32 widthSize, heightSize, widthMode, heightMode;
    widthSize = View::MeasureSpec::GetSize(widthMeasureSpec);
    heightSize = View::MeasureSpec::GetSize(heightMeasureSpec);
    widthMode = View::MeasureSpec::GetMode(widthMeasureSpec);
    heightMode = View::MeasureSpec::GetMode(heightMeasureSpec);

    using Elastos::Core::Math;

    Int32 iw = 0, ih = 0;
    if (mRemoteIndicator != NULL) {
        mRemoteIndicator->GetIntrinsicWidth(&iw);
        mRemoteIndicator->GetIntrinsicHeight(&ih);
    }
    Int32 minWidth = Math::Max(mMinWidth, iw);
    Int32 minHeight = Math::Max(mMinHeight, ih);

    Int32 pl, pt, pr, pb;
    GetPadding(&pl, &pt, &pr, &pb);

    Int32 width;
    switch (widthMode) {
        case Elastos::Droid::View::View::MeasureSpec::EXACTLY:
            width = widthSize;
            break;
        case Elastos::Droid::View::View::MeasureSpec::AT_MOST:
            width = Math::Min(widthSize, minWidth + pl + pr);
            break;
        default:
        case Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED:
            width = minWidth + pl + pr;
            break;
    }

    Int32 height;
    switch (heightMode) {
        case Elastos::Droid::View::View::MeasureSpec::EXACTLY:
            height = heightSize;
            break;
        case Elastos::Droid::View::View::MeasureSpec::AT_MOST:
            height = Math::Min(heightSize, minHeight + pt + pb);
            break;
        default:
        case Elastos::Droid::View::View::MeasureSpec::UNSPECIFIED:
            height = minHeight + pt + pb;
            break;
    }

    SetMeasuredDimension(width, height);
    return NOERROR;
}

void MediaRouteButton::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    Elastos::Droid::View::View::OnDraw(canvas);

    if (mRemoteIndicator == NULL) return;

    Int32 pl, pt, pr, pb;
    GetPadding(&pl, &pt, &pr, &pb);

    Int32 w, h;
    GetWidth(&w);
    GetHeight(&h);
    Int32 left = pl;
    Int32 right = w - pr;
    Int32 top = pt;
    Int32 bottom = h - pb;

    Int32 drawWidth, drawHeight;
    mRemoteIndicator->GetIntrinsicWidth(&drawWidth);
    mRemoteIndicator->GetIntrinsicHeight(&drawHeight);
    Int32 drawLeft = left + (right - left - drawWidth) / 2;
    Int32 drawTop = top + (bottom - top - drawHeight) / 2;

    mRemoteIndicator->SetBounds(drawLeft, drawTop,
        drawLeft + drawWidth, drawTop + drawHeight);
    mRemoteIndicator->Draw(canvas);
}

void MediaRouteButton::RefreshRoute()
{
    if (mAttachedToWindow) {
        AutoPtr<IMediaRouterRouteInfo> route;
        mRouter->GetSelectedRoute((IMediaRouterRouteInfo**)&route);
        Boolean isRemote = FALSE;
        route->IsDefault(&isRemote);
        if (isRemote) {
            route->MatchesTypes(mRouteTypes, &isRemote);
        }
        Boolean isConnecting = FALSE;
        if (isRemote) {
            route->IsConnecting(&isConnecting);
        }

        Boolean needsRefresh = FALSE;
        if (mRemoteActive != isRemote) {
            mRemoteActive = isRemote;
            needsRefresh = TRUE;
        }
        if (mIsConnecting != isConnecting) {
            mIsConnecting = isConnecting;
            needsRefresh = TRUE;
        }

        if (needsRefresh) {
            RefreshDrawableState();
        }

        Boolean bval;
        mRouter->IsRouteAvailable(mRouteTypes,
            IMediaRouter::AVAILABILITY_FLAG_IGNORE_DEFAULT_ROUTE, &bval);
        SetEnabled(bval);
    }
}

} // namespace App
} // namespace Droid
} // namespace Elastos
