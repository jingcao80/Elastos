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

#include "elastos/droid/support/v4/view/PagerTitleStrip.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/R.h"
#include <elastos/core/Math.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Droid::Os::Build;
using Elastos::Droid::Text::TextUtilsTruncateAt_END;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::Widget::CTextView;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

const String PagerTitleStrip::TAG("PagerTitleStrip");

static AutoPtr< ArrayOf<Int32> > InitATTRS()
{
    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(4);
    (*args)[0] = Elastos::Droid::R::attr::textAppearance;
    (*args)[1] = Elastos::Droid::R::attr::textSize;
    (*args)[2] = Elastos::Droid::R::attr::textColor;
    (*args)[3] = Elastos::Droid::R::attr::gravity;

    return args;
}

const AutoPtr<ArrayOf<Int32> > PagerTitleStrip::ATTRS = InitATTRS();

static AutoPtr< ArrayOf<Int32> > InitTEXT_ATTRS()
{
    AutoPtr< ArrayOf<Int32> > args = ArrayOf<Int32>::Alloc(1);
    (*args)[0] = 0x0101038c; // android.R.attr.textAllCaps;

    return args;
}

const AutoPtr<ArrayOf<Int32> > PagerTitleStrip::TEXT_ATTRS = InitTEXT_ATTRS();

const Float PagerTitleStrip::SIDE_ALPHA = 0.6f;
const Int32 PagerTitleStrip::TEXT_SPACING = 16; // dip

AutoPtr<IPagerTitleStripImpl> PagerTitleStrip::IMPL;

Boolean PagerTitleStrip::sInit = PagerTitleStrip::InitStatic();

Boolean PagerTitleStrip::InitStatic()
{
    if (Build::VERSION::SDK_INT >= 14) {
        IMPL = new PagerTitleStripImplIcs();
    }
    else {
        IMPL = new PagerTitleStripImplBase();
    }
    return TRUE;
}

//===============================================================================
//                  PagerTitleStrip::PagerTitleStripImplBase
//===============================================================================

CAR_INTERFACE_IMPL(PagerTitleStrip::PagerTitleStripImplBase, Object, IPagerTitleStripImpl)

PagerTitleStrip::PagerTitleStripImplBase::PagerTitleStripImplBase()
{}

PagerTitleStrip::PagerTitleStripImplBase::~PagerTitleStripImplBase()
{}

ECode PagerTitleStrip::PagerTitleStripImplBase::SetSingleLineAllCaps(
    /* [in] */ ITextView* text)
{
    return text->SetSingleLine();
}

//===============================================================================
//                  PagerTitleStrip::PagerTitleStripImplIcs
//===============================================================================

CAR_INTERFACE_IMPL(PagerTitleStrip::PagerTitleStripImplIcs, Object, IPagerTitleStripImpl)

PagerTitleStrip::PagerTitleStripImplIcs::PagerTitleStripImplIcs()
{}

PagerTitleStrip::PagerTitleStripImplIcs::~PagerTitleStripImplIcs()
{}

ECode PagerTitleStrip::PagerTitleStripImplIcs::SetSingleLineAllCaps(
    /* [in] */ ITextView* text)
{
    Slogger::I("PagerTitleStrip::PagerTitleStripImplIcs", "PagerTitleStripImplIcs is TODO");
    // PagerTitleStripIcs::SetSingleLineAllCaps(text);
    return NOERROR;
}

//===============================================================================
//                  PagerTitleStrip::PageListener
//===============================================================================

CAR_INTERFACE_IMPL_2(PagerTitleStrip::PageListener, DataSetObserver, IViewPagerOnPageChangeListener, IViewPagerOnAdapterChangeListener)

PagerTitleStrip::PageListener::PageListener(
    /* [in] */ PagerTitleStrip* host)
    : mScrollState(0)
    , mHost(host)
{}

PagerTitleStrip::PageListener::~PageListener()
{}

