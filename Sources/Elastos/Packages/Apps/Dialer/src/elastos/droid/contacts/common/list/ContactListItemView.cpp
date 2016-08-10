
#include "Elastos.Droid.Content.h"
#include "elastos/droid/contacts/common/ContactPresenceIconUtil.h"
#include "elastos/droid/contacts/common/ContactStatusUtil.h"
#include "elastos/droid/contacts/common/list/ContactListItemView.h"
#include "elastos/droid/contacts/common/util/ViewUtil.h"
#include "elastos/droid/contacts/common/util/SearchUtil.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/Math.h>
#include <elastos/core/Character.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include "elastos/droid/R.h"
#include "R.h"

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Database::CCharArrayBuffer;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Provider::IContactsContract;
using Elastos::Droid::Provider::IContacts;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::ISpannableString;
using Elastos::Droid::Text::CSpannableString;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::Text::TextUtilsTruncateAt_MARQUEE;
using Elastos::Droid::View::IGravity;
using Elastos::Droid::Widget::ImageViewScaleType_FIT_CENTER;
using Elastos::Droid::Widget::EIID_ISelectionBoundsAdjuster;
using Elastos::Droid::Widget::CTextView;
using Elastos::Droid::Widget::CImageView;
using Elastos::Core::CString;
using Elastos::Core::Character;
using Elastos::Core::StringBuilder;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

AutoPtr<IPattern> InitSplitPattern()
{
    AutoPtr<IPattern> pattern;
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    helper->Compile(String("([\\w-\\.]+)@((?:[\\w]+\\.)+)([a-zA-Z]{2,4})|[\\w]+"), (IPattern**)&pattern);
    return pattern;
}
const AutoPtr<IPattern> ContactListItemView::SPLIT_PATTERN = InitSplitPattern();

ContactListItemView::ContactListItemView()
    : mPreferredHeight(0)
    , mGapBetweenImageAndText(0)
    , mGapBetweenLabelAndData(0)
    , mPresenceIconMargin(4)
    , mPresenceIconSize(16)
    , mTextIndent(0)
    , mTextOffsetTop(0)
    , mNameTextViewTextSize(0)
    , mHeaderWidth(0)
    , mLeftOffset(0)
    , mRightOffset(0)
    , mLabelViewWidthWeight(3)
    , mDataViewWidthWeight(5)
    , mIsSectionHeaderEnabled(FALSE)
    , mQuickContactEnabled(TRUE)
    , mDefaultPhotoViewSize(0)
    , mPhotoViewWidth(0)
    , mPhotoViewHeight(0)
    , mKeepHorizontalPaddingForPhotoView(FALSE)
    , mKeepVerticalPaddingForPhotoView(IColor::BLACK)
    , mPhotoViewWidthAndHeightAreReady(FALSE)
    , mNameTextViewHeight(FALSE)
    , mNameTextViewTextColor(FALSE)
    , mPhoneticNameTextViewHeight(FALSE)
    , mLabelViewHeight(FALSE)
    , mDataViewHeight(FALSE)
    , mSnippetTextViewHeight(FALSE)
    , mStatusTextViewHeight(FALSE)
    , mLabelAndDataViewMaxHeight(FALSE)
    , mActivatedStateSupported(FALSE)
    , mAdjustSelectionBoundsEnabled(TRUE)
{
    mPhotoPosition = GetDefaultPhotoPosition(FALSE /* normal/non opposite */);
    CCharArrayBuffer::New(128, (ICharArrayBuffer**)&mDataBuffer);
    CCharArrayBuffer::New(128, (ICharArrayBuffer**)&mPhoneticNameBuffer);
    CRect::New((IRect**)&mBoundsWithoutHeader);
}

CAR_INTERFACE_IMPL(ContactListItemView, ViewGroup, ISelectionBoundsAdjuster)

PhotoPosition ContactListItemView::GetDefaultPhotoPosition(
    /* [in] */ Boolean opposite)
{
    AutoPtr<ILocale> locale;
    AutoPtr<ILocaleHelper> localehelper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&localehelper);
    localehelper->GetDefault((ILocale**)&locale);
    Int32 layoutDirection = TextUtils::GetLayoutDirectionFromLocale(locale);
    switch (layoutDirection) {
        case IView::LAYOUT_DIRECTION_RTL:
            return (opposite ? PhotoPosition_LEFT : PhotoPosition_RIGHT);
        case IView::LAYOUT_DIRECTION_LTR:
        default:
            return (opposite ? PhotoPosition_RIGHT : PhotoPosition_LEFT);
    }
}

ECode ContactListItemView::constructor(
    /* [in ] */ IContext* context)
{
    FAIL_RETURN(ViewGroup::constructor(context))
    mTextHighlighter = new TextHighlighter(ITypeface::BOLD);
    return NOERROR;
}

ECode ContactListItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    FAIL_RETURN(ViewGroup::constructor(context, attrs))

    // Read all style values
    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ITypedArray> a;
    ctx->ObtainStyledAttributes(attrs, Elastos::Droid::Dialer::R::styleable::ContactListItemView,
            (ITypedArray**)&a);
    a->GetDimensionPixelSize(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_height,
            mPreferredHeight, &mPreferredHeight);
    a->GetDrawable(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_activated_background,
            (IDrawable**)&mActivatedBackgroundDrawable);

    a->GetDimensionPixelOffset(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_gap_between_image_and_text,
            mGapBetweenImageAndText, &mGapBetweenImageAndText);
    a->GetDimensionPixelOffset(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_gap_between_label_and_data,
            mGapBetweenLabelAndData, &mGapBetweenLabelAndData);
    a->GetDimensionPixelOffset(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_presence_icon_margin,
            mPresenceIconMargin, &mPresenceIconMargin);
    a->GetDimensionPixelOffset(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_presence_icon_size,
            mPresenceIconSize, &mPresenceIconSize);
    a->GetDimensionPixelOffset(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_photo_size,
            mDefaultPhotoViewSize, &mDefaultPhotoViewSize);
    a->GetDimensionPixelOffset(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_text_indent,
            mTextIndent, &mTextIndent);
    a->GetDimensionPixelOffset(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_text_offset_top,
            mTextOffsetTop, &mTextOffsetTop);
    a->GetInteger(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_data_width_weight,
            mDataViewWidthWeight, &mDataViewWidthWeight);
    a->GetInteger(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_label_width_weight,
            mLabelViewWidthWeight, &mLabelViewWidthWeight);
    a->GetColor(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_name_text_color,
            mNameTextViewTextColor, &mNameTextViewTextColor);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Float size;
    res->GetDimension(Elastos::Droid::Dialer::R::dimen::contact_browser_list_item_text_size, &size);
    a->GetDimension(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_name_text_size,
            (Int32)size, (Float*)&mNameTextViewTextSize);

    Int32 left, top, right, bottom;
    a->GetDimensionPixelOffset(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_padding_left,
            0, &left),
    a->GetDimensionPixelOffset(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_padding_top,
            0, &top),
    a->GetDimensionPixelOffset(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_padding_right,
            0, &right),
    a->GetDimensionPixelOffset(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_padding_bottom,
            0, &bottom)

    SetPaddingRelative(left, top, right, bottom);

    mTextHighlighter = new TextHighlighter(ITypeface::BOLD);

    a->Recycle();

    AutoPtr<ITypedArray> ta;
    ctx->ObtainStyledAttributes(Elastos::Droid::Dialer::R::styleable::Theme, (ITypedArray**)&ta);
    a = ta;
    a->GetColorStateList(Elastos::Droid::Dialer::R::styleable::Theme_android_textColorSecondary,
            (IColorStateList**)&mSecondaryTextColor);
    a->Recycle();

    res->GetDimensionPixelSize(
            Elastos::Droid::Dialer::R::dimen::contact_list_section_header_width, &mHeaderWidth);

    if (mActivatedBackgroundDrawable != NULL) {
        mActivatedBackgroundDrawable->SetCallback(IDrawableCallback::Probe(this));
    }

    SetLayoutDirection(IView::LAYOUT_DIRECTION_LOCALE);
    return NOERROR;
}

