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

#ifndef __ELASTOS_DROID_SUPPORT_V4_VIEW_PAGERTABSTRIP_H__
#define __ELASTOS_DROID_SUPPORT_V4_VIEW_PAGERTABSTRIP_H__

#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/support/v4/view/PagerTitleStrip.h"

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;

namespace Elastos {
namespace Droid {
namespace Support {
namespace V4 {
namespace View {

/**
 * PagerTabStrip is an interactive indicator of the current, next,
 * and previous pages of a {@link ViewPager}. It is intended to be used as a
 * child view of a ViewPager widget in your XML layout.
 * Add it as a child of a ViewPager in your layout file and set its
 * android:layout_gravity to TOP or BOTTOM to pin it to the top or bottom
 * of the ViewPager. The title from each page is supplied by the method
 * {@link PagerAdapter#getPageTitle(int)} in the adapter supplied to
 * the ViewPager.
 *
 * <p>For a non-interactive indicator, see {@link PagerTitleStrip}.</p>
 */
class PagerTabStrip
    : public PagerTitleStrip
    , public IPagerTabStrip
{
private:
    class InitOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InitOnClickListener(
            /* [in] */ PagerTabStrip* host,
            /* [in] */ Int32 id);

        ~InitOnClickListener();

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        PagerTabStrip* mHost;
        Int32 mId;
    };

public:
    CAR_INTERFACE_DECL()

    TO_STRING_IMPL("PagerTabStrip")

    PagerTabStrip();

    virtual ~PagerTabStrip();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Set the color of the tab indicator bar.
     *
     * @param color Color to set as an 0xRRGGBB value. The high byte (alpha) is ignored.
     */
    virtual CARAPI SetTabIndicatorColor(
        /* [in] */ Int32 color);

    /**
     * Set the color of the tab indicator bar from a color resource.
     *
     * @param resId Resource ID of a color resource to load
     */
    // @ColorRes
    virtual CARAPI SetTabIndicatorColorResource(
        /* [in] */ Int32 resId);

    /**
     * @return The current tab indicator color as an 0xRRGGBB value.
     */
    virtual CARAPI GetTabIndicatorColor(
        /* [out] */ Int32* indicatorColor);

    //@Override
    CARAPI SetPadding(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom);

    //@Override
    CARAPI SetTextSpacing(
        /* [in] */ Int32 textSpacing);

    //@Override
    CARAPI SetBackgroundDrawable(
        /* [in] */ IDrawable* d);

    //@Override
    CARAPI SetBackgroundColor(
        /* [in] */ Int32 color);

    //@Override
    //@DrawableRes
    CARAPI SetBackgroundResource(
        /* [in] */ Int32 resId);

    /**
     * Set whether this tab strip should draw a full-width underline in the
     * current tab indicator color.
     *
     * @param drawFull true to draw a full-width underline, false otherwise
     */
    virtual CARAPI SetDrawFullUnderline(
        /* [in] */ Boolean drawFull);

    /**
     * Return whether or not this tab strip will draw a full-width underline.
     * This defaults to true if no background is set.
     *
     * @return true if this tab strip will draw a full-width underline in the
     * current tab indicator color.
     */
    virtual CARAPI GetDrawFullUnderline(
        /* [out] */ Boolean* drawFullUnderline);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

protected:
    //@Override
    virtual CARAPI_(Int32) GetMinHeight();

    //@Override
    virtual CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    //@Override
    virtual CARAPI_(void) UpdateTextPositions(
        /* [in] */ Int32 position,
        /* [in] */ Float positionOffset,
        /* [in] */ Boolean force);

private:
    static const String TAG;

    static const Int32 INDICATOR_HEIGHT;// = 3; // dp
    static const Int32 MIN_PADDING_BOTTOM;// = INDICATOR_HEIGHT + 3; // dp
    static const Int32 TAB_PADDING;// = 16; // dp
    static const Int32 TAB_SPACING;// = 32; // dp
    static const Int32 MIN_TEXT_SPACING;// = TAB_SPACING + TAB_PADDING * 2; // dp
    static const Int32 FULL_UNDERLINE_HEIGHT;// = 1; // dp
    static const Int32 MIN_STRIP_HEIGHT;// = 32; // dp

    Int32 mIndicatorColor;
    Int32 mIndicatorHeight;

    Int32 mMinPaddingBottom;
    Int32 mMinTextSpacing;
    Int32 mMinStripHeight;

    Int32 mTabPadding;

    AutoPtr<IPaint> mTabPaint;
    AutoPtr<IRect> mTempRect;

    Int32 mTabAlpha;

    Boolean mDrawFullUnderline;
    Boolean mDrawFullUnderlineSet;
    Int32 mFullUnderlineHeight;

    Boolean mIgnoreTap;
    Float mInitialMotionX;
    Float mInitialMotionY;
    Int32 mTouchSlop;
};

} // namespace View
} // namespace V4
} // namespace Support
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SUPPORT_V4_VIEW_PAGERTABSTRIP_H__