ECode PagerTitleStrip::PageListener::OnPageScrolled(
    /* [in] */ Int32 position,
    /* [in] */ Float positionOffset,
    /* [in] */ Int32 positionOffsetPixels)
{
    if (positionOffset > 0.5f) {
        // Consider ourselves to be on the next page when we're 50% of the way there.
        position++;
    }
    mHost->UpdateTextPositions(position, positionOffset, FALSE);
    return NOERROR;
}

ECode PagerTitleStrip::PageListener::OnPageSelected(
    /* [in] */ Int32 position)
{
    if (mScrollState == IViewPager::SCROLL_STATE_IDLE) {
        // Only update the text here if we're not dragging or settling.
        Int32 item;
        mHost->mPager->GetCurrentItem(&item);
        AutoPtr<IPagerAdapter> adapter;
        mHost->mPager->GetAdapter((IPagerAdapter**)&adapter);
        mHost->UpdateText(item, adapter);

        Float offset = mHost->mLastKnownPositionOffset >= 0 ? mHost->mLastKnownPositionOffset : 0;
        mHost->UpdateTextPositions(item, offset, TRUE);
    }
    return NOERROR;
}

ECode PagerTitleStrip::PageListener::OnPageScrollStateChanged(
    /* [in] */ Int32 state)
{
    mScrollState = state;
    return NOERROR;
}

ECode PagerTitleStrip::PageListener::OnAdapterChanged(
    /* [in] */ IPagerAdapter* oldAdapter,
    /* [in] */ IPagerAdapter* newAdapter)
{
    mHost->UpdateAdapter(oldAdapter, newAdapter);
    return NOERROR;
}

ECode PagerTitleStrip::PageListener::OnChanged()
{
    Int32 item;
    mHost->mPager->GetCurrentItem(&item);
    AutoPtr<IPagerAdapter> adapter;
    mHost->mPager->GetAdapter((IPagerAdapter**)&adapter);
    mHost->UpdateText(item, adapter);

    Float offset = mHost->mLastKnownPositionOffset >= 0 ?mHost->mLastKnownPositionOffset : 0;
    mHost->UpdateTextPositions(item, offset, TRUE);
    return NOERROR;
}

//===============================================================================
//                  PagerTitleStrip
//===============================================================================

CAR_INTERFACE_IMPL_2(PagerTitleStrip, ViewGroup, IPagerTitleStrip, IViewPagerDecor)

PagerTitleStrip::PagerTitleStrip()
    : mTextColor(0)
    , mLastKnownCurrentPage(-1)
    , mLastKnownPositionOffset(-1)
    , mScaledTextSpacing(0)
    , mGravity(0)
    , mUpdatingText(FALSE)
    , mUpdatingPositions(FALSE)
    , mNonPrimaryAlpha(0)
{
    mPageListener = new PageListener(this);
}

PagerTitleStrip::~PagerTitleStrip()
{}

ECode PagerTitleStrip::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode PagerTitleStrip::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    ViewGroup::constructor(context, attrs);

    CTextView::New(context, (ITextView**)&mPrevText);
    CTextView::New(context, (ITextView**)&mCurrText);
    CTextView::New(context, (ITextView**)&mNextText);
    AddView(IView::Probe(mPrevText));
    AddView(IView::Probe(mCurrText));
    AddView(IView::Probe(mNextText));

    AutoPtr<ITypedArray> a;
    context->ObtainStyledAttributes(attrs, ATTRS, (ITypedArray**)&a);
    Int32 textAppearance;
    a->GetResourceId(0, 0, &textAppearance);
    if (textAppearance != 0) {
        mPrevText->SetTextAppearance(context, textAppearance);
        mCurrText->SetTextAppearance(context, textAppearance);
        mNextText->SetTextAppearance(context, textAppearance);
    }
    Int32 textSize;
    a->GetDimensionPixelSize(1, 0, &textSize);
    if (textSize != 0) {
        SetTextSize(ITypedValue::COMPLEX_UNIT_PX, textSize);
    }
    Boolean hasValue;
    if (a->HasValue(2, &hasValue), hasValue) {
        Int32 textColor;
        a->GetColor(2, 0, &textColor);
        mPrevText->SetTextColor(textColor);
        mCurrText->SetTextColor(textColor);
        mNextText->SetTextColor(textColor);
    }
    a->GetInteger(3, IGravity::BOTTOM, &mGravity);
    a->Recycle();

    AutoPtr<IColorStateList> colors;
    mCurrText->GetTextColors((IColorStateList**)&colors);
    colors->GetDefaultColor(&mTextColor);
    SetNonPrimaryAlpha(SIDE_ALPHA);

    mPrevText->SetEllipsize(TextUtilsTruncateAt_END);
    mCurrText->SetEllipsize(TextUtilsTruncateAt_END);
    mNextText->SetEllipsize(TextUtilsTruncateAt_END);

    Boolean allCaps = FALSE;
    if (textAppearance != 0) {
        AutoPtr<ITypedArray> ta;
        context->ObtainStyledAttributes(textAppearance, TEXT_ATTRS, (ITypedArray**)&ta);
        ta->GetBoolean(0, FALSE, &allCaps);
        ta->Recycle();
    }

    if (allCaps) {
        SetSingleLineAllCaps(mPrevText);
        SetSingleLineAllCaps(mCurrText);
        SetSingleLineAllCaps(mNextText);
    }
    else {
        mPrevText->SetSingleLine();
        mCurrText->SetSingleLine();
        mNextText->SetSingleLine();
    }

    AutoPtr<IResources> resources;
    context->GetResources((IResources**)&resources);
    AutoPtr<IDisplayMetrics> metrics;
    resources->GetDisplayMetrics((IDisplayMetrics**)&metrics);
    Float density;
    metrics->GetDensity(&density);
    mScaledTextSpacing = (Int32) (TEXT_SPACING * density);
    return NOERROR;
}

void PagerTitleStrip::SetSingleLineAllCaps(
    /* [in] */ ITextView* text)
{
    IMPL->SetSingleLineAllCaps(text);
}

ECode PagerTitleStrip::SetTextSpacing(
    /* [in] */ Int32 spacingPixels)
{
    mScaledTextSpacing = spacingPixels;
    RequestLayout();
    return NOERROR;
}

ECode PagerTitleStrip::GetTextSpacing(
    /* [out] */ Int32* scaledTextSpacing)
{
    *scaledTextSpacing = mScaledTextSpacing;
    return NOERROR;
}

ECode PagerTitleStrip::SetNonPrimaryAlpha(
    /* [in] */ Float alpha)
{
    mNonPrimaryAlpha = (Int32) (alpha * 255) & 0xFF;
    Int32 transparentColor = (mNonPrimaryAlpha << 24) | (mTextColor & 0xFFFFFF);
    mPrevText->SetTextColor(transparentColor);
    mNextText->SetTextColor(transparentColor);
    return NOERROR;
}

ECode PagerTitleStrip::SetTextColor(
    /* [in] */ Int32 color)
{
    mTextColor = color;
    mCurrText->SetTextColor(color);
    Int32 transparentColor = (mNonPrimaryAlpha << 24) | (mTextColor & 0xFFFFFF);
    mPrevText->SetTextColor(transparentColor);
    mNextText->SetTextColor(transparentColor);
    return NOERROR;
}

ECode PagerTitleStrip::SetTextSize(
    /* [in] */ Int32 unit,
    /* [in] */ Float size)
{
    mPrevText->SetTextSize(unit, size);
    mCurrText->SetTextSize(unit, size);
    mNextText->SetTextSize(unit, size);
    return NOERROR;
}

ECode PagerTitleStrip::SetGravity(
    /* [in] */ Int32 gravity)
{
    mGravity = gravity;
    RequestLayout();
    return NOERROR;
}