void ContactListItemView::SetUnknownNameText(
    /* [in] */ ICharSequence* unknownNameText)
{
    mUnknownNameText = unknownNameText;
}

void ContactListItemView::SetQuickContactEnabled(
    /* [in] */ Boolean flag)
{
    mQuickContactEnabled = flag;
}

ECode ContactListItemView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // We will match parent's width and wrap content vertically, but make sure
    // height is no less than listPreferredItemHeight.
    Int32 specWidth;
    ResolveSize(0, widthMeasureSpec, &specWidth);
    Int32 preferredHeight = mPreferredHeight;

    mNameTextViewHeight = 0;
    mPhoneticNameTextViewHeight = 0;
    mLabelViewHeight = 0;
    mDataViewHeight = 0;
    mLabelAndDataViewMaxHeight = 0;
    mSnippetTextViewHeight = 0;
    mStatusTextViewHeight = 0;

    EnsurePhotoViewSize();

    // Width each TextView is able to use.
    Int32 effectiveWidth;
    // All the other Views will honor the photo, so available width for them may be shrunk.
    if (mPhotoViewWidth > 0 || mKeepHorizontalPaddingForPhotoView) {
        Int32 paddingLeft, paddingRight;
        GetPaddingLeft(&paddingLeft);
        GetPaddingRight(&paddingRight);
        effectiveWidth = specWidth - paddingLeft - paddingRight
                - (mPhotoViewWidth + mGapBetweenImageAndText);
    }
    else {
        Int32 paddingLeft, paddingRight;
        GetPaddingLeft(&paddingLeft);
        GetPaddingRight(&paddingRight);
        effectiveWidth = specWidth - paddingLeft - paddingRight;
    }

    if (mIsSectionHeaderEnabled) {
        effectiveWidth -= mHeaderWidth + mGapBetweenImageAndText;
    }

    // Go over all visible text views and measure actual width of each of them.
    // Also calculate their heights to get the total height for this entire view.

    AutoPtr<IView> view = IView::Probe(mNameTextView);
    if (IsVisible(view)) {
        // Calculate width for name text - this parallels similar measurement in onLayout.
        Int32 nameTextWidth = effectiveWidth;
        if (mPhotoPosition != PhotoPosition.LEFT) {
            nameTextWidth -= mTextIndent;
        }
        view->Measure(
                MeasureSpec::MakeMeasureSpec(nameTextWidth, MeasureSpec::EXACTLY),
                MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED));
        view->GetMeasuredHeight(&mNameTextViewHeight);
    }

    view = IView::Probe(mPhoneticNameTextView);
    if (IsVisible(view)) {
        view->Measure(
                MeasureSpec::MakeMeasureSpec(effectiveWidth, MeasureSpec::EXACTLY),
                MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED));
        view->GetMeasuredHeight(&mPhoneticNameTextViewHeight);
    }

    // If both data (phone number/email address) and label (type like "MOBILE") are quite long,
    // we should ellipsize both using appropriate ratio.
    Int32 dataWidth;
    Int32 labelWidth;
    view = IView::Probe(mDataView);
    if (IsVisible(view)) {
        if (IsVisible(IView::Probe(mLabelView))) {
            Int32 totalWidth = effectiveWidth - mGapBetweenLabelAndData;
            dataWidth = ((totalWidth * mDataViewWidthWeight)
                    / (mDataViewWidthWeight + mLabelViewWidthWeight));
            labelWidth = ((totalWidth * mLabelViewWidthWeight) /
                    (mDataViewWidthWeight + mLabelViewWidthWeight));
        }
        else {
            dataWidth = effectiveWidth;
            labelWidth = 0;
        }
    }
    else {
        dataWidth = 0;
        if (IsVisible(IView::Probe(mLabelView))) {
            labelWidth = effectiveWidth;
        }
        else {
            labelWidth = 0;
        }
    }

    if (IsVisible(view)) {
        view->Measure(MeasureSpec::MakeMeasureSpec(dataWidth, MeasureSpec::EXACTLY),
                MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED));
        view->GetMeasuredHeight(&mDataViewHeight);
    }

    view = IView::Probe(mLabelView);
    if (IsVisible(view)) {
        // For performance reason we don't want AT_MOST usually, but when the picture is
        // on right, we need to use it anyway because mDataView is next to mLabelView.
        Int32 mode = (mPhotoPosition == PhotoPosition_LEFT
                ? MeasureSpec::EXACTLY : MeasureSpec::AT_MOST);
        view->Measure(MeasureSpec::MakeMeasureSpec(labelWidth, mode),
                MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED));
        view->GetMeasuredHeight(&mLabelViewHeight);
    }
    mLabelAndDataViewMaxHeight = Elastos::Core::Math::Max(mLabelViewHeight, mDataViewHeight);

    view = IView::Probe(mSnippetView);
    if (IsVisible(view)) {
        view->Measure(
                MeasureSpec::MakeMeasureSpec(effectiveWidth, MeasureSpec::EXACTLY),
                MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED));
        view->GetMeasuredHeight(&mSnippetTextViewHeight);
    }

    // Status view height is the biggest of the text view and the presence icon
    view = IView::Probe(mPresenceIcon);
    if (IsVisible(view)) {
        view->Measure(
                MeasureSpec::MakeMeasureSpec(mPresenceIconSize, MeasureSpec::EXACTLY),
                MeasureSpec::MakeMeasureSpec(mPresenceIconSize, MeasureSpec::EXACTLY));
        mPresenceIcon->GetMeasuredHeight(&mStatusTextViewHeight);
    }

    view = IView::Probe(mStatusView);
    if (IsVisible(view)) {
        // Presence and status are in a same row, so status will be affected by icon size.
        Int32 statusWidth;
        if (IsVisible(IView::Probe(mPresenceIcon))) {
            Int32 w;
            IView::Probe(mPresenceIcon)->GetMeasuredWidth(&w);
            statusWidth = (effectiveWidth - w - mPresenceIconMargin);
        }
        else {
            statusWidth = effectiveWidth;
        }
        view->Measure(MeasureSpec::MakeMeasureSpec(statusWidth, MeasureSpec::EXACTLY),
                MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED));
        Int32 h;
        view->GetMeasuredHeight(&h);
        mStatusTextViewHeight = Elastos::Core::Math::Max(mStatusTextViewHeight, h);
    }

    // Calculate height including padding.
    Int32 height = (mNameTextViewHeight + mPhoneticNameTextViewHeight +
            mLabelAndDataViewMaxHeight +
            mSnippetTextViewHeight + mStatusTextViewHeight);

    // Make sure the height is at least as high as the photo
    Int32 paddingB, paddingT;
    GetPaddingBottom(&paddingB);
    GetPaddingTop(&paddingT);
    height = Elastos::Core::Math::Max(height, mPhotoViewHeight + paddingB + paddingT);

    // Make sure height is at least the preferred height
    height = Elastos::Core::Math::Max(height, preferredHeight);

    // Measure the header if it is visible.
    Int32 visibility;
    if (mHeaderTextView != NULL &&
            (IView::Probe(mHeaderTextView)->GetVisibility(&visibility), visibility == VISIBLE)) {
        IView::Probe(mHeaderTextView)->Measure(
                MeasureSpec::MakeMeasureSpec(mHeaderWidth, MeasureSpec::EXACTLY),
                MeasureSpec::MakeMeasureSpec(0, MeasureSpec::UNSPECIFIED));
    }

    SetMeasuredDimension(specWidth, height);
    return NOERROR;
}

ECode ContactListItemView::OnLayout(
    /* [in] */ Boolean changed,
    /* [in] */ Int32 left,
    /* [in] */ Int32 top,
    /* [in] */ Int32 right,
    /* [in] */ Int32 bottom)
{
    Int32 height = bottom - top;
    Int32 width = right - left;

    // Determine the vertical bounds by laying out the header first.
    Int32 topBound = 0;
    Int32 bottomBound = height;
    Int32 leftBound;
    GetPaddingLeft(&leftBound);
    Int32 paddingR;
    GetPaddingRight(&paddingR);
    Int32 rightBound = width - paddingR;

    Boolean isLayoutRtl = ViewUtil::IsViewLayoutRtl(this);

    // Put the section header on the left side of the contact view.
    if (mIsSectionHeaderEnabled) {
        if (mHeaderTextView != NULL) {
            AutoPtr<IView> view = IView::Probe(mHeaderTextView);
            Int32 headerHeight;
            view->GetMeasuredHeight(&headerHeight);
            Int32 headerTopBound = (bottomBound + topBound - headerHeight) / 2 + mTextOffsetTop;

            view->Layout(
                    isLayoutRtl ? rightBound - mHeaderWidth : leftBound,
                    headerTopBound,
                    isLayoutRtl ? rightBound : leftBound + mHeaderWidth,
                    headerTopBound + headerHeight);
        }
        if (isLayoutRtl) {
            rightBound -= mHeaderWidth;
        }
        else {
            leftBound += mHeaderWidth;
        }
    }

    mBoundsWithoutHeader->Set(left + leftBound, topBound, left + rightBound, bottomBound);
    mLeftOffset = left + leftBound;
    mRightOffset = left + rightBound;
    if (mIsSectionHeaderEnabled) {
        if (isLayoutRtl) {
            rightBound -= mGapBetweenImageAndText;
        }
        else {
            leftBound += mGapBetweenImageAndText;
        }
    }

    Boolean isActivated;
    if (mActivatedStateSupported && (IsActivated(&isActivated), isActivated)) {
        mActivatedBackgroundDrawable->SetBounds(mBoundsWithoutHeader);
    }

    AutoPtr<IView> photoView = mQuickContact != NULL ? IView::Probe(mQuickContact) : IView::Probe(mPhotoView);
    if (mPhotoPosition == PhotoPosition_LEFT) {
        // Photo is the left most view. All the other Views should on the right of the photo.
        if (photoView != NULL) {
            // Center the photo vertically
            Int32 photoTop = topBound + (bottomBound - topBound - mPhotoViewHeight) / 2;
            photoView->Layout(
                    leftBound,
                    photoTop,
                    leftBound + mPhotoViewWidth,
                    photoTop + mPhotoViewHeight);
            leftBound += mPhotoViewWidth + mGapBetweenImageAndText;
        }
        else if (mKeepHorizontalPaddingForPhotoView) {
            // Draw nothing but keep the padding.
            leftBound += mPhotoViewWidth + mGapBetweenImageAndText;
        }
    }
    else {
        // Photo is the right most view. Right bound should be adjusted that way.
        if (photoView != NULL) {
            // Center the photo vertically
            Int32 photoTop = topBound + (bottomBound - topBound - mPhotoViewHeight) / 2;
            photoView->Layout(
                    rightBound - mPhotoViewWidth,
                    photoTop,
                    rightBound,
                    photoTop + mPhotoViewHeight);
            rightBound -= (mPhotoViewWidth + mGapBetweenImageAndText);
        }
        else if (mKeepHorizontalPaddingForPhotoView) {
            // Draw nothing but keep the padding.
            rightBound -= (mPhotoViewWidth + mGapBetweenImageAndText);
        }

        // Add indent between left-most padding and texts.
        leftBound += mTextIndent;
    }

    // Center text vertically, then apply the top offset.
    Int32 totalTextHeight = mNameTextViewHeight + mPhoneticNameTextViewHeight +
            mLabelAndDataViewMaxHeight + mSnippetTextViewHeight + mStatusTextViewHeight;
    Int32 textTopBound = (bottomBound + topBound - totalTextHeight) / 2 + mTextOffsetTop;

    // Layout all text view and presence icon
    // Put name TextView first
    AutoPtr<IView> view = IView::Probe(mNameTextView);
    if (IsVisible(view)) {
        view->Layout(leftBound,
                textTopBound,
                rightBound,
                textTopBound + mNameTextViewHeight);
        textTopBound += mNameTextViewHeight;
    }

    // Presence and status
    if (isLayoutRtl) {
        Int32 statusRightBound = rightBound;
        view = IView::Probe(mPresenceIcon);
        if (IsVisible(view)) {
            Int32 iconWidth;
            view->GetMeasuredWidth(&iconWidth);
            view->Layout(
                    rightBound - iconWidth,
                    textTopBound,
                    rightBound,
                    textTopBound + mStatusTextViewHeight);
            statusRightBound -= (iconWidth + mPresenceIconMargin);
        }

        view = IView::Probe(mStatusView);
        if (IsVisible(view)) {
            view->Layout(leftBound,
                    textTopBound,
                    statusRightBound,
                    textTopBound + mStatusTextViewHeight);
        }
    }
    else {
        Int32 statusLeftBound = leftBound;
        view = IView::Probe(mPresenceIcon);
        if (IsVisible(view)) {
            Int32 iconWidth;
            view->GetMeasuredWidth(&iconWidth);
            view->Layout(
                    leftBound,
                    textTopBound,
                    leftBound + iconWidth,
                    textTopBound + mStatusTextViewHeight);
            statusLeftBound += (iconWidth + mPresenceIconMargin);
        }

        view = IView::Probe(mStatusView);
        if (IsVisible(view)) {
            view->Layout(statusLeftBound,
                    textTopBound,
                    rightBound,
                    textTopBound + mStatusTextViewHeight);
        }
    }

    if (IsVisible(IView::Probe(mStatusView)) || isVisible(IView::Probe(mPresenceIcon))) {
        textTopBound += mStatusTextViewHeight;
    }

    // Rest of text views
    Int32 dataLeftBound = leftBound;
    view = IView::Probe(mPhoneticNameTextView);
    if (isVisible(view)) {
        view->Layout(leftBound,
                textTopBound,
                rightBound,
                textTopBound + mPhoneticNameTextViewHeight);
        textTopBound += mPhoneticNameTextViewHeight;
    }

    // Label and Data align bottom.
    view = IView::Probe(mLabelView);
    if (IsVisible(view)) {
        if (mPhotoPosition == PhotoPosition_LEFT) {
            // When photo is on left, label is placed on the right edge of the list item.
            Int32 w;
            view->GetMeasuredWidth(&w);
            view->Layout(rightBound - w,
                    textTopBound + mLabelAndDataViewMaxHeight - mLabelViewHeight,
                    rightBound,
                    textTopBound + mLabelAndDataViewMaxHeight);
            rightBound -= w;
        }
        else {
            // When photo is on right, label is placed on the left of data view.
            Int32 w;
            view->GetMeasuredWidth(&w);
            dataLeftBound = leftBound + w;
            view->Layout(leftBound,
                    textTopBound + mLabelAndDataViewMaxHeight - mLabelViewHeight,
                    dataLeftBound,
                    textTopBound + mLabelAndDataViewMaxHeight);
            dataLeftBound += mGapBetweenLabelAndData;
        }
    }

    view = IView::Probe(mDataView);
    if (IsVisible(view)) {
        view->Layout(dataLeftBound,
                textTopBound + mLabelAndDataViewMaxHeight - mDataViewHeight,
                rightBound,
                textTopBound + mLabelAndDataViewMaxHeight);
    }
    if (IsVisible(IView::Probe(mLabelView)) || IsVisible(IView::Probe(mDataView))) {
        textTopBound += mLabelAndDataViewMaxHeight;
    }

    view = IView::Probe(mSnippetView);
    if (IsVisible(view)) {
        view->Layout(leftBound,
                textTopBound,
                rightBound,
                textTopBound + mSnippetTextViewHeight);
    }
    return NOERROR;
}

