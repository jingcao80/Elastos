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

#ifndef __ELASTOS_DROID_SUPPORT_V4_VIEW_PAGERTITLESTRIP_H__
#define __ELASTOS_DROID_SUPPORT_V4_VIEW_PAGERTITLESTRIP_H__

#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/database/DataSetObserver.h"
#include "elastos/droid/view/ViewGroup.h"
#include "_Elastos.Droid.Support.V4.View.h"

using Elastos::Droid::Database::DataSetObserver;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

/**
 * PagerTitleStrip is a non-interactive indicator of the current, next,
 * and previous pages of a {@link ViewPager}. It is intended to be used as a
 * child view of a ViewPager widget in your XML layout.
 * Add it as a child of a ViewPager in your layout file and set its
 * android:layout_gravity to TOP or BOTTOM to pin it to the top or bottom
 * of the ViewPager. The title from each page is supplied by the method
 * {@link PagerAdapter#getPageTitle(int)} in the adapter supplied to
 * the ViewPager.
 *
 * <p>For an interactive indicator, see {@link PagerTabStrip}.</p>
 */
class PagerTitleStrip
    : public ViewGroup
    , public IPagerTitleStrip
    , public IViewPagerDecor
{
protected:
    class PagerTitleStripImplBase
        : public Object
        , public IPagerTitleStripImpl
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("PagerTitleStrip::PagerTitleStripImplBase")

        PagerTitleStripImplBase();

        ~PagerTitleStripImplBase();

        CARAPI SetSingleLineAllCaps(
            /* [in] */ ITextView* text);
    };

    class PagerTitleStripImplIcs
        : public Object
        , public IPagerTitleStripImpl
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("PagerTitleStrip::PagerTitleStripImplIcs")

        PagerTitleStripImplIcs();

        ~PagerTitleStripImplIcs();

        CARAPI SetSingleLineAllCaps(
            /* [in] */ ITextView* text);
    };

private:
    class PageListener
        : public DataSetObserver
        , public IViewPagerOnPageChangeListener
        , public IViewPagerOnAdapterChangeListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("PagerTitleStrip::PageListener")

        PageListener(
            /* [in] */ PagerTitleStrip* host);

        ~PageListener();

        // @Override
        CARAPI OnPageScrolled(
            /* [in] */ Int32 position,
            /* [in] */ Float positionOffset,
            /* [in] */ Int32 positionOffsetPixels);

        // @Override
        CARAPI OnPageSelected(
            /* [in] */ Int32 position);

        // @Override
        CARAPI OnPageScrollStateChanged(
            /* [in] */ Int32 state);

        // @Override
        CARAPI OnAdapterChanged(
            /* [in] */ IPagerAdapter* oldAdapter,
            /* [in] */ IPagerAdapter* newAdapter);

        // @Override
        CARAPI OnChanged();

    private:
        Int32 mScrollState;
        PagerTitleStrip* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("PagerTitleStrip")

    PagerTitleStrip();

    virtual ~PagerTitleStrip();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Set the required spacing between title segments.
     *
     * @param spacingPixels Spacing between each title displayed in pixels
     */
    virtual CARAPI SetTextSpacing(
        /* [in] */ Int32 spacingPixels);

    /**
     * @return The required spacing between title segments in pixels
     */
    virtual CARAPI GetTextSpacing(
        /* [out] */ Int32* scaledTextSpacing);

    /**
     * Set the alpha value used for non-primary page titles.
     *
     * @param alpha Opacity value in the range 0-1f
     */
    virtual CARAPI SetNonPrimaryAlpha(
        /* [in] */ Float alpha);

    /**
     * Set the color value used as the base color for all displayed page titles.
     * Alpha will be ignored for non-primary page titles. See {@link #setNonPrimaryAlpha(float)}.
     *
     * @param color Color hex code in 0xAARRGGBB format
     */
    virtual CARAPI SetTextColor(
        /* [in] */ Int32 color);

    /**
     * Set the default text size to a given unit and value.
     * See {@link TypedValue} for the possible dimension units.
     *
     * <p>Example: to set the text size to 14px, use
     * setTextSize(TypedValue.COMPLEX_UNIT_PX, 14);</p>
     *
     * @param unit The desired dimension unit
     * @param size The desired size in the given units
     */
    virtual CARAPI SetTextSize(
        /* [in] */ Int32 unit,
        /* [in] */ Float size);

    /**
     * Set the {@link Gravity} used to position text within the title strip.
     * Only the vertical gravity component is used.
     *
     * @param gravity {@link Gravity} constant for positioning title text
     */
    virtual CARAPI SetGravity(
        /* [in] */ Int32 gravity);

    // @Override
    virtual CARAPI RequestLayout();

protected:
    // @Override
    virtual CARAPI OnAttachedToWindow();

    // @Override
    virtual CARAPI OnDetachedFromWindow();

    virtual CARAPI_(void) UpdateText(
        /* [in] */ Int32 currentItem,
        /* [in] */ IPagerAdapter* adapter);

    virtual CARAPI_(void) UpdateAdapter(
        /* [in] */ IPagerAdapter* oldAdapter,
        /* [in] */ IPagerAdapter* newAdapter);

    virtual CARAPI_(void) UpdateTextPositions(
        /* [in] */ Int32 position,
        /* [in] */ Float positionOffset,
        /* [in] */ Boolean force);

    // @Override
    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

    virtual CARAPI_(Int32) GetMinHeight();

private:
    static CARAPI_(void) SetSingleLineAllCaps(
        /* [in] */ ITextView* text);

    static Boolean InitStatic();

protected:
    AutoPtr<IViewPager> mPager;
    AutoPtr<ITextView> mPrevText;
    AutoPtr<ITextView> mCurrText;
    AutoPtr<ITextView> mNextText;

    Int32 mTextColor;

private:
    static const String TAG;
    static const AutoPtr<ArrayOf<Int32> > ATTRS;

    static const AutoPtr<ArrayOf<Int32> > TEXT_ATTRS;

    static const Float SIDE_ALPHA;
    static const Int32 TEXT_SPACING; // dip

    static AutoPtr<IPagerTitleStripImpl> IMPL;

    static Boolean sInit;
    // static {
    //     if (android.os.Build.VERSION.SDK_INT >= 14) {
    //         IMPL = new PagerTitleStripImplIcs();
    //     }
    //     else {
    //         IMPL = new PagerTitleStripImplBase();
    //     }
    // }

    Int32 mLastKnownCurrentPage;
    Float mLastKnownPositionOffset;
    Int32 mScaledTextSpacing;
    Int32 mGravity;

    Boolean mUpdatingText;
    Boolean mUpdatingPositions;

    AutoPtr<PageListener> mPageListener;

    // WeakReference<PagerAdapter> mWatchingAdapter;
    AutoPtr<IWeakReference> mWatchingAdapter;

    Int32 mNonPrimaryAlpha;
};

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SUPPORT_V4_VIEW_PAGERTITLESTRIP_H__