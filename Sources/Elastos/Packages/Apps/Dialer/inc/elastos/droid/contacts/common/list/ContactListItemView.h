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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTITEMVIEW_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTITEMVIEW_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/contacts/common/format/TextHighlighter.h"
#include "elastos/droid/view/ViewGroup.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Contacts::Common::Format::TextHighlighter;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::ICharArrayBuffer;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Text::TextUtilsTruncateAt;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::ISelectionBoundsAdjuster;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IQuickContactBadge;
using Elastos::Droid::Widget::IImageView;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

/**
 * A custom view for an item in the contact list.
 * The view contains the contact's photo, a set of text views (for name, status, etc...) and
 * icons for presence and call.
 * The view uses no XML file for layout and all the measurements and layouts are done
 * in the onMeasure and onLayout methods.
 *
 * The layout puts the contact's photo on the right side of the view, the call icon (if present)
 * to the left of the photo, the text lines are aligned to the left and the presence icon (if
 * present) is set to the left of the status line.
 *
 * The layout also supports a header (used as a header of a group of contacts) that is above the
 * contact's data and a divider between contact view.
 */
class ContactListItemView
    : public ViewGroup
    , public IContactListItemView
    , public ISelectionBoundsAdjuster
{
protected:
    class HighlightSequence : public Object
    {
    public:
        HighlightSequence(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end)
            : mStart(start)
            , mEnd(end)
        {}

    private:
        Int32 mStart;
        Int32 mEnd;

        friend class ContactListItemView;
    };

public:
    ContactListItemView();

    CAR_INTERFACE_DECL()

    static CARAPI_(PhotoPosition) GetDefaultPhotoPosition(
        /* [in] */ Boolean opposite);

    CARAPI constructor(
        /* [in ] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(void) SetUnknownNameText(
        /* [in] */ ICharSequence* unknownNameText);

    CARAPI_(void) SetQuickContactEnabled(
        /* [in] */ Boolean flag);

    CARAPI AdjustListItemSelectionBounds(
        /* [in] */ IRect* bounds);

    // @Override
    CARAPI JumpDrawablesToCurrentState();

    // @Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

    /**
     * Sets section header or makes it invisible if the title is null.
     */
    CARAPI_(void) SetSectionHeader(
        /* [in] */ const String& title);

    CARAPI_(void) SetIsSectionHeaderEnabled(
        /* [in] */ Boolean isSectionHeaderEnabled);

    /**
     * Returns the quick contact badge, creating it if necessary.
     */
    CARAPI GetQuickContact(
        /* [out] */ IQuickContactBadge** badge);

    /**
     * Returns the photo view, creating it if necessary.
     */
    CARAPI_(AutoPtr<IImageView>) GetPhotoView();

    /**
     * Removes the photo view.
     */
    CARAPI_(void) RemovePhotoView();

    /**
     * Removes the photo view.
     *
     * @param keepHorizontalPadding True means data on the right side will have
     *            padding on left, pretending there is still a photo view.
     * @param keepVerticalPadding True means the View will have some height
     *            enough for accommodating a photo view.
     */
    CARAPI_(void) RemovePhotoView(
        /* [in] */ Boolean keepHorizontalPadding,
        /* [in] */ Boolean keepVerticalPadding);

    /**
     * Sets a word prefix that will be highlighted if encountered in fields like
     * name and search snippet. This will disable the mask highlighting for names.
     * <p>
     * NOTE: must be all upper-case
     */
    CARAPI_(void) SetHighlightedPrefix(
        /* [in] */ const String& upperCasePrefix);

    /**
     * Clears previously set highlight sequences for the view.
     */
    CARAPI_(void) ClearHighlightSequences();

    /**
     * Adds a highlight sequence to the name highlighter.
     * @param start The start position of the highlight sequence.
     * @param end The end position of the highlight sequence.
     */
    CARAPI_(void) AddNameHighlightSequence(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Adds a highlight sequence to the number highlighter.
     * @param start The start position of the highlight sequence.
     * @param end The end position of the highlight sequence.
     */
    CARAPI_(void) AddNumberHighlightSequence(
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    /**
     * Returns the text view for the contact name, creating it if necessary.
     */
    CARAPI_(AutoPtr<ITextView>) GetNameTextView();

    /**
     * Adds or updates a text view for the phonetic name.
     */
    CARAPI_(void) SetPhoneticName(
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 size);

    /**
     * Returns the text view for the phonetic name, creating it if necessary.
     */
    CARAPI_(AutoPtr<ITextView>) GetPhoneticNameTextView();

    /**
     * Adds or updates a text view for the data label.
     */
    CARAPI_(void) SetLabel(
        /* [in] */ ICharSequence* text);

    /**
     * Returns the text view for the data label, creating it if necessary.
     */
    CARAPI_(AutoPtr<ITextView>) GetLabelView();

    /**
     * Adds or updates a text view for the data element.
     */
    CARAPI_(void) SetData(
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 size);

    /**
     * Sets phone number for a list item. This takes care of number highlighting if the highlight
     * mask exists.
     */
    CARAPI_(void) SetPhoneNumber(
        /* [in] */ const String& text,
        /* [in] */ const String& countryIso);

    /**
     * Returns the text view for the data text, creating it if necessary.
     */
    CARAPI_(AutoPtr<ITextView>) GetDataView();

    /**
     * Adds or updates a text view for the search snippet.
     */
    CARAPI_(void) SetSnippet(
        /* [in] */ const String& text);

    /**
     * Returns the text view for the search snippet, creating it if necessary.
     */
    CARAPI_(AutoPtr<ITextView>) GetSnippetView();

    /**
     * Returns the text view for the status, creating it if necessary.
     */
    CARAPI_(AutoPtr<ITextView>) GetStatusView();

    /**
     * Adds or updates a text view for the status.
     */
    CARAPI_(void) SetStatus(
        /* [in] */ ICharSequence* text);

    /**
     * Adds or updates the presence icon view.
     */
    CARAPI_(void) SetPresence(
        /* [in] */ IDrawable* icon);

    CARAPI_(void) ShowDisplayName(
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 nameColumnIndex,
        /* [in] */ Int32 displayOrder);

    CARAPI_(void) SetDisplayName(
        /* [in] */ ICharSequence* name,
        /* [in] */ Boolean highlight);

    CARAPI_(void) SetDisplayName(
        /* [in] */ ICharSequence* name);

    CARAPI_(void) HideDisplayName();

    CARAPI_(void) ShowPhoneticName(
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 phoneticNameColumnIndex);

    CARAPI_(void) HidePhoneticName();

    /**
     * Sets the proper icon (star or presence or nothing) and/or status message.
     */
    CARAPI_(void) ShowPresenceAndStatusMessage(
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 presenceColumnIndex,
        /* [in] */ Int32 contactStatusColumnIndex);

    /**
     * Shows search snippet.
     */
    CARAPI_(void) ShowSnippet(
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 summarySnippetColumnIndex);

    /**
     * Shows data element.
     */
    CARAPI_(void) ShowData(
        /* [in] */ ICursor* cursor,
        /* [in] */ Int32 dataColumnIndex);

    CARAPI_(void) SetActivatedStateSupported(
        /* [in] */ Boolean flag);

    CARAPI_(void) SetAdjustSelectionBoundsEnabled(
        /* [in] */ Boolean enabled);

    // @Override
    CARAPI RequestLayout();

    CARAPI_(void) SetPhotoPosition(
        /* [in] */ PhotoPosition photoPosition);

    CARAPI_(PhotoPosition) GetPhotoPosition();

    /**
     * Set drawable resources directly for both the background and the drawable resource
     * of the photo view
     *
     * @param backgroundId Id of background resource
     * @param drawableId Id of drawable resource
     */
    CARAPI_(void) SetDrawableResource(
        /* [in] */ Int32 backgroundId,
        /* [in] */ Int32 drawableId);

    // @Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* res);

protected:
    // @Override
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    CARAPI_(Boolean) IsVisible(
        /* [in] */ IView* view);

    CARAPI_(Int32) GetDefaultPhotoViewSize();

    // @Override
    CARAPI DrawableStateChanged();

    // @Override
    CARAPI_(Boolean) VerifyDrawable(
        /* [in] */ IDrawable* who);

    CARAPI_(TextUtilsTruncateAt) GetTextEllipsis();

    CARAPI_(Boolean) PointIsInView(
        /* [in] */ Float localX,
        /* [in] */ Float localY);

private:
    /**
     * Extracts width and height from the style
     */
    CARAPI_(void) EnsurePhotoViewSize();

    /**
     * Gets a LayoutParam that corresponds to the default photo size.
     *
     * @return A new LayoutParam.
     */
    CARAPI_(AutoPtr<IViewGroupLayoutParams>) GetDefaultPhotoLayoutParams();

    CARAPI_(void) SetMarqueeText(
        /* [in] */ ITextView* textView,
        /* [in] */ ArrayOf<Char32>* text,
        /* [in] */ Int32 size);

    CARAPI_(void) SetMarqueeText(
        /* [in] */ ITextView* textView,
        /* [in] */ ICharSequence* text);

    /**
     * Used for deferred snippets from the database. The contents come back as large strings which
     * need to be extracted for display.
     *
     * @param snippet The snippet from the database.
     * @param query The search query substring.
     * @param displayName The contact display name.
     * @return The proper snippet to display.
     */
    CARAPI_(String) UpdateSnippet(
        /* [in] */ const String& snippet,
        /* [in] */ const String& query,
        /* [in] */ const String& displayName);

    CARAPI_(String) Snippetize(
        /* [in] */ const String& line,
        /* [in] */ Int32 matchIndex,
        /* [in] */ Int32 maxLength);

    /**
     * Helper method for splitting a string into tokens.  The lists passed in are populated with
     * the
     * tokens and offsets into the content of each token.  The tokenization function parses e-mail
     * addresses as a single token; otherwise it splits on any non-alphanumeric character.
     *
     * @param content Content to split.
     * @return List of token strings.
     */
    static CARAPI_(AutoPtr<ArrayOf<String> >) Split(
        /* [in] */ const String& content);

private:
    static const AutoPtr<IPattern> SPLIT_PATTERN;

    // Style values for layout and appearance
    // The initialized values are defaults if none is provided through xml.
    Int32 mPreferredHeight;
    Int32 mGapBetweenImageAndText;
    Int32 mGapBetweenLabelAndData;
    Int32 mPresenceIconMargin;
    Int32 mPresenceIconSize;
    Int32 mTextIndent;
    Int32 mTextOffsetTop;
    Int32 mNameTextViewTextSize;
    Int32 mHeaderWidth;
    AutoPtr<IDrawable> mActivatedBackgroundDrawable;

    // Set in onLayout. Represent left and right position of the View on the screen.
    Int32 mLeftOffset;
    Int32 mRightOffset;

    /**
     * Used with {@link #mLabelView}, specifying the width ratio between label and data.
     */
    Int32 mLabelViewWidthWeight;
    /**
     * Used with {@link #mDataView}, specifying the width ratio between label and data.
     */
    Int32 mDataViewWidthWeight;

    Elastos::Utility::Etl::List<AutoPtr<HighlightSequence> > mNameHighlightSequence;
    Elastos::Utility::Etl::List<AutoPtr<HighlightSequence> > mNumberHighlightSequence;

    // Highlighting prefix for names.
    String mHighlightedPrefix;

    PhotoPosition mPhotoPosition;

    // Header layout data
    AutoPtr<ITextView> mHeaderTextView;
    Boolean mIsSectionHeaderEnabled;

    // The views inside the contact view
    Boolean mQuickContactEnabled;
    AutoPtr<IQuickContactBadge> mQuickContact;
    AutoPtr<IImageView> mPhotoView;
    AutoPtr<ITextView> mNameTextView;
    AutoPtr<ITextView> mPhoneticNameTextView;
    AutoPtr<ITextView> mLabelView;
    AutoPtr<ITextView> mDataView;
    AutoPtr<ITextView> mSnippetView;
    AutoPtr<ITextView> mStatusView;
    AutoPtr<IImageView> mPresenceIcon;

    AutoPtr<IColorStateList> mSecondaryTextColor;

    Int32 mDefaultPhotoViewSize;
    /**
     * Can be effective even when {@link #mPhotoView} is null, as we want to have horizontal padding
     * to align other data in this View.
     */
    Int32 mPhotoViewWidth;
    /**
     * Can be effective even when {@link #mPhotoView} is null, as we want to have vertical padding.
     */
    Int32 mPhotoViewHeight;

    /**
     * Only effective when {@link #mPhotoView} is null.
     * When true all the Views on the right side of the photo should have horizontal padding on
     * those left assuming there is a photo.
     */
    Boolean mKeepHorizontalPaddingForPhotoView;
    /**
     * Only effective when {@link #mPhotoView} is null.
     */
    Boolean mKeepVerticalPaddingForPhotoView;

    /**
     * True when {@link #mPhotoViewWidth} and {@link #mPhotoViewHeight} are ready for being used.
     * False indicates those values should be updated before being used in position calculation.
     */
    Boolean mPhotoViewWidthAndHeightAreReady;

    Int32 mNameTextViewHeight;
    Int32 mNameTextViewTextColor;
    Int32 mPhoneticNameTextViewHeight;
    Int32 mLabelViewHeight;
    Int32 mDataViewHeight;
    Int32 mSnippetTextViewHeight;
    Int32 mStatusTextViewHeight;

    // Holds Math.max(mLabelTextViewHeight, mDataViewHeight), assuming Label and Data share the
    // same row.
    Int32 mLabelAndDataViewMaxHeight;

    // TODO: some TextView fields are using CharArrayBuffer while some are not. Determine which is
    // more efficient for each case or in general, and simplify the whole implementation.
    // Note: if we're sure MARQUEE will be used every time, there's no reason to use
    // CharArrayBuffer, since MARQUEE requires Span and thus we need to copy characters inside the
    // buffer to Spannable once, while CharArrayBuffer is for directly applying char array to
    // TextView without any modification.
    AutoPtr<ICharArrayBuffer> mDataBuffer;
    AutoPtr<ICharArrayBuffer> mPhoneticNameBuffer;

    Boolean mActivatedStateSupported;
    Boolean mAdjustSelectionBoundsEnabled;

    AutoPtr<IRect> mBoundsWithoutHeader;

    /** A helper used to highlight a prefix in a text field. */
    AutoPtr<TextHighlighter> mTextHighlighter;
    AutoPtr<ICharSequence> mUnknownNameText;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_CONTACTLISTITEMVIEW_H__