ECode PagerTitleStrip::OnAttachedToWindow()
{
    ViewGroup::OnAttachedToWindow();

    AutoPtr<IViewParent> parent;
    GetParent((IViewParent**)&parent);
    AutoPtr<IViewPager> pager = IViewPager::Probe(parent);
    if (pager == NULL) {
        Slogger::E(TAG, "PagerTitleStrip must be a direct child of a ViewPager.");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    AutoPtr<IPagerAdapter> adapter;
    pager->GetAdapter((IPagerAdapter**)&adapter);

    AutoPtr<IViewPagerOnPageChangeListener> listener;
    pager->SetInternalPageChangeListener(mPageListener, (IViewPagerOnPageChangeListener**)&listener);
    pager->SetOnAdapterChangeListener(mPageListener);
    mPager = pager;
    AutoPtr<IPagerAdapter> ref;
    if (mWatchingAdapter != NULL) {
        mWatchingAdapter->Resolve(EIID_IPagerAdapter, (IInterface**)&ref);
    }
    UpdateAdapter(ref, adapter);
    return NOERROR;
}

ECode PagerTitleStrip::OnDetachedFromWindow()
{
    ViewGroup::OnDetachedFromWindow();
    if (mPager != NULL) {
        AutoPtr<IPagerAdapter> adapter;
        mPager->GetAdapter((IPagerAdapter**)&adapter);
        UpdateAdapter(adapter, NULL);
        AutoPtr<IViewPagerOnPageChangeListener> listener;
        mPager->SetInternalPageChangeListener(NULL, (IViewPagerOnPageChangeListener**)&listener);
        mPager->SetOnAdapterChangeListener(NULL);
        mPager = NULL;
    }
    return NOERROR;
}

void PagerTitleStrip::UpdateText(
    /* [in] */ Int32 currentItem,
    /* [in] */ IPagerAdapter* adapter)
{
    Int32 itemCount = 0;
    if (adapter != NULL) {
        adapter->GetCount(&itemCount);
    }
    mUpdatingText = TRUE;

    AutoPtr<ICharSequence> text;
    if (currentItem >= 1 && adapter != NULL) {
        adapter->GetPageTitle(currentItem - 1, (ICharSequence**)&text);
    }
    mPrevText->SetText(text);

    AutoPtr<ICharSequence> cs;
    if (adapter != NULL && currentItem < itemCount) {
        adapter->GetPageTitle(currentItem, (ICharSequence**)&cs);
    }
    mCurrText->SetText(cs);

    text = NULL;
    if (currentItem + 1 < itemCount && adapter != NULL) {
        text = NULL;
        adapter->GetPageTitle(currentItem + 1, (ICharSequence**)&text);
    }
    mNextText->SetText(text);

    // Measure everything
    Int32 width, height, left, right, top, bottom;
    GetWidth(&width);
    GetPaddingLeft(&left);
    GetPaddingRight(&right);
    width -= left + right;

    GetHeight(&height);
    GetPaddingTop(&top);
    GetPaddingBottom(&bottom);

    const Int32 childHeight = height - top - bottom;
    const Int32 childWidthSpec = View::MeasureSpec::MakeMeasureSpec((Int32) (width * 0.8f),
            View::MeasureSpec::AT_MOST);
    const Int32 childHeightSpec = View::MeasureSpec::MakeMeasureSpec(childHeight, View::MeasureSpec::AT_MOST);
    IView::Probe(mPrevText)->Measure(childWidthSpec, childHeightSpec);
    IView::Probe(mCurrText)->Measure(childWidthSpec, childHeightSpec);
    IView::Probe(mNextText)->Measure(childWidthSpec, childHeightSpec);

    mLastKnownCurrentPage = currentItem;

    if (!mUpdatingPositions) {
        UpdateTextPositions(currentItem, mLastKnownPositionOffset, FALSE);
    }

    mUpdatingText = FALSE;
}

ECode PagerTitleStrip::RequestLayout()
{
    if (!mUpdatingText) {
        ViewGroup::RequestLayout();
    }
    return NOERROR;
}

void PagerTitleStrip::UpdateAdapter(
    /* [in] */ IPagerAdapter* oldAdapter,
    /* [in] */ IPagerAdapter* newAdapter)
{
    if (oldAdapter != NULL) {
        oldAdapter->UnregisterDataSetObserver(mPageListener);
        mWatchingAdapter = NULL;
    }
    if (newAdapter != NULL) {
        newAdapter->RegisterDataSetObserver(mPageListener);

        IWeakReferenceSource* wrs = IWeakReferenceSource::Probe(newAdapter);
        assert(wrs != NULL);
        mWatchingAdapter = NULL;
        wrs->GetWeakReference((IWeakReference**)&mWatchingAdapter);
    }
    if (mPager != NULL) {
        mLastKnownCurrentPage = -1;
        mLastKnownPositionOffset = -1;
        Int32 item;
        mPager->GetCurrentItem(&item);
        UpdateText(item, newAdapter);
        RequestLayout();
    }
}

void PagerTitleStrip::UpdateTextPositions(
    /* [in] */ Int32 position,
    /* [in] */ Float positionOffset,
    /* [in] */ Boolean force)
{
    if (position != mLastKnownCurrentPage) {
        AutoPtr<IPagerAdapter> adapter;
        mPager->GetAdapter((IPagerAdapter**)&adapter);
        UpdateText(position, adapter);
    }
    else if (!force && positionOffset == mLastKnownPositionOffset) {
        return;
    }

    mUpdatingPositions = TRUE;

    IView* prevText = IView::Probe(mPrevText);
    IView* currText = IView::Probe(mCurrText);
    IView* nextText = IView::Probe(mNextText);

    Int32 prevWidth;
    prevText->GetMeasuredWidth(&prevWidth);
    Int32 currWidth;
    currText->GetMeasuredWidth(&currWidth);
    Int32 nextWidth;
    nextText->GetMeasuredWidth(&nextWidth);
    Int32 halfCurrWidth = currWidth / 2;

    Int32 stripWidth;
    GetWidth(&stripWidth);
    Int32 stripHeight;
    GetHeight(&stripHeight);
    Int32 paddingLeft;
    GetPaddingLeft(&paddingLeft);
    Int32 paddingRight;
    GetPaddingRight(&paddingRight);
    Int32 paddingTop;
    GetPaddingTop(&paddingTop);
    Int32 paddingBottom;
    GetPaddingBottom(&paddingBottom);
    Int32 textPaddedLeft = paddingLeft + halfCurrWidth;
    Int32 textPaddedRight = paddingRight + halfCurrWidth;
    Int32 contentWidth = stripWidth - textPaddedLeft - textPaddedRight;

    Float currOffset = positionOffset + 0.5f;
    if (currOffset > 1.f) {
        currOffset -= 1.f;
    }
    Int32 currCenter = stripWidth - textPaddedRight - (Int32) (contentWidth * currOffset);
    Int32 currLeft = currCenter - currWidth / 2;
    Int32 currRight = currLeft + currWidth;

    Int32 prevBaseline;
    prevText->GetBaseline(&prevBaseline);
    Int32 currBaseline;
    currText->GetBaseline(&currBaseline);
    Int32 nextBaseline;
    nextText->GetBaseline(&nextBaseline);
    using Elastos::Core::Math;
    Int32 maxBaseline = Math::Max(Math::Max(prevBaseline, currBaseline), nextBaseline);
    Int32 prevTopOffset = maxBaseline - prevBaseline;
    Int32 currTopOffset = maxBaseline - currBaseline;
    Int32 nextTopOffset = maxBaseline - nextBaseline;

    Int32 prevHeight;
    Int32 alignedPrevHeight = prevTopOffset + (prevText->GetMeasuredHeight(&prevHeight), prevHeight);
    Int32 currHeight;
    Int32 alignedCurrHeight = currTopOffset + (currText->GetMeasuredHeight(&currHeight), currHeight);
    Int32 nextHeight;
    Int32 alignedNextHeight = nextTopOffset + (nextText->GetMeasuredHeight(&nextHeight), nextHeight);
    Int32 maxTextHeight = Math::Max(Math::Max(alignedPrevHeight, alignedCurrHeight),
            alignedNextHeight);

    Int32 vgrav = mGravity & IGravity::VERTICAL_GRAVITY_MASK;

    Int32 prevTop;
    Int32 currTop;
    Int32 nextTop;
    switch (vgrav) {
        default:
        case IGravity::TOP:
            prevTop = paddingTop + prevTopOffset;
            currTop = paddingTop + currTopOffset;
            nextTop = paddingTop + nextTopOffset;
            break;
        case IGravity::CENTER_VERTICAL: {
            Int32 paddedHeight = stripHeight - paddingTop - paddingBottom;
            Int32 centeredTop = (paddedHeight - maxTextHeight) / 2;
            prevTop = centeredTop + prevTopOffset;
            currTop = centeredTop + currTopOffset;
            nextTop = centeredTop + nextTopOffset;
            break;
        }
        case IGravity::BOTTOM: {
            Int32 bottomGravTop = stripHeight - paddingBottom - maxTextHeight;
            prevTop = bottomGravTop + prevTopOffset;
            currTop = bottomGravTop + currTopOffset;
            nextTop = bottomGravTop + nextTopOffset;
            break;
        }
    }

    currText->Layout(currLeft, currTop, currRight,
            currTop + (currText->GetMeasuredHeight(&currHeight), currHeight));

    Int32 prevLeft = Math::Min(paddingLeft, currLeft - mScaledTextSpacing - prevWidth);
    prevText->Layout(prevLeft, prevTop, prevLeft + prevWidth,
            prevTop + (prevText->GetMeasuredHeight(&prevHeight), prevHeight));

    Int32 nextLeft = Math::Max(stripWidth - paddingRight - nextWidth,
            currRight + mScaledTextSpacing);
    nextText->Layout(nextLeft, nextTop, nextLeft + nextWidth,
            nextTop + (nextText->GetMeasuredHeight(&nextHeight), nextHeight));

    mLastKnownPositionOffset = positionOffset;
    mUpdatingPositions = FALSE;
}

ECode PagerTitleStrip::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 widthMode = View::MeasureSpec::GetMode(widthMeasureSpec);
    Int32 heightMode = View::MeasureSpec::GetMode(heightMeasureSpec);
    Int32 widthSize = View::MeasureSpec::GetSize(widthMeasureSpec);
    Int32 heightSize = View::MeasureSpec::GetSize(heightMeasureSpec);

    if (widthMode != View::MeasureSpec::EXACTLY) {
        Slogger::E(TAG, "Must measure with an exact width");
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    Int32 childHeight = heightSize;
    Int32 minHeight = GetMinHeight();
    Int32 top, bottom;
    GetPaddingTop(&top);
    GetPaddingBottom(&bottom);
    Int32 padding = 0;
    padding = top + bottom;
    childHeight -= padding;

    Int32 childWidthSpec = View::MeasureSpec::MakeMeasureSpec((Int32) (widthSize * 0.8f),
            View::MeasureSpec::AT_MOST);
    Int32 childHeightSpec = View::MeasureSpec::MakeMeasureSpec(childHeight, View::MeasureSpec::AT_MOST);

    IView::Probe(mPrevText)->Measure(childWidthSpec, childHeightSpec);
    IView::Probe(mCurrText)->Measure(childWidthSpec, childHeightSpec);
    IView::Probe(mNextText)->Measure(childWidthSpec, childHeightSpec);

    if (heightMode == View::MeasureSpec::EXACTLY) {
        SetMeasuredDimension(widthSize, heightSize);
    }
    else {
        Int32 textHeight;
        IView::Probe(mCurrText)->GetMeasuredHeight(&textHeight);
        SetMeasuredDimension(widthSize, Elastos::Core::Math::Max(minHeight, textHeight + padding));
    }
    return NOERROR;
}

ECode PagerTitleStrip::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 l,
    /* [in] */ Int32 t,
    /* [in] */ Int32 r,
    /* [in] */ Int32 b)
{
    if (mPager != NULL) {
        Float offset = mLastKnownPositionOffset >= 0 ? mLastKnownPositionOffset : 0;
        UpdateTextPositions(mLastKnownCurrentPage, offset, TRUE);
    }
    return NOERROR;
}

Int32 PagerTitleStrip::GetMinHeight()
{
    Int32 minHeight = 0;
    AutoPtr<IDrawable> bg;
    GetBackground((IDrawable**)&bg);
    if (bg != NULL) {
        bg->GetIntrinsicHeight(&minHeight);
    }
    return minHeight;
}

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos