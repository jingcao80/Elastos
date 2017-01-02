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

#include "elastos/droid/widget/Switch.h"
#include "elastos/droid/animation/ObjectAnimator.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/Typeface.h"
#include "elastos/droid/graphics/Insets.h"
#include "elastos/droid/text/CStaticLayout.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/CTextPaint.h"
#include "elastos/droid/text/CLayoutHelper.h"
#include "elastos/droid/text/method/CAllCapsTransformationMethod.h"
#include "elastos/droid/utility/MathUtils.h"
#include "elastos/droid/view/CViewConfiguration.h"
#include "elastos/droid/view/CMotionEvent.h"
#include "elastos/droid/content/res/CTypedArray.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::Animation::ObjectAnimator;
using Elastos::Droid::Content::Res::ICompatibilityInfo;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::Typeface;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::Insets;
using Elastos::Droid::Graphics::Drawable::EIID_IDrawableCallback;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::CTextPaint;
using Elastos::Droid::Text::IStaticLayout;
using Elastos::Droid::Text::CStaticLayout;
using Elastos::Droid::Text::ILayoutHelper;
using Elastos::Droid::Text::CLayoutHelper;
using Elastos::Droid::Text::ALIGN_NORMAL;
using Elastos::Droid::Text::Method::CAllCapsTransformationMethod;
using Elastos::Droid::Text::Method::IAllCapsTransformationMethod;
using Elastos::Droid::Utility::IProperty;
using Elastos::Droid::Utility::MathUtils;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IViewConfiguration;
using Elastos::Droid::View::CViewConfiguration;
using Elastos::Droid::View::CMotionEvent;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::Content::Res::CTypedArray;
using Elastos::Droid::Content::Res::CAssetManager;
using Elastos::Droid::R;
using Elastos::Core::IFloat;
using Elastos::Core::CFloat;
using Elastos::Core::CString;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Widget {

static AutoPtr<ArrayOf<Int32> > InitArray()
{
    AutoPtr< ArrayOf<Int32> > temp = ArrayOf<Int32>::Alloc(1);
    (*temp)[0] = R::attr::state_checked;
    return temp;
}

static AutoPtr<IFloatProperty> GetThumbPos()
{
    AutoPtr<IFloatProperty> temp = new Switch::SwitchProperty(String("thumbPos"));
    return temp;
}

const String Switch::SWITCH_NAME("Switch");
const Int32 Switch::THUMB_ANIMATION_DURATION = 250;
const Int32 Switch::TOUCH_MODE_IDLE;
const Int32 Switch::TOUCH_MODE_DOWN;
const Int32 Switch::TOUCH_MODE_DRAGGING;
const Int32 Switch::SANS;
const Int32 Switch::SERIF;
const Int32 Switch::MONOSPACE;
const AutoPtr<ArrayOf<Int32> > Switch::CHECKED_STATE_SET = InitArray();
AutoPtr<IFloatProperty> Switch::THUMB_POS = GetThumbPos();

CAR_INTERFACE_IMPL(Switch, CompoundButton, ISwitch)

Switch::Switch()
    : mThumbTextPadding(0)
    , mSwitchMinWidth(0)
    , mSwitchPadding(0)
    , mSplitTrack(FALSE)
    , mShowText(FALSE)
    , mTouchMode(0)
    , mTouchSlop(0)
    , mTouchX(0.0f)
    , mTouchY(0.0f)
    , mMinFlingVelocity(0)
    , mThumbPosition(0.0f)
    , mSwitchWidth(0)
    , mSwitchHeight(0)
    , mThumbWidth(0)
    , mSwitchLeft(0)
    , mSwitchTop(0)
    , mSwitchRight(0)
    , mSwitchBottom(0)
{
    mVelocityTracker = VelocityTracker::Obtain();
    CRect::New((IRect**)&mTempRect);
}

ECode Switch::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode Switch::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::switchStyle);
}

ECode Switch::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode Switch::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    CompoundButton::constructor(context, attrs, defStyleAttr, defStyleRes);

    CTextPaint::New(IPaint::ANTI_ALIAS_FLAG, (ITextPaint**)&mTextPaint);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> metrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);
    mTextPaint->SetDensity(density);
    AutoPtr<ICompatibilityInfo> info;
    res->GetCompatibilityInfo((ICompatibilityInfo**)&info);
    Float applicationScale = 0;
    info->GetApplicationScale(&applicationScale);
    IPaint::Probe(mTextPaint)->SetCompatibilityScaling(applicationScale);

    AutoPtr< ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::Switch);

    AutoPtr<ITypedArray> a;
    FAIL_RETURN(context->ObtainStyledAttributes(attrs, attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&a));

    a->GetDrawable(R::styleable::Switch_thumb, (IDrawable**)&mThumbDrawable);
    if (mThumbDrawable != NULL) {
        mThumbDrawable->SetCallback(this);
    }
    a->GetDrawable(R::styleable::Switch_track, (IDrawable**)&mTrackDrawable);
    if (mTrackDrawable != NULL) {
        mTrackDrawable->SetCallback(this);
    }

    a->GetText(R::styleable::Switch_textOn, (ICharSequence**)&mTextOn);
    a->GetText(R::styleable::Switch_textOff, (ICharSequence**)&mTextOff);
    a->GetBoolean(R::styleable::Switch_showText, TRUE, &mShowText);
    a->GetDimensionPixelSize(R::styleable::Switch_thumbTextPadding, 0, &mThumbTextPadding);
    a->GetDimensionPixelSize(R::styleable::Switch_switchMinWidth, 0, &mSwitchMinWidth);
    a->GetDimensionPixelSize(R::styleable::Switch_switchPadding, 0, &mSwitchPadding);
    a->GetBoolean(R::styleable::Switch_splitTrack, FALSE, &mSplitTrack);

    Int32 appearance = 0;
    a->GetResourceId(R::styleable::Switch_switchTextAppearance, 0, &appearance);

    if(appearance != 0) {
        SetSwitchTextAppearance(context, appearance);
    }
    a->Recycle();

    AutoPtr<IViewConfiguration> config = CViewConfiguration::Get(context);
    config->GetScaledTouchSlop(&mTouchSlop);
    config->GetScaledMinimumFlingVelocity(&mMinFlingVelocity);

    RefreshDrawableState();
    Boolean result;
    IsChecked(&result);
    SetChecked(result);

    return NOERROR;
}

