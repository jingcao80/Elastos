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

#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CCANDIDATEVIEW_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CCANDIDATEVIEW_H__

#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_InputMethod_Pinyin_CCandidateView.h"
#include "elastos/droid/inputmethod/pinyin/CBalloonHint.h"
#include "elastos/droid/inputmethod/pinyin/CPinyinIME.h"
#include <elastos/droid/os/HandlerRunnable.h>
#include <elastos/droid/view/View.h>
#include <elastos/utility/etl/Vector.h>

using Elastos::Droid::Graphics::IPaintFontMetricsInt;
using Elastos::Droid::Os::HandlerRunnable;
using Elastos::Droid::View::IGestureDetector;
using Elastos::Droid::View::View;
using Elastos::Utility::Etl::Vector;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * View to show candidate list. There two candidate view instances which are
 * used to show animation when user navigates between pages.
 */
CarClass(CCandidateView)
    , public Elastos::Droid::View::View
    , public ICandidateView
{
private:
    class PressTimer
        : public HandlerRunnable
    {
    public:
        PressTimer(
            /* [in] */ CCandidateView* host);

        CARAPI_(void) StartTimer(
            /* [in] */ Int64 afterMillis,
            /* [in] */ Int32 pageNo,
            /* [in] */ Int32 activeInPage);

        CARAPI_(Int32) GetPageToShow();

        CARAPI_(Int32) GetActiveCandOfPageToShow();

        CARAPI_(Boolean) RemoveTimer();

        CARAPI_(Boolean) IsPending();

        CARAPI Run();

    private:
        Boolean mTimerPending;
        Int32 mPageNoToShow;
        Int32 mActiveCandOfPage;
        CCandidateView* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CCandidateView();

    CARAPI constructor(
        /* [in] */ IContext* ctx,
        /* [in] */ IAttributeSet* attrs);

    CARAPI Initialize(
        /* [in] */ IArrowUpdater* arrowUpdater,
        /* [in] */ CBalloonHint* balloonHint,
        /* [in] */ IGestureDetector* gestureDetector,
        /* [in] */ ICandidateViewListener* cvListener);

    CARAPI SetDecodingInfo(
        /* [in] */ CPinyinIME::DecodingInfo* decInfo);

    CARAPI_(Int32) GetActiveCandiatePosInPage();

    CARAPI_(Int32) GetActiveCandiatePosGlobal();

    /**
     * Show a page in the decoding result set previously.
     *
     * @param pageNo Which page to show.
     * @param activeCandInPage Which candidate should be set as active item.
     * @param enableActiveHighlight When FALSE, active item will not be
     *        highlighted.
     */
    CARAPI ShowPage(
        /* [in] */ Int32 pageNo,
        /* [in] */ Int32 activeCandInPage,
        /* [in] */ Boolean enableActiveHighlight);

    CARAPI EnableActiveHighlight(
        /* [in] */ Boolean enableActiveHighlight);

    CARAPI_(Boolean) ActiveCursorForward();

    CARAPI_(Boolean) ActiveCurseBackward();

    // Because the candidate view under the current focused one may also get
    // touching events. Here we just bypass the event to the container and let
    // it decide which view should handle the event.
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnTouchEventReal(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

protected:
    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    using View::OnSizeChanged;

    CARAPI OnSizeChanged();

    CARAPI_(Boolean) CalculatePage(
        /* [in] */ Int32 pageNo);

    CARAPI_(String) GetLimitedCandidateForDrawing(
        /* [in] */ const String& rawCandidate,
        /* [in] */ Float widthToDraw);

    CARAPI_(Float) DrawVerticalSeparator(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Float xPos);

    CARAPI_(Int32) MapToItemInPage(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI_(void) ShowBalloon(
        /* [in] */ Int32 candPos,
        /* [in] */ Boolean delayedShow);

private:
    /**
     * The minimum width to show a item.
     */
    static const Float MIN_ITEM_WIDTH;

    /**
     * Suspension points used to display Int64 items.
     */
    static String SUSPENSION_POINTS;

    /**
     * The width to draw candidates.
     */
    Int32 mContentWidth;

    /**
     * The height to draw candidate content.
     */
    Int32 mContentHeight;

    /**
     * Whether footnotes are displayed. Footnote is shown when hardware keyboard
     * is available.
     */
    Boolean mShowFootnote;

    /**
     * Balloon hint for candidate press/release.
     */
    AutoPtr<CBalloonHint> mBalloonHint;

    /**
     * Desired position of the balloon to the input view.
     */
    AutoPtr< ArrayOf<Int32> > mHintPositionToInputView;

    /**
     * Decoding result to show.
     */
    AutoPtr<CPinyinIME::DecodingInfo> mDecInfo;

    /**
     * Listener used to notify IME that user clicks a candidate, or navigate
     * between them.
     */
    AutoPtr<ICandidateViewListener> mCvListener;

    /**
     * Used to notify the container to update the status of forward/backward
     * arrows.
     */
    AutoPtr<IArrowUpdater> mArrowUpdater;

    /**
     * If TRUE, update the arrow status when drawing candidates.
     */
    Boolean mUpdateArrowStatusWhenDraw;

    /**
     * Page number of the page displayed in this view.
     */
    Int32 mPageNo;

    /**
     * Active candidate position in this page.
     */
    Int32 mActiveCandInPage;

    /**
     * Used to decided whether the active candidate should be highlighted or
     * not. If user changes focus to composing view (The view to show Pinyin
     * string), the highlight in candidate view should be removed.
     */
    Boolean mEnableActiveHighlight;

    /**
     * The page which is just calculated.
     */
    Int32 mPageNoCalculated;

    /**
     * The Drawable used to display as the background of the high-lighted item.
     */
    AutoPtr<IDrawable> mActiveCellDrawable;

    /**
     * The Drawable used to display as separators between candidates.
     */
    AutoPtr<IDrawable> mSeparatorDrawable;

    /**
     * Color to draw normal candidates generated by IME.
     */
    Int32 mImeCandidateColor;

    /**
     * Color to draw normal candidates Recommended by application.
     */
    Int32 mRecommendedCandidateColor;

    /**
     * Color to draw the normal(not highlighted) candidates, it can be one of
     * {@link #mImeCandidateColor} or {@link #mRecommendedCandidateColor}.
     */
    Int32 mNormalCandidateColor;

    /**
     * Color to draw the active(highlighted) candidates, including candidates
     * from IME and candidates from application.
     */
    Int32 mActiveCandidateColor;

    /**
     * Text size to draw candidates generated by IME.
     */
    Int32 mImeCandidateTextSize;

    /**
     * Text size to draw candidates recommended by application.
     */
    Int32 mRecommendedCandidateTextSize;

    /**
     * The current text size to draw candidates. It can be one of
     * {@link #mImeCandidateTextSize} or {@link #mRecommendedCandidateTextSize}.
     */
    Int32 mCandidateTextSize;

    /**
     * Paint used to draw candidates.
     */
    AutoPtr<IPaint> mCandidatesPaint;

    /**
     * Used to draw footnote.
     */
    AutoPtr<IPaint> mFootnotePaint;

    /**
     * The width to show suspension points.
     */
    Float mSuspensionPointsWidth;

    /**
     * Rectangle used to draw the active candidate.
     */
    AutoPtr<IRectF> mActiveCellRect;

    /**
     * Left and right margins for a candidate. It is specified in xml, and is
     * the minimum margin for a candidate. The actual gap between two candidates
     * is 2 * {@link #mCandidateMargin} + {@link #mSeparatorDrawable}.
     * getIntrinsicWidth(). Because length of candidate is not fixed, there can
     * be some extra space after the last candidate in the current page. In
     * order to achieve best look-and-feel, this extra space will be divided and
     * allocated to each candidates.
     */
    Float mCandidateMargin;

    /**
     * Left and right extra margins for a candidate.
     */
    Float mCandidateMarginExtra;

    /**
     * Rectangles for the candidates in this page.
     **/
    Vector<AutoPtr<IRectF> > mCandRects;

    /**
     * FontMetricsInt used to measure the size of candidates.
     */
    AutoPtr<IPaintFontMetricsInt> mFmiCandidates;

    /**
     * FontMetricsInt used to measure the size of footnotes.
     */
    AutoPtr<IPaintFontMetricsInt> mFmiFootnote;

    AutoPtr<PressTimer> mTimer;

    AutoPtr<IGestureDetector> mGestureDetector;

    AutoPtr< ArrayOf<Int32> > mLocationTmp;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_CCANDIDATEVIEW_H__