ECode ContactListItemView::AdjustListItemSelectionBounds(
    /* [in] */ IRect* bounds)
{
    if (mAdjustSelectionBoundsEnabled) {
        Int32 top;
        bounds->GetTop(&top);
        Int32 headerTop;
        mBoundsWithoutHeader->GetTop(&headerTop);
        bounds->SetTop(top + headerTop);
        bounds->GetTop(&top);
        Int32 height;
        mBoundsWithoutHeader->GetHeight(&height);
        bounds->SetBottom(top + height);
        Int32 headerLeft;
        mBoundsWithoutHeader->GetLeft(&headerLeft);
        bounds->SetLeft(headerLeft);
        Int32 headerRight;
        mBoundsWithoutHeader->GetRight(&headerRight);
        bounds->SetRight(headerRight);
    }
    return NOERROR;
}

Boolean ContactListItemView::IsVisible(
    /* [in] */ IView* view)
{
    Int32 visibility;
    return view != NULL && (view->GetVisibility(&visibility), visibility == IView::VISIBLE);
}

void ContactListItemView::EnsurePhotoViewSize()
{
    if (!mPhotoViewWidthAndHeightAreReady) {
        mPhotoViewWidth = mPhotoViewHeight = GetDefaultPhotoViewSize();
        if (!mQuickContactEnabled && mPhotoView == NULL) {
            if (!mKeepHorizontalPaddingForPhotoView) {
                mPhotoViewWidth = 0;
            }
            if (!mKeepVerticalPaddingForPhotoView) {
                mPhotoViewHeight = 0;
            }
        }

        mPhotoViewWidthAndHeightAreReady = TRUE;
    }
}

Int32 ContactListItemView::GetDefaultPhotoViewSize()
{
    return mDefaultPhotoViewSize;
}

AutoPtr<IViewGroupLayoutParams> ContactListItemView::GetDefaultPhotoLayoutParams()
{
    AutoPtr<IViewGroupLayoutParams> params;
    GenerateDefaultLayoutParams((IViewGroupLayoutParams**)&params);
    params->SetWidth(GetDefaultPhotoViewSize());
    Int32 width;
    params->GetWidth(&width);
    params->SetHeight(width);
    return params;
}

ECode ContactListItemView::DrawableStateChanged()
{
    FAIL_RETURN(ViewGroup::DrawableStateChanged())
    if (mActivatedStateSupported) {
        AutoPtr<ArrayOf<Int32> > state;
        GetDrawableState((ArrayOf<Int32>**)&state);
        Boolean result;
        mActivatedBackgroundDrawable->SetState(state, &result);
    }
    return NOERROR;
}

Boolean ContactListItemView::VerifyDrawable(
    /* [in] */ IDrawable* who)
{
    return who == mActivatedBackgroundDrawable.Get() || ViewGroup::VerifyDrawable(who);
}

ECode ContactListItemView::JumpDrawablesToCurrentState()
{
    FAIL_RETURN(ViewGroup::JumpDrawablesToCurrentState())
    if (mActivatedStateSupported) {
        mActivatedBackgroundDrawable->JumpToCurrentState();
    }
    return NOERROR;
}

ECode ContactListItemView::DispatchDraw(
    /* [in] */ ICanvas* canvas)
{
    Boolean isActivated;
    if (mActivatedStateSupported && (IsActivated(&isActivated), isActivated)) {
        mActivatedBackgroundDrawable->Draw(canvas);
    }

    return ViewGroup::DispatchDraw(canvas);
}

void ContactListItemView::SetSectionHeader(
    /* [in] */ const String& title)
{
    if (!TextUtils::IsEmpty(title)) {
        if (mHeaderTextView == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            AutoPtr<ITextView> temp;
            CTextView::New(context, (ITextView**)&temp);
            mHeaderTextView = temp;
            mHeaderTextView->SetTextAppearance(context, Elastos::Droid::Dialer::R::style::SectionHeaderStyle);
            mHeaderTextView->SetGravity(
                    ViewUtil::IsViewLayoutRtl(IView::Probe(this)) ? IGravity::RIGHT : IGravity::LEFT);
            AddView(IView::Probe(mHeaderTextView));
        }
        AutoPtr<ICharSequence> cs;
        CString::New(title, (ICharSequence**)&cs)
        SetMarqueeText(mHeaderTextView, cs);
        IView::Probe(mHeaderTextView)->SetVisibility(IView::VISIBLE);
        mHeaderTextView->SetAllCaps(TRUE);
    }
    else if (mHeaderTextView != NULL) {
        IView::Probe(mHeaderTextView)->SetVisibility(IView::GONE);
    }
}

void ContactListItemView::SetIsSectionHeaderEnabled(
    /* [in] */ Boolean isSectionHeaderEnabled)
{
    mIsSectionHeaderEnabled = isSectionHeaderEnabled;
}

ECode ContactListItemView::GetQuickContact(
    /* [out] */ IQuickContactBadge** badge)
{
    valiVALIDATE_NOT_NULL(badge)
    *badge = NULL;
    if (!mQuickContactEnabled) {
        Logger::E("ContactListItemView", "QuickContact is disabled for this view");
        return E_ILLEGAL_STATE_EXCEPTION;
    }
    if (mQuickContact == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CQuickContactBadge::New(context, (IQuickContactBadge**)&mQuickContact);
        mQuickContact->SetOverlay(NULL);
        AutoPtr<IViewGroupLayoutParams> params = GetDefaultPhotoLayoutParams();
        IView::Probe(mQuickContact)->SetLayoutParams(params);
        if (mNameTextView != NULL) {
            AutoPtr<ICharSequence> cs;
            mNameTextView->GetText((ICharSequence**)&cs);
            String text;
            cs->ToString(&text);
            String str;
            context->GetString(Elastos::Droid::Dialer::R::string::description_quick_contact_for,
                    text, &str);
            AutoPtr<ICharSequence> desc;
            CString::New(str, (ICharSequence**)&desc);
            IView::Probe(mQuickContact)->SetContentDescription(desc);
        }

        AddView(IView::Probe(mQuickContact));
        mPhotoViewWidthAndHeightAreReady = FALSE;
    }
    *badge = mQuickContact;
    REFCOUNT_ADD(*badge)
    return NOERROR;
}

AutoPtr<IImageView> ContactListItemView::GetPhotoView()
{
    if (mPhotoView == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CImageView::New(context, (IImageView**)&mPhotoView);
        AutoPtr<IViewGroupLayoutParams> params = GetDefaultPhotoLayoutParams();
        AutoPtr<IView> view = IView::Probe(mPhotoView);
        view->SetLayoutParams(params);
        // Quick contact style used above will set a background - remove it
        view->SetBackground(NULL);
        AddView(view);
        mPhotoViewWidthAndHeightAreReady = FALSE;
    }
    return mPhotoView;
}

void ContactListItemView::RemovePhotoView()
{
    RemovePhotoView(FALSE, TRUE);
}

void ContactListItemView::RemovePhotoView(
    /* [in] */ Boolean keepHorizontalPadding,
    /* [in] */ Boolean keepVerticalPadding)
{
    mPhotoViewWidthAndHeightAreReady = FALSE;
    mKeepHorizontalPaddingForPhotoView = keepHorizontalPadding;
    mKeepVerticalPaddingForPhotoView = keepVerticalPadding;
    if (mPhotoView != NULL) {
        RemoveView(IView::Probe(mPhotoView));
        mPhotoView = NULL;
    }
    if (mQuickContact != NULL) {
        RemoveView(IView::Probe(mQuickContact));
        mQuickContact = NULL;
    }
}

void ContactListItemView::SetHighlightedPrefix(
    /* [in] */ const String& upperCasePrefix)
{
    mHighlightedPrefix = upperCasePrefix;
}

void ContactListItemView::ClearHighlightSequences()
{
    mNameHighlightSequence.Clear();
    mNumberHighlightSequence.Clear();
    mHighlightedPrefix = NULL;
}

void ContactListItemView::AddNameHighlightSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<HighlightSequence> sequence = new HighlightSequence(start, end);
    mNameHighlightSequence.PushBack(sequence);
}

void ContactListItemView::AddNumberHighlightSequence(
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    AutoPtr<HighlightSequence> sequence = new HighlightSequence(start, end);
    mNumberHighlightSequence.PushBack(sequence);
}

AutoPtr<ITextView> ContactListItemView::GetNameTextView()
{
    if (mNameTextView == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CTextView::New(context, (ITextView**)&mNameTextView);
        mNameTextView->SetSingleLine(TRUE);
        mNameTextView->SetEllipsize(GetTextEllipsis());
        mNameTextView->SetTextColor(mNameTextViewTextColor);
        mNameTextView->SetTextSize(ITypedValue::COMPLEX_UNIT_PX,
                mNameTextViewTextSize);
        // Manually call setActivated() since this view may be added after the first
        // setActivated() call toward this whole item view.
        AutoPtr<IView> view = IView::Probe(mNameTextView);
        Boolean isActivated;
        IsActivated(&isActivated);
        view->SetActivated(isActivated);
        mNameTextView->SetGravity(IGravity::CENTER_VERTICAL);
        view->SetTextAlignment(IView::TEXT_ALIGNMENT_VIEW_START);
        view->SetId(Elastos::Droid::Dialer::R::id::cliv_name_textview);
        mNameTextView->SetElegantTextHeight(FALSE);
        AddView(view);
    }
    return mNameTextView;
}

void ContactListItemView::SetPhoneticName(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 size)
{
    if (text == NULL || size == 0) {
        if (mPhoneticNameTextView != NULL) {
            IView::Probe(mPhoneticNameTextView)->SetVisibility(IView::GONE);
        }
    }
    else {
        GetPhoneticNameTextView();
        SetMarqueeText(mPhoneticNameTextView, text, size);
        IView::Probe(mPhoneticNameTextView)->SetVisibility(VISIBLE);
    }
}

AutoPtr<ITextView> ContactListItemView::GetPhoneticNameTextView()
{
    if (mPhoneticNameTextView == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CTextView::New(context, (ITextView**)&mPhoneticNameTextView);
        AutoPtr<IView> view = IView::Probe(mPhoneticNameTextView);
        mPhoneticNameTextView->SetSingleLine(TRUE);
        mPhoneticNameTextView->SetEllipsize(GetTextEllipsis());
        mPhoneticNameTextView->SetTextAppearance(context, Elastos::Droid::R::style::TextAppearance_Small);
        AutoPtr<ITypeface> typeface;
        mPhoneticNameTextView->GetTypeface((ITypeface**)&typeface);
        mPhoneticNameTextView->SetTypeface(typeface, ITypeface::BOLD);
        Boolean isActivated;
        IsActivated(&isActivated);
        view->SetActivated(isActivated);
        view->SetId(Elastos::Droid::Dialer::R::id::cliv_phoneticname_textview);
        AddView(view);
    }
    return mPhoneticNameTextView;
}