ECode Switch::SetSwitchTextAppearance(
    /* [in] */ IContext* context,
    /* [in] */ Int32 resid)
{
    AutoPtr< ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::TextAppearance);
    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(resid, attrIds, (ITypedArray**)&a);

    AutoPtr<IColorStateList> colors;
    a->GetColorStateList(R::styleable::TextAppearance_textColor, (IColorStateList**)&colors);

    if (colors) {
        mTextColors = colors;
    }
    else {
        GetTextColors((IColorStateList**)&mTextColors);
    }

    Int32 ts = 0;
    a->GetDimensionPixelSize(R::styleable::TextAppearance_textSize, 0, &ts);
    if (ts != 0) {
        Float textSize = 0;
        IPaint* textPaint = IPaint::Probe(mTextPaint);
        textPaint->GetTextSize(&textSize);
        if (ts != textSize) {
            textPaint->SetTextSize(ts);
            RequestLayout();
        }
    }

    Int32 typefaceIndex = 0, styleIndex = 0;

    a->GetInt32(R::styleable::TextAppearance_typeface, -1, &typefaceIndex);
    a->GetInt32(R::styleable::TextAppearance_textStyle, -1, &styleIndex);

    SetSwitchTypefaceByIndex(typefaceIndex, styleIndex);

    Boolean allCaps = FALSE;
    a->GetBoolean(R::styleable::TextAppearance_textAllCaps, FALSE, &allCaps);
    if (allCaps) {
        AutoPtr<IAllCapsTransformationMethod> atm;
        AutoPtr<IContext> ctx;
        GetContext((IContext**)&ctx);
        CAllCapsTransformationMethod::New(ctx, (IAllCapsTransformationMethod**)&atm);
        mSwitchTransformationMethod = ITransformationMethod2::Probe(atm);
        mSwitchTransformationMethod->SetLengthChangesAllowed(TRUE);
    }
    else {
        mSwitchTransformationMethod = NULL;
    }

    a->Recycle();

    return NOERROR;
}

void Switch::SetSwitchTypefaceByIndex(
    /* [in] */ Int32 typefaceIndex,
    /* [in] */ Int32 styleIndex)
{
    AutoPtr<ITypeface> tf;
    switch (typefaceIndex) {
        case SANS:
            tf = Typeface::SANS_SERIF;
            break;
        case SERIF:
            tf = Typeface::SERIF;
            break;
        case MONOSPACE:
            tf = Typeface::MONOSPACE;
            break;
    }

    SetSwitchTypeface(tf, styleIndex);
}

ECode Switch::SetSwitchTypeface(
    /* [in] */ ITypeface* tf,
    /* [in] */ Int32 style)
{
    IPaint* textPaint = IPaint::Probe(mTextPaint);
    if (style > 0) {
        AutoPtr<ITypeface> typeface;
        if (!tf) {
            Typeface::DefaultFromStyle(style, (ITypeface**)&typeface);
            tf = typeface;
        }
        else {
            Typeface::Create(tf, style, (ITypeface**)&typeface);
            tf = typeface;
        }

        SetSwitchTypeface(tf);
        // now compute what (if any) algorithmic styling is needed
        Int32 typefaceStyle = 0;
        if (tf != NULL) {
            tf->GetStyle(&typefaceStyle);
        }
        Int32 need = style & ~typefaceStyle;
        textPaint->SetFakeBoldText((need & ITypeface::BOLD) != 0);
        textPaint->SetTextSkewX((need & ITypeface::ITALIC) != 0 ? -0.25f : 0);
    }
    else {
        textPaint->SetFakeBoldText(FALSE);
        textPaint->SetTextSkewX(0);
        SetSwitchTypeface(tf);
    }
    return NOERROR;
}

ECode Switch::SetSwitchTypeface(
    /* [in] */ ITypeface* tf)
{
    IPaint* textPaint = IPaint::Probe(mTextPaint);
    AutoPtr<ITypeface> face;
    textPaint->GetTypeface((ITypeface**)&face);
    if (face.Get() != tf) {
        textPaint->SetTypeface(tf);

        RequestLayout();
        Invalidate();
    }
    return NOERROR;
}

ECode Switch::SetSwitchPadding(
    /* [in] */ Int32 pixels)
{
    mSwitchPadding = pixels;
    RequestLayout();
    return NOERROR;
}

ECode Switch::GetSwitchPadding(
    /* [out] */ Int32* padding)
{
    VALIDATE_NOT_NULL(padding)
    *padding = mSwitchPadding;
    return NOERROR;
}

ECode Switch::SetSwitchMinWidth(
    /* [in] */ Int32 pixels)
{
    mSwitchMinWidth = pixels;
    RequestLayout();
    return NOERROR;
}

ECode Switch::GetSwitchMinWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width)
    *width = mSwitchMinWidth;
    return NOERROR;
}

ECode Switch::SetThumbTextPadding(
    /* [in] */ Int32 pixels)
{
    mThumbTextPadding = pixels;
    RequestLayout();
    return NOERROR;
}

ECode Switch::GetThumbTextPadding(
    /* [out] */ Int32* padding)
{
    VALIDATE_NOT_NULL(padding)
    *padding = mThumbTextPadding;
    return NOERROR;
}

ECode Switch::SetTrackDrawable(
    /* [in] */ IDrawable* track)
{
    if (mTrackDrawable != NULL) {
        mTrackDrawable->SetCallback(NULL);
    }
    mTrackDrawable = track;
    if (track != NULL) {
        track->SetCallback(this);
    }
    RequestLayout();
    return NOERROR;
}

ECode Switch::SetTrackResource(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IDrawable> drawable;
    context->GetDrawable(resId, (IDrawable**)&drawable);
    SetTrackDrawable(drawable);
    return NOERROR;
}

ECode Switch::GetTrackDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    *drawable = mTrackDrawable;
    REFCOUNT_ADD(*drawable)
    return NOERROR;
}

ECode Switch::SetThumbDrawable(
    /* [in] */ IDrawable* thumb)
{
    if (mThumbDrawable != NULL) {
        mThumbDrawable->SetCallback(NULL);
    }
    mThumbDrawable = thumb;
    if (thumb != NULL) {
        thumb->SetCallback(this);
    }
    RequestLayout();
    return NOERROR;
}

ECode Switch::SetThumbResource(
    /* [in] */ Int32 resId)
{
    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IDrawable> drawable;
    context->GetDrawable(resId, (IDrawable**)&drawable);
    SetThumbDrawable(drawable);
    return NOERROR;
}

ECode Switch::GetThumbDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable)
    *drawable = mThumbDrawable;
    REFCOUNT_ADD(*drawable)
    return NOERROR;
}

/**
 * Specifies whether the track should be split by the thumb. When true,
 * the thumb's optical bounds will be clipped out of the track drawable,
 * then the thumb will be drawn into the resulting gap.
 *
 * @param splitTrack Whether the track should be split by the thumb
 *
 * @attr ref android.R.styleable#Switch_splitTrack
 */
ECode Switch::SetSplitTrack(
    /* [in] */ Boolean splitTrack)
{
    mSplitTrack = splitTrack;
    Invalidate();
    return NOERROR;
}

/**
 * Returns whether the track should be split by the thumb.
 *
 * @attr ref android.R.styleable#Switch_splitTrack
 */
ECode Switch::GetSplitTrack(
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = mSplitTrack;
    return NOERROR;
}

ECode Switch::GetTextOn(
    /* [out] */ ICharSequence** c)
{
    VALIDATE_NOT_NULL(c)
    *c = mTextOn;
    REFCOUNT_ADD(*c)
    return NOERROR;
}

ECode Switch::SetTextOn(
    /* [in] */ ICharSequence* textOn)
{
    mTextOn = textOn;
    RequestLayout();
    return NOERROR;
}

ECode Switch::GetTextOff(
    /* [out] */ ICharSequence** off)
{
    VALIDATE_NOT_NULL(off)
    *off = mTextOff;
    REFCOUNT_ADD(*off)
    return NOERROR;
}

ECode Switch::SetTextOff(
    /* [in] */ ICharSequence* textOff)
{
    mTextOff = textOff;
    RequestLayout();
    return NOERROR;
}

/**
 * Sets whether the on/off text should be displayed.
 *
 * @param showText {@code true} to display on/off text
 * @attr ref android.R.styleable#Switch_showText
 */
ECode Switch::SetShowText(
    /* [in] */ Boolean showText)
{
    if (mShowText != showText) {
        mShowText = showText;
        RequestLayout();
    }
    return NOERROR;
}

/**
 * @return whether the on/off text should be displayed
 * @attr ref android.R.styleable#Switch_showText
 */
ECode Switch::GetShowText(
    /* [out] */ Boolean* text)
{
    VALIDATE_NOT_NULL(text)
    *text = mShowText;
    return NOERROR;
}

ECode Switch::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    if (mShowText) {
        if (!mOnLayout) {
            mOnLayout = MakeLayout(mTextOn);
        }
        if (!mOffLayout) {
            mOffLayout = MakeLayout(mTextOff);
        }
    }

    AutoPtr<IRect> padding = mTempRect;
    CRect* temp = (CRect*)padding.Get();
    Int32 thumbWidth = 0;
    Int32 thumbHeight = 0;
    if (mThumbDrawable != NULL) {
        // Cached thumb width does not include padding.
        Boolean isGetPadding;
        mThumbDrawable->GetPadding(padding, &isGetPadding);
        mThumbDrawable->GetIntrinsicWidth(&thumbWidth);
        thumbWidth -=  (temp->mLeft + temp->mRight);
        mThumbDrawable->GetIntrinsicHeight(&thumbHeight);
    }

    using Elastos::Core::Math;

    Int32 maxTextWidth = 0;
    if (mShowText) {
        Int32 onLayoutWidth, offLayoutWidth;
        mOnLayout->GetWidth(&onLayoutWidth);
        mOffLayout->GetWidth(&offLayoutWidth);
        maxTextWidth = Math::Max(onLayoutWidth, offLayoutWidth)
                + mThumbTextPadding * 2;
    }

    mThumbWidth = Math::Max(maxTextWidth, thumbWidth);

    Int32 trackHeight = 0;
    if (mTrackDrawable != NULL) {
        Boolean isGetPadding;
        mTrackDrawable->GetPadding(padding, &isGetPadding);
        mTrackDrawable->GetIntrinsicHeight(&trackHeight);
    }
    else {
        padding->SetEmpty();
    }

    // Adjust left and right padding to ensure there's enough room for the
    // thumb's padding (when present).
    Int32 paddingLeft = temp->mLeft;
    Int32 paddingRight = temp->mRight;
    if (mThumbDrawable != NULL) {
        AutoPtr<IInsets> inset;
        mThumbDrawable->GetOpticalInsets((IInsets**)&inset);
        Insets* in = (Insets*)inset.Get();
        paddingLeft = Math::Max(paddingLeft, in->mLeft);
        paddingRight = Math::Max(paddingRight, in->mRight);
    }

    Int32 switchWidth = Math::Max(mSwitchMinWidth,
            2 * mThumbWidth + paddingLeft + paddingRight);
    Int32 switchHeight = Math::Max(trackHeight, thumbHeight);

    mSwitchWidth = switchWidth;
    mSwitchHeight = switchHeight;

    CompoundButton::OnMeasure(widthMeasureSpec, heightMeasureSpec);

    Int32 measuredHeight;
    GetMeasuredHeight(&measuredHeight);
    if (measuredHeight < switchHeight) {
        Int32 state;
        GetMeasuredWidthAndState(&state);
        SetMeasuredDimension(state, switchHeight);
    }
    return NOERROR;
}

ECode Switch::OnPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    CompoundButton::OnPopulateAccessibilityEvent(event);

    Boolean res;
    IsChecked(&res);
    AutoPtr<ICharSequence> text = res ? mTextOn : mTextOff;
    if (text != NULL) {
        AutoPtr<IList> list;
        IAccessibilityRecord::Probe(event)->GetText((IList**)&list);
        list->Add(text);
    }
    return NOERROR;
}

AutoPtr<ILayout> Switch::MakeLayout(
    /* [in] */ ICharSequence* text)
{
    AutoPtr<ICharSequence> transformed;
    if (mSwitchTransformationMethod != NULL) {
        ITransformationMethod::Probe(mSwitchTransformationMethod)->GetTransformation(text,
                this, (ICharSequence**)&transformed);
    }
    else {
        transformed = text;
    }

    AutoPtr<ILayoutHelper> helper;
    CLayoutHelper::AcquireSingleton((ILayoutHelper**)&helper);
    Float res = 0;
    helper->GetDesiredWidth(transformed, mTextPaint, &res);

    AutoPtr<ILayout> layout;
    CStaticLayout::New(transformed, mTextPaint,
            (Int32)(Elastos::Core::Math::Ceil(res)), ALIGN_NORMAL, 1.f, 0,
            TRUE, (ILayout**)&layout);
    return layout;
}

Boolean Switch::HitThumb(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Int32 thumbOffset = GetThumbOffset();

    Boolean res = FALSE;
    mThumbDrawable->GetPadding(mTempRect, &res);
    Int32 thumbTop = mSwitchTop - mTouchSlop;
    Int32 thumbLeft = mSwitchLeft + thumbOffset - mTouchSlop;
    AutoPtr<CRect> rect = (CRect*)mTempRect.Get();
    Int32 thumbRight = thumbLeft + mThumbWidth + rect->mLeft + rect->mRight + mTouchSlop;
    Int32 thumbBottom = mSwitchBottom + mTouchSlop;
    return x > thumbLeft && x < thumbRight && y > thumbTop && y < thumbBottom;
}

ECode Switch::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    mVelocityTracker->AddMovement(ev);
    Int32 action = 0;
    ev->GetActionMasked(&action);

    switch (action) {
        case IMotionEvent::ACTION_DOWN: {
            Float x = 0, y = 0;
            ev->GetX(&x);
            ev->GetY(&y);
            Boolean isEnabled;
            if ((IsEnabled(&isEnabled), isEnabled) && HitThumb(x, y)) {
                mTouchMode = TOUCH_MODE_DOWN;
                mTouchX = x;
                mTouchY = y;
            }
            break;
        }

        case IMotionEvent::ACTION_MOVE: {
            switch (mTouchMode) {
                case TOUCH_MODE_IDLE: {
                    break;
                }

                case TOUCH_MODE_DOWN: {
                    Float x = 0, y = 0;
                    ev->GetX(&x);
                    ev->GetY(&y);
                    if (Elastos::Core::Math::Abs(x - mTouchX) > mTouchSlop ||
                        Elastos::Core::Math::Abs(y - mTouchY) > mTouchSlop) {
                        mTouchMode = TOUCH_MODE_DRAGGING;
                        AutoPtr<IViewParent> parent;
                        GetParent((IViewParent**)&parent);
                        parent->RequestDisallowInterceptTouchEvent(TRUE);
                        mTouchX = x;
                        mTouchY = y;
                        *res = TRUE;
                        return NOERROR;
                    }
                    break;
                }

                case TOUCH_MODE_DRAGGING: {
                    Float x;
                    ev->GetX(&x);
                    Int32 thumbScrollRange = GetThumbScrollRange();
                    Float thumbScrollOffset = x - mTouchX;
                    Float dPos;
                    if (thumbScrollRange != 0) {
                        dPos = thumbScrollOffset / thumbScrollRange;
                    }
                    else {
                        // If the thumb scroll range is empty, just use the
                        // movement direction to snap on or off.
                        dPos = thumbScrollOffset > 0 ? 1 : -1;
                    }
                    Boolean isLayoutRtl;
                    if (IsLayoutRtl(&isLayoutRtl), isLayoutRtl) {
                        dPos = -dPos;
                    }
                    Float newPos = MathUtils::Constrain(mThumbPosition + dPos, 0.0f, 1.0f);
                    if (newPos != mThumbPosition) {
                        mTouchX = x;
                        SetThumbPosition(newPos);
                    }
                    *res = TRUE;
                    return NOERROR;
                }
            }
            break;
        }

        case IMotionEvent::ACTION_UP:
        case IMotionEvent::ACTION_CANCEL: {
            if (mTouchMode == TOUCH_MODE_DRAGGING) {
                StopDrag(ev);
                // Allow super class to handle pressed state, etc
                Boolean result;
                CompoundButton::OnTouchEvent(ev, &result);
                *res = TRUE;
                return NOERROR;
            }
            mTouchMode = TOUCH_MODE_IDLE;
            mVelocityTracker->Clear();
            break;
        }
    }

    return CompoundButton::OnTouchEvent(ev, res);
}

void Switch::CancelSuperTouch(
    /* [in] */ IMotionEvent* ev)
{
    AutoPtr<IMotionEvent> cancle;
    CMotionEvent::Obtain(ev, (IMotionEvent**)&cancle);
    cancle->SetAction(IMotionEvent::ACTION_CANCEL);
    Boolean res;
    CompoundButton::OnTouchEvent(cancle, &res);
    IInputEvent::Probe(cancle)->Recycle();
}

void Switch::StopDrag(
    /* [in] */ IMotionEvent* ev)
{
    mTouchMode = TOUCH_MODE_IDLE;

    // Commit the change if the event is up and not canceled and the switch
    // has not been disabled during the drag.
    Int32 action = 0;
    ev->GetAction(&action);
    Boolean isEnabled;
    Boolean commitChange = action == IMotionEvent::ACTION_UP && (IsEnabled(&isEnabled), isEnabled);
    Boolean newState;

    if (commitChange) {
        mVelocityTracker->ComputeCurrentVelocity(1000);
        Float xvel;
        mVelocityTracker->GetXVelocity(&xvel);

        if (Elastos::Core::Math::Abs(xvel) > mMinFlingVelocity) {
            Boolean isLayoutRtl;
            IsLayoutRtl(&isLayoutRtl);
            newState = isLayoutRtl ? (xvel < 0) : (xvel > 0);
        }
        else {
            newState = GetTargetCheckedState();
        }
    }
    else {
        IsChecked(&newState);
    }

    SetChecked(newState);
    CancelSuperTouch(ev);
}

void Switch::AnimateThumbToCheckedState(
    /* [in] */ Boolean newCheckedState)
{
    Float targetPosition = newCheckedState ? 1.0f : 0.0f;
    AutoPtr< ArrayOf<Float> > param = ArrayOf<Float>::Alloc(1);
    (*param)[0] = targetPosition;
    mPositionAnimator = ObjectAnimator::OfFloat(
        TO_IINTERFACE(this), IProperty::Probe(THUMB_POS), param);
    IAnimator * positionAnimator = IAnimator::Probe(mPositionAnimator);
    positionAnimator->SetDuration(THUMB_ANIMATION_DURATION);
    mPositionAnimator->SetAutoCancel(TRUE);
    positionAnimator->Start();
}

void Switch::CancelPositionAnimator()
{
    if (mPositionAnimator != NULL) {
        IAnimator::Probe(mPositionAnimator)->Cancel();
    }
}

Boolean Switch::GetTargetCheckedState()
{
    return mThumbPosition > 0.5f;
}

/**
 * Sets the thumb position as a decimal value between 0 (off) and 1 (on).
 *
 * @param position new position between [0,1]
 */
void Switch::SetThumbPosition(
    /* [in] */ Float position)
{
    mThumbPosition = position;
    Invalidate();
}

ECode Switch::Toggle()
{
    Boolean res;
    IsChecked(&res);
    SetChecked(!res);
    return NOERROR;
}

ECode Switch::SetChecked(
    /* [in] */ Boolean checked)
{
    CompoundButton::SetChecked(checked);
    // Calling the super method may result in setChecked() getting called
    // recursively with a different value, so load the REAL value...
    IsChecked(&checked);

    Boolean isAttachedToWindow, isLaidOut;
    if ((IsAttachedToWindow(&isAttachedToWindow), isAttachedToWindow)
        && (IsLaidOut(&isLaidOut), isLaidOut)) {
        AnimateThumbToCheckedState(checked);
    }
    else {
        // Immediately move the thumb to the new position.
        CancelPositionAnimator();
        SetThumbPosition(checked ? 1 : 0);
    }
    return NOERROR;
}

ECode Switch::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    CompoundButton::OnLayout(changed, left, top, right, bottom);

    Int32 opticalInsetLeft = 0;
    Int32 opticalInsetRight = 0;
    if (mThumbDrawable != NULL) {
        AutoPtr<IRect> trackPadding = mTempRect;
        if (mTrackDrawable != NULL) {
            Boolean isGetPadding;
            mTrackDrawable->GetPadding(trackPadding, &isGetPadding);
        }
        else {
            trackPadding->SetEmpty();
        }

        AutoPtr<IInsets> insets;
        mThumbDrawable->GetOpticalInsets((IInsets**)&insets);
        Insets* in = (Insets*)insets.Get();
        CRect* temp = (CRect*)trackPadding.Get();
        opticalInsetLeft = Elastos::Core::Math::Max(0, in->mLeft - temp->mLeft);
        opticalInsetRight = Elastos::Core::Math::Max(0, in->mRight - temp->mRight);
    }

    Int32 switchRight, switchLeft;
    Boolean isLayoutRtl;
    if (IsLayoutRtl(&isLayoutRtl), isLayoutRtl) {
        GetPaddingLeft(&switchLeft);
        switchLeft += opticalInsetLeft;
        switchRight = switchLeft + mSwitchWidth - opticalInsetLeft - opticalInsetRight;
    }
    else {
        Int32 width, right;
        GetWidth(&width);
        GetPaddingRight(&right);
        switchRight = width - right - opticalInsetRight;
        switchLeft = switchRight - mSwitchWidth + opticalInsetLeft + opticalInsetRight;
    }

    Int32 switchTop = 0, switchBottom = 0, gravity = 0;
    GetGravity(&gravity);
    switch (gravity & IGravity::VERTICAL_GRAVITY_MASK) {
        default:
        case IGravity::TOP:
            GetPaddingTop(&switchTop);
            switchBottom = switchTop + mSwitchHeight;
            break;

        case IGravity::CENTER_VERTICAL: {
            Int32 top, height, bottom;
            GetPaddingTop(&top);
            GetHeight(&height);
            GetPaddingBottom(&bottom);
            switchTop = (top + height - bottom) / 2 - mSwitchHeight / 2;
            switchBottom = switchTop + mSwitchHeight;
            break;
        }

        case IGravity::BOTTOM: {
            Int32 height, bottom;
            GetHeight(&height);
            GetPaddingBottom(&bottom);
            switchBottom = height - bottom;
            switchTop = switchBottom - mSwitchHeight;
            break;
        }

    }

    mSwitchLeft = switchLeft;
    mSwitchTop = switchTop;
    mSwitchBottom = switchBottom;
    mSwitchRight = switchRight;

    return NOERROR;
}

ECode Switch::Draw(
    /* [in] */ ICanvas* canvas)
{
    CRect* padding = (CRect*)mTempRect.Get();
    Int32 switchLeft = mSwitchLeft;
    Int32 switchTop = mSwitchTop;
    Int32 switchRight = mSwitchRight;
    Int32 switchBottom = mSwitchBottom;

    Int32 thumbInitialLeft = switchLeft + GetThumbOffset();

    AutoPtr<IInsets> thumbInsets;
    if (mThumbDrawable != NULL) {
        mThumbDrawable->GetOpticalInsets((IInsets**)&thumbInsets);
    }
    else {
        thumbInsets = Insets::NONE;
    }

    // Layout the track.
    if (mTrackDrawable != NULL) {
        Boolean isGetPadding;
        mTrackDrawable->GetPadding(padding, &isGetPadding);

        // Adjust thumb position for track padding.
        thumbInitialLeft += padding->mLeft;

        // If necessary, offset by the optical insets of the thumb asset.
        Int32 trackLeft = switchLeft;
        Int32 trackTop = switchTop;
        Int32 trackRight = switchRight;
        Int32 trackBottom = switchBottom;
        Insets* in = (Insets*)thumbInsets.Get();
        if (thumbInsets != Insets::NONE) {
            if (in->mLeft > padding->mLeft) {
                trackLeft += in->mLeft - padding->mLeft;
            }
            if (in->mTop > padding->mTop) {
                trackTop += in->mTop - padding->mTop;
            }
            if (in->mRight > padding->mRight) {
                trackRight -= in->mRight - padding->mRight;
            }
            if (in->mBottom > padding->mBottom) {
                trackBottom -= in->mBottom - padding->mBottom;
            }
        }
        mTrackDrawable->SetBounds(trackLeft, trackTop, trackRight, trackBottom);
    }

    // Layout the thumb.
    if (mThumbDrawable != NULL) {
        Boolean isGetPadding;
        mThumbDrawable->GetPadding(padding, &isGetPadding);

        Int32 thumbLeft = thumbInitialLeft - padding->mLeft;
        Int32 thumbRight = thumbInitialLeft + mThumbWidth + padding->mRight;
        mThumbDrawable->SetBounds(thumbLeft, switchTop, thumbRight, switchBottom);

        AutoPtr<IDrawable> background;
        GetBackground((IDrawable**)&background);
        if (background != NULL) {
            background->SetHotspotBounds(thumbLeft, switchTop, thumbRight, switchBottom);
        }
    }

    // Draw the background.
    CompoundButton::Draw(canvas);
    return NOERROR;
}

void Switch::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    CompoundButton::OnDraw(canvas);

    CRect* padding = (CRect*)mTempRect.Get();
    AutoPtr<IDrawable> trackDrawable = mTrackDrawable;
    if (trackDrawable != NULL) {
        Boolean isGetPadding;
        trackDrawable->GetPadding(padding, &isGetPadding);
    }
    else {
        padding->SetEmpty();
    }

    Int32 switchTop = mSwitchTop;
    Int32 switchBottom = mSwitchBottom;
    Int32 switchInnerTop = switchTop + padding->mTop;
    Int32 switchInnerBottom = switchBottom - padding->mBottom;

    AutoPtr<IDrawable> thumbDrawable = mThumbDrawable;
    if (trackDrawable != NULL) {
        if (mSplitTrack && thumbDrawable != NULL) {
            AutoPtr<IInsets> insets;
            thumbDrawable->GetOpticalInsets((IInsets**)&insets);
            thumbDrawable->CopyBounds(mTempRect);
            Insets* in = (Insets*)insets.Get();
            padding->mLeft += in->mLeft;
            padding->mRight -= in->mRight;

            Int32 saveCount;
            canvas->Save(&saveCount);
            Boolean isNonEmpty;
            canvas->ClipRect(padding, Elastos::Droid::Graphics::RegionOp_DIFFERENCE, &isNonEmpty);
            trackDrawable->Draw(canvas);
            canvas->RestoreToCount(saveCount);
        }
        else {
            trackDrawable->Draw(canvas);
        }
    }

    Int32 saveCount;
    canvas->Save(&saveCount);

    if (thumbDrawable != NULL) {
        thumbDrawable->Draw(canvas);
    }

    AutoPtr<ILayout> switchText = GetTargetCheckedState() ? mOnLayout : mOffLayout;
    if (switchText != NULL) {
        AutoPtr< ArrayOf<Int32> > drawableState;
        GetDrawableState((ArrayOf<Int32>**)&drawableState);
        if (mTextColors != NULL) {
            Int32 color;
            mTextColors->GetColorForState(drawableState, 0, &color);
            IPaint::Probe(mTextPaint)->SetColor(color);
        }
        mTextPaint->SetDrawableState(drawableState);

        Int32 cX;
        if (thumbDrawable != NULL) {
            AutoPtr<IRect> bounds;
            thumbDrawable->GetBounds((IRect**)&bounds);
            CRect* boundsTemp = (CRect*)bounds.Get();
            cX = boundsTemp->mLeft + boundsTemp->mRight;
        }
        else {
            GetWidth(&cX);
        }

        Int32 layoutWidth, layoutHeight;
        switchText->GetWidth(&layoutWidth);
        Int32 left = cX / 2 - layoutWidth / 2;
        switchText->GetHeight(&layoutHeight);
        Int32 top = (switchInnerTop + switchInnerBottom) / 2 - layoutHeight / 2;
        canvas->Translate((Float)left, (Float)top);
        switchText->Draw(canvas);
    }

    canvas->RestoreToCount(saveCount);
}

ECode Switch::GetCompoundPaddingLeft(
    /* [out] */ Int32* left)
{
    VALIDATE_NOT_NULL(left);

    Boolean isLayoutRtl;
    if (IsLayoutRtl(&isLayoutRtl), !isLayoutRtl) {
        return CompoundButton::GetCompoundPaddingLeft(left);
    }

    Int32 _left;
    CompoundButton::GetCompoundPaddingLeft(&_left);
    Int32 padding = _left + mSwitchWidth;
    AutoPtr<ICharSequence> text;
    GetText((ICharSequence**)&text);
    if (!TextUtils::IsEmpty(text)) {
        padding += mSwitchPadding;
    }
    *left = padding;
    return NOERROR;
}

ECode Switch::GetCompoundPaddingRight(
    /* [out] */ Int32* right)
{
    VALIDATE_NOT_NULL(right);
    Boolean isLayoutRtl;
    if (IsLayoutRtl(&isLayoutRtl), isLayoutRtl) {
        return CompoundButton::GetCompoundPaddingRight(right);
    }

    Int32 _right;
    CompoundButton::GetCompoundPaddingRight(&_right);
    Int32 padding = _right + mSwitchWidth;
    AutoPtr<ICharSequence> text;
    GetText((ICharSequence**)&text);
    if (!TextUtils::IsEmpty(text)) {
        padding += mSwitchPadding;
    }

    *right = padding;
    return NOERROR;
}

/**
 * Translates thumb position to offset according to current RTL setting and
 * thumb scroll range. Accounts for both track and thumb padding.
 *
 * @return thumb offset
 */
Int32 Switch::GetThumbOffset()
{
    Float thumbPosition;
    Boolean isLayoutRtl;
    if (IsLayoutRtl(&isLayoutRtl), isLayoutRtl) {
        thumbPosition = 1.0f - mThumbPosition;
    }
    else {
        thumbPosition = mThumbPosition;
    }
    return (Int32) (thumbPosition * GetThumbScrollRange() + 0.5f);
}

Int32 Switch::GetThumbScrollRange()
{
    if (mTrackDrawable != NULL) {
        CRect* padding = (CRect*)mTempRect.Get();
        Boolean isGetPadding;
        mTrackDrawable->GetPadding(padding, &isGetPadding);

        AutoPtr<IInsets> insets;
        if (mThumbDrawable != NULL) {
            mThumbDrawable->GetOpticalInsets((IInsets**)&insets);
        }
        else {
            insets = Insets::NONE;
        }
        Insets* in = (Insets*)insets.Get();

        return mSwitchWidth - mThumbWidth - padding->mLeft - padding->mRight
                - in->mLeft - in->mRight;
    }
    else {
        return 0;
    }
}

ECode Switch::OnCreateDrawableState(
    /* [in] */ Int32 extraSpace,
    /* [out, callee] */ ArrayOf<Int32>** drawableState)
{
    VALIDATE_NOT_NULL(drawableState)
    CompoundButton::OnCreateDrawableState(extraSpace + 1, drawableState);
    Boolean res;
    IsChecked(&res);
    if (res) {
        MergeDrawableStates(*drawableState, CHECKED_STATE_SET);
    }
    return NOERROR;
}

ECode Switch::DrawableStateChanged()
{
    CompoundButton::DrawableStateChanged();

    AutoPtr< ArrayOf<Int32> > myDrawableState;
    GetDrawableState((ArrayOf<Int32>**)&myDrawableState);

    Boolean isStateful;
    if (mThumbDrawable != NULL) {
        mThumbDrawable->SetState(myDrawableState, &isStateful);
    }

    if (mTrackDrawable != NULL) {
        mTrackDrawable->SetState(myDrawableState, &isStateful);
    }

    Invalidate();
    return NOERROR;
}

//@Override
ECode Switch::DrawableHotspotChanged(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    CompoundButton::DrawableHotspotChanged(x, y);

    if (mThumbDrawable != NULL) {
        mThumbDrawable->SetHotspot(x, y);
    }

    if (mTrackDrawable != NULL) {
        mTrackDrawable->SetHotspot(x, y);
    }
    return NOERROR;
}

Boolean Switch::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    return CompoundButton::VerifyDrawable(who) || who == mThumbDrawable.Get() || who == mTrackDrawable.Get();
}

ECode Switch::JumpDrawablesToCurrentState()
{
    CompoundButton::JumpDrawablesToCurrentState();

    if (mThumbDrawable != NULL) mThumbDrawable->JumpToCurrentState();
    if (mTrackDrawable != NULL) mTrackDrawable->JumpToCurrentState();

    Boolean isRunning;
    if (mPositionAnimator != NULL
        && (IAnimator::Probe(mPositionAnimator)->IsRunning(&isRunning), isRunning)) {
        IAnimator::Probe(mPositionAnimator)->End();
        mPositionAnimator = NULL;
    }
    return NOERROR;
}

ECode Switch::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    CompoundButton::OnInitializeAccessibilityEvent(event);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CSwitch"), (ICharSequence**)&seq);
    return IAccessibilityRecord::Probe(event)->SetClassName(seq);
}

ECode Switch::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    CompoundButton::OnInitializeAccessibilityNodeInfo(info);
    AutoPtr<ICharSequence> seq;
    CString::New(String("CSwitch"), (ICharSequence**)&seq);
    info->SetClassName(seq);

    Boolean res;
    IsChecked(&res);
    AutoPtr<ICharSequence> switchText = res ? mTextOn : mTextOff;
    if (!TextUtils::IsEmpty(switchText)) {
        AutoPtr<ICharSequence> oldText;
        info->GetText((ICharSequence**)&oldText);
        if (TextUtils::IsEmpty(oldText)) {
            info->SetText(switchText);
        }
        else {
            StringBuilder sb;
            sb.Append(oldText);
            sb.AppendChar(' ');
            sb.Append(switchText);
            info->SetText(sb.ToCharSequence());
        }
    }
    return NOERROR;
}

/////////////////////////////////////////////////////////////
//              Switch::SwitchProperty
/////////////////////////////////////////////////////////////
Switch::SwitchProperty::SwitchProperty(
    /* [in] */ const String& name)
    : FloatProperty(name)
{
}

ECode Switch::SwitchProperty::Get(
    /* [in] */ IInterface* obj,
    /* [out] */ IInterface** rst)
{
    VALIDATE_NOT_NULL(rst)
    Switch* temp = (Switch*)ISwitch::Probe(obj);
    AutoPtr<IFloat> result;
    CFloat::New(temp->mThumbPosition, (IFloat**)&result);
    *rst = result;
    REFCOUNT_ADD(*rst)
    return NOERROR;
}

ECode Switch::SwitchProperty::SetValue(
    /* [in] */ IInterface* obj,
    /* [in] */ Float value)
{
    Switch* temp = (Switch*)ISwitch::Probe(obj);
    temp->SetThumbPosition(value);
    return NOERROR;
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