void ContactListItemView::SetLabel(
    /* [in] */ ICharSequence* text)
{
    if (TextUtils::IsEmpty(text)) {
        if (mLabelView != NULL) {
            IView::Probe(mLabelView)->SetVisibility(IView::GONE);
        }
    }
    else {
        GetLabelView();
        SetMarqueeText(mLabelView, text);
        IView::Probe(mLabelView)->SetVisibility(VISIBLE);
    }
}

AutoPtr<ITextView> ContactListItemView::GetLabelView()
{
    if (mLabelView == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CTextView::New(context, (ITextView**)&mLabelView);
        mLabelView->SetSingleLine(TRUE);
        mLabelView->SetEllipsize(GetTextEllipsis());
        mLabelView->SetTextAppearance(context, Elastos::Droid::Dialer::R::style::TextAppearanceSmall);
        if (mPhotoPosition == PhotoPosition_LEFT) {
            mLabelView->SetAllCaps(TRUE);
            mLabelView->SetGravity(IGravity::END);
        }
        else {
            AutoPtr<ITypeface> typeface;
            mLabelView->GetTypeface((ITypeface**)&typeface);
            mLabelView->SetTypeface(typeface, ITypeface::BOLD);
        }
        AutoPtr<IView> view = IView::Probe(mLabelView);
        Boolean isActivated;
        IsActivated(&isActivated);
        view->SetActivated(isActivated);
        view->SetId(Elastos::Droid::Dialer::R::id::cliv_label_textview);
        AddView(view);
    }
    return mLabelView;
}

void ContactListItemView::SetData(
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 size)
{
    if (text == NULL || size == 0) {
        if (mDataView != NULL) {
            IView::Probe(mDataView)->SetVisibility(IView::GONE);
        }
    }
    else {
        GetDataView();
        SetMarqueeText(mDataView, text, size);
        IView::Probe(mDataView)->SetVisibility(VISIBLE);
    }
}

void ContactListItemView::SetPhoneNumber(
    /* [in] */ const String& text,
    /* [in] */ const String& countryIso)
{
    if (text.IsNull()) {
        if (mDataView != NULL) {
            IView::Probe(mDataView)->SetVisibility(IView::GONE);
        }
    }
    else {
        GetDataView();

        // TODO: Format number using PhoneNumberUtils.formatNumber before assigning it to
        // mDataView. Make sure that determination of the highlight sequences are done only
        // after number formatting.

        // Sets phone number texts for display after highlighting it, if applicable.
        // CharSequence textToSet = text;
        AutoPtr<ISpannableString> textToSet;
        CSpannableString::New(text, (ISpannableString**)&textToSet);

        if (mNumberHighlightSequence.Begin() != mNumberHighlightSequence.End()) {
            AutoPtr<HighlightSequence> highlightSequence = *(mNumberHighlightSequence.Begin());
            mTextHighlighter->ApplyMaskingHighlight(textToSet, highlightSequence->mStart,
                    highlightSequence->mEnd);
        }

        SetMarqueeText(mDataView, textToSet);
        AutoPtr<IView> view = IView::Probe(mDataView);
        view->SetVisibility(VISIBLE);

        // We have a phone number as "mDataView" so make it always LTR and VIEW_START
        view->SetTextDirection(IView::TEXT_DIRECTION_LTR);
        view->SetTextAlignment(IView::TEXT_ALIGNMENT_VIEW_START);
    }
}

void ContactListItemView::SetMarqueeText(
    /* [in] */ ITextView* textView,
    /* [in] */ ArrayOf<Char32>* text,
    /* [in] */ Int32 size)
{
    if (GetTextEllipsis() == TextUtilsTruncateAt_MARQUEE) {
        SetMarqueeText(textView, String(*text, 0, size));
    }
    else {
        textView->SetText(text, 0, size);
    }
}

void ContactListItemView::SetMarqueeText(
    /* [in] */ ITextView* textView,
    /* [in] */ ICharSequence* text)
{
    if (GetTextEllipsis() == TextUtilsTruncateAt_MARQUEE) {
        // To show MARQUEE correctly (with END effect during non-active state), we need
        // to build Spanned with MARQUEE in addition to TextView's ellipsize setting.
        AutoPtr<ISpannableString> spannable;
        CSpannableString::New(text, (ISpannableString**)&spannable);
        Int32 len;
        spannable->GetLength(&len);
        spannable->SetSpan(TextUtilsTruncateAt_MARQUEE, 0, len, ISpannable::SPAN_EXCLUSIVE_EXCLUSIVE);
        textView->SetText(ICharSequence::Probe(spannable));
    }
    else {
        textView->SetText(text);
    }
}

AutoPtr<ITextView> ContactListItemView::GetDataView()
{
    if (mDataView == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CTextView::New(context, (ITextView**)&mDataView);
        mDataView->SetSingleLine(TRUE);
        mDataView->SetEllipsize(GetTextEllipsis());
        mDataView->SetTextAppearance(context, Elastos::Droid::Dialer::R::style::TextAppearanceSmall);
        AutoPtr<IView> view = IView::Probe(mDataView);
        Boolean isActivated;
        IsActivated(&isActivated);
        view->SetActivated(isActivated);
        view->SetId(Elastos::Droid::Dialer::R::id::cliv_data_view);
        mDataView->SetElegantTextHeight(FALSE);
        AddView(view);
    }
    return mDataView;
}

void ContactListItemView::SetSnippet(
    /* [in] */ const String& text)
{
    if (TextUtils::IsEmpty(text)) {
        if (mSnippetView != NULL) {
            IView::Probe(mSnippetView)->SetVisibility(IView::GONE);
        }
    }
    else {
        AutoPtr<ITextView> view = GetSnippetView();
        mTextHighlighter->SetPrefixText(view, text, mHighlightedPrefix);
        IView::Probe(mSnippetView)->SetVisibility(VISIBLE);
    }
}

AutoPtr<ITextView> ContactListItemView::GetSnippetView()
{
    if (mSnippetView == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CTextView::New(context, (ITextView**)&mSnippetView);
        mSnippetView->SetSingleLine(TRUE);
        mSnippetView->SetEllipsize(GetTextEllipsis());
        mSnippetView->SetTextAppearance(context, Elastos::Droid::Dialer::R::style::TextAppearance_Small);
        AutoPtr<IView> view = IView::Probe(mSnippetView);
        Boolean isActivated;
        IsActivated(&isActivated);
        view->SetActivated(isActivated);
        AddView(view);
    }
    return mSnippetView;
}

AutoPtr<ITextView> ContactListItemView::GetStatusView()
{
    if (mStatusView == NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        CTextView::New(context, (ITextView**)&mStatusView);
        mStatusView->SetSingleLine(TRUE);
        mStatusView->SetEllipsize(GetTextEllipsis());
        mStatusView->SetTextAppearance(context, Elastos::Droid::Dialer::R::style::TextAppearance_Small);
        mStatusView->SetTextColor(mSecondaryTextColor);
        AutoPtr<IView> view = IView::Probe(mStatusView);
        Boolean isActivated;
        IsActivated(&isActivated);
        view->SetActivated(isActivated);
        view->SetTextAlignment(IView::TEXT_ALIGNMENT_VIEW_START);
        AddView(view);
    }
    return mStatusView;
}

void ContactListItemView::SetStatus(
    /* [in] */ ICharSequence* text)
{
    if (TextUtils::IsEmpty(text)) {
        if (mStatusView != NULL) {
            IView::Probe(mStatusView)->SetVisibility(IView::GONE);
        }
    }
    else {
        GetStatusView();
        SetMarqueeText(mStatusView, text);
        IView::Probe(mStatusView)->SetVisibility(VISIBLE);
    }
}

void ContactListItemView::SetPresence(
    /* [in] */ IDrawable* icon)
{
    if (icon != NULL) {
        if (mPresenceIcon == NULL) {
            AutoPtr<IContext> context;
            GetContext((IContext**)&context);
            CImageView::New(context, (IImageView**)&mPresenceIcon);
            AddView(IView::Probe(mPresenceIcon));
        }
        mPresenceIcon->SetImageDrawable(icon);
        mPresenceIcon->SetScaleType(ImageViewScaleType_FIT_CENTER);
        IView::Probe(mPresenceIcon)->SetVisibility(View.VISIBLE);
    }
    else {
        if (mPresenceIcon != NULL) {
            IView::Probe(mPresenceIcon)->SetVisibility(IView::GONE);
        }
    }
}

TextUtilsTruncateAt ContactListItemView::GetTextEllipsis()
{
    return TextUtilsTruncateAt_MARQUEE;
}

void ContactListItemView::ShowDisplayName(
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 nameColumnIndex,
    /* [in] */ Int32 displayOrder)
{
    String str;
    cursor->GetString(nameColumnIndex, &str);
    AutoPtr<ICharSequence> name;
    CString::New(str, (ICharSequence**)&name);
    SetDisplayName(name);

    // Since the quick contact content description is derived from the display name and there is
    // no guarantee that when the quick contact is initialized the display name is already set,
    // do it here too.
    if (mQuickContact != NULL) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        AutoPtr<ICharSequence> textCS;
        mNameTextView->GetText((ICharSequence**)&textCS);
        String text;
        textCS->ToString(&text);
        context->GetString(Elastos::Droid::Dialer::R::string::description_quick_contact_for,
                text, &str);
        AutoPtr<ICharSequence> desc;
        CString::New(str, (ICharSequence**)&desc);
        IView::Probe(mQuickContact)->SetContentDescription(desc);
    }
}

void ContactListItemView::SetDisplayName(
    /* [in] */ ICharSequence* name,
    /* [in] */ Boolean highlight)
{
    if (!TextUtils::IsEmpty(name) && highlight) {
        ClearHighlightSequences();
        Int32 len;
        name->GetLength(&len);
        AddNameHighlightSequence(0, len);
    }
    SetDisplayName(name);
}

void ContactListItemView::SetDisplayName(
    /* [in] */ ICharSequence* name)
{
    if (!TextUtils::IsEmpty(name)) {
        // Chooses the available highlighting method for highlighting.
        if (mHighlightedPrefix != NULL) {
            name = mTextHighlighter->ApplyPrefixHighlight(name, mHighlightedPrefix);
        }
        else if (mNameHighlightSequence.Begin() != mNameHighlightSequence.End()) {
            AutoPtr<ISpannableString> spannableName;
            CSpannableString::New(name, (ISpannableString**)&spannableName);
            List<AutoPtr<HighlightSequence> >::Iterator it = mNameHighlightSequence.Begin();
            for (; it != mNameHighlightSequence.End(); ++it) {
                AutoPtr<HighlightSequence> highlightSequence = *it;
                mTextHighlighter->ApplyMaskingHighlight(spannableName, highlightSequence->mStart,
                        highlightSequence->mEnd);
            }
            name = ICharSequence::Probe(spannableName);
        }
    }
    else {
        name = mUnknownNameText;
    }
    SetMarqueeText(getNameTextView(), name);
}

void ContactListItemView::HideDisplayName()
{
    if (mNameTextView != NULL) {
        RemoveView(IView::Probe(mNameTextView));
        mNameTextView = NULL;
    }
}

void ContactListItemView::ShowPhoneticName(
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 phoneticNameColumnIndex)
{
    cursor->CopyStringToBuffer(phoneticNameColumnIndex, mPhoneticNameBuffer);
    Int32 phoneticNameSize;
    mPhoneticNameBuffer->GetSizeCopied(&phoneticNameSize);
    if (phoneticNameSize != 0) {
        AutoPtr<ArrayOf<Char32> > data;
        mPhoneticNameBuffer->GetData((ArrayOf<Char32>**)&data);
        SetPhoneticName(data, phoneticNameSize);
    }
    else {
        SetPhoneticName(NULL, 0);
    }
}

void ContactListItemView::HidePhoneticName()
{
    if (mPhoneticNameTextView != NULL) {
        RemoveView(IView::Probe(mPhoneticNameTextView));
        mPhoneticNameTextView = NULL;
    }
}

void ContactListItemView::ShowPresenceAndStatusMessage(
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 presenceColumnIndex,
    /* [in] */ Int32 contactStatusColumnIndex)
{
    AutoPtr<IDrawable> icon;
    Int32 presence = 0;
    Boolean isNull;
    if (cursor->IsNull(presenceColumnIndex, &isNull), !isNull) {
        cursor->GetInt32(presenceColumnIndex, &presence);
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        icon = ContactPresenceIconUtil::GetPresenceIcon(context, presence);
    }
    SetPresence(icon);

    String statusMessage(NULL);
    Boolean isNull;
    if (contactStatusColumnIndex != 0 && (cursor->IsNull(contactStatusColumnIndex, &isNull), !isNull)) {
        cursor->GetString(contactStatusColumnIndex, &statusMessage);
    }
    // If there is no status message from the contact, but there was a presence value, then use
    // the default status message string
    if (statusMessage.IsNull() && presence != 0) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        statusMessage = ContactStatusUtil::GetStatusString(context, presence);
    }
    SetStatus(statusMessage);
}

void ContactListItemView::ShowSnippet(
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 summarySnippetColumnIndex)
{
    Int32 count;
    if (cursor->GetColumnCount(&count), count <= summarySnippetColumnIndex) {
        SetSnippet(String(NULL));
        return;
    }

    String snippet;
    cursor->GetString(summarySnippetColumnIndex, *snippet);

    // Do client side snippeting if provider didn't do it
    AutoPtr<IBundle> extras;
    cursor->GetExtras((IBundle**)&extras);
    Boolean value;
    if (extras->GetBoolean(IContactsContract::DEFERRED_SNIPPETING, &value), value) {

        String query;
        extras->GetString(IContactsContract::DEFERRED_SNIPPETING_QUERY, &query);

        String displayName(NULL);
        Int32 displayNameIndex;
        cursor->GetColumnIndex(IContacts::DISPLAY_NAME, &displayNameIndex);
        if (displayNameIndex >= 0) {
            cursor->GetString(displayNameIndex, &displayName);
        }

        snippet = UpdateSnippet(snippet, query, displayName);

    }
    else {
        if (!snippet.IsNull()) {
            Int32 from = 0;
            Int32 to = snippet->GetLength();
            Int32 start = snippet.IndexOf(IDefaultContactListAdapter::SNIPPET_START_MATCH);
            if (start == -1) {
                snippet = String(NULL);
            }
            else {
                Int32 firstNl = snippet.LastIndexOf('\n', start);
                if (firstNl != -1) {
                    from = firstNl + 1;
                }
                Int32 end = snippet.LastIndexOf(IDefaultContactListAdapter::SNIPPET_END_MATCH);
                if (end != -1) {
                    Int32 lastNl = snippet.IndexOf('\n', end);
                    if (lastNl != -1) {
                        to = lastNl;
                    }
                }

                StringBuilder sb;
                for (Int32 i = from; i < to; i++) {
                    Char32 c = snippet.GetChar(i);
                    if (c != IDefaultContactListAdapter::SNIPPET_START_MATCH &&
                            c != IDefaultContactListAdapter::SNIPPET_END_MATCH) {
                        sb.AppendChar(c);
                    }
                }
                snippet = sb.ToString();
            }
        }
    }

    SetSnippet(snippet);
}

String ContactListItemView::UpdateSnippet(
    /* [in] */ const String& snippet,
    /* [in] */ const String& query,
    /* [in] */ const String& displayName)
{
    if (TextUtils::IsEmpty(snippet) || TextUtils::IsEmpty(query)) {
        return String(NULL);
    }
    query = SearchUtil::CleanStartAndEndOfSearchQuery(query.ToLowerCase());

    // If the display name already contains the query term, return empty - snippets should
    // not be needed in that case.
    if (!TextUtils::IsEmpty(displayName)) {
        String lowerDisplayName = displayName.ToLowerCase();
        AutoPtr<ArrayOf<String> > nameTokens = Split(lowerDisplayName);
        for (Int32 i = 0; i < nameTokens->GetLength(); ++i) {
            String nameToken = (*nameTokens)[i];
            if (nameToken.StartWith(query)) {
                return String(NULL);
            }
        }
    }

    // The snippet may contain multiple data lines.
    // Show the first line that matches the query.
    AutoPtr<SearchUtil::MatchedLine> matched = SearchUtil::FindMatchingLine(snippet, query);

    if (matched != NULL && !matched->mLine.IsNull()) {
        // Tokenize for long strings since the match may be at the end of it.
        // Skip this part for short strings since the whole string will be displayed.
        // Most contact strings are short so the snippetize method will be called infrequently.
        AutoPtr<IResources> res;
        GetResources((IResources**)&res);
        Int32 lengthThreshold;
        res->GetInteger(
                Elastos::Droid::Dialer::R::integer::snippet_length_before_tokenize, &lengthThreshold);
        if (matched->mLine.GetLength() > lengthThreshold) {
            return Snippetize(matched->mLine, matched->mStartIndex, lengthThreshold);
        }
        else {
            return matched->mLine;
        }
    }

    // No match found.
    return String(NULL);
}

String ContactListItemView::Snippetize(
    /* [in] */ const String& line,
    /* [in] */ Int32 matchIndex,
    /* [in] */ Int32 maxLength)
{
    // Show up to maxLength characters. But we only show full tokens so show the last full token
    // up to maxLength characters. So as many starting tokens as possible before trying ending
    // tokens.
    Int32 remainingLength = maxLength;
    Int32 tempRemainingLength = remainingLength;

    // Start the end token after the matched query.
    Int32 index = matchIndex;
    Int32 endTokenIndex = index;

    // Find the match token first.
    while (index < line.GetLength()) {
        if (!Character::IsLetterOrDigit(line.GetChar(index))) {
            endTokenIndex = index;
            remainingLength = tempRemainingLength;
            break;
        }
        tempRemainingLength--;
        index++;
    }

    // Find as much content before the match.
    index = matchIndex - 1;
    tempRemainingLength = remainingLength;
    Int32 startTokenIndex = matchIndex;
    while (index > -1 && tempRemainingLength > 0) {
        if (!Character::IsLetterOrDigit(line.GetChar(index))) {
            startTokenIndex = index;
            remainingLength = tempRemainingLength;
        }
        tempRemainingLength--;
        index--;
    }

    index = endTokenIndex;
    tempRemainingLength = remainingLength;
    // Find remaining content at after match.
    while (index < line.GetLength() && tempRemainingLength > 0) {
        if (!Character::IsLetterOrDigit(line.GetChar(index))) {
            endTokenIndex = index;
        }
        tempRemainingLength--;
        index++;
    }
    // Append ellipse if there is content before or after.
    StringBuilder sb;
    if (startTokenIndex > 0) {
        sb.Append("...");
    }
    sb.Append(line.Substring(startTokenIndex, endTokenIndex));
    if (endTokenIndex < line.GetLength()) {
        sb.Append("...");
    }
    return sb.ToString();
}

AutoPtr<ArrayOf<String> > ContactListItemView::Split(
    /* [in] */ const String& content)
{
    AutoPtr<IMatcher> matcher;
    SPLIT_PATTERN->Matcher(content, (IMatcher**)&matcher);
    AutoPtr<IMatchResult> result = IMatchResult::Probe(matcher);
    Int32 count;
    result->GroupCount(&count);
    AutoPtr<ArrayOf<String> > tokens = ArrayOf<String>::Alloc(count);
    Boolean hasNext;
    Int32 i = 0;
    while (matcher->Find(&hasNext), hasNext) {
        String str;
        matcher->Group(&str);
        (*tokens)[i] = str;
        i++;
    }
    return tokens;
}
// begin from this
void ContactListItemView::ShowData(
    /* [in] */ ICursor* cursor,
    /* [in] */ Int32 dataColumnIndex);

void ContactListItemView::SetActivatedStateSupported(
    /* [in] */ Boolean flag);

void ContactListItemView::SetAdjustSelectionBoundsEnabled(
    /* [in] */ Boolean enabled);

ECode ContactListItemView::RequestLayout();

void ContactListItemView::SetPhotoPosition(PhotoPosition photoPosition);

PhotoPosition ContactListItemView::GetPhotoPosition();

void ContactListItemView::SetDrawableResource(
    /* [in] */ Int32 backgroundId,
    /* [in] */ Int32 drawableId);

ECode ContactListItemView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* res);

Boolean ContactListItemView::PointIsInView(
    /* [in] */ Float localX,
    /* [in] */ Float localY);

} // List
} // Common
} // Contacts
} // Apps
} // Elastos