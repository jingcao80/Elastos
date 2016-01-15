
#include "CPinyinCandidateView.h"
#include "CDecodingInfo.h"
#include "CBalloonHint.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/Math.h>
#include "elastos/droid/graphics/CPaint.h"


using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Os::EIID_IHandler;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

CAR_OBJECT_IMPL(CPinyinCandidateView);
CAR_INTERFACE_IMPL(CPinyinCandidateView, View, ICandidateView);

CPinyinCandidateView::PressTimer::PressTimer(
    /* [in] */ CPinyinCandidateView* host)
    : mTimerPending(FALSE)
    , mPageNoToShow(0)
    , mActiveCandOfPage(0)
    , mHost(host)
{
}

void CPinyinCandidateView::PressTimer::StartTimer(
    /* [in] */ Int64 afterMillis,
    /* [in] */ Int32 pageNo,
    /* [in] */ Int32 activeInPage)
{
    mHost->mTimer->RemoveTimer();
    Boolean result = FALSE;
    PostDelayed(this, afterMillis, &result);
    mTimerPending = TRUE;
    mPageNoToShow = pageNo;
    mActiveCandOfPage = activeInPage;
}

Int32 CPinyinCandidateView::PressTimer::GetPageToShow()
{
    return mPageNoToShow;
}

Int32 CPinyinCandidateView::PressTimer::GetActiveCandOfPageToShow()
{
    return mActiveCandOfPage;
}

Boolean CPinyinCandidateView::PressTimer::RemoveTimer()
{
    if (mTimerPending) {
        mTimerPending = FALSE;
        Handler::RemoveCallbacks(this);
        return TRUE;
    }
    return FALSE;
}

Boolean CPinyinCandidateView::PressTimer::IsPending()
{
    return mTimerPending;
}

ECode CPinyinCandidateView::PressTimer::Run()
{
    if (mPageNoToShow >= 0 && mActiveCandOfPage >= 0) {
        // Always enable to highlight the clicked one.
        mHost->ShowPage(mPageNoToShow, mActiveCandOfPage, TRUE);
        mHost->Invalidate();
    }
    mTimerPending = FALSE;
    return NOERROR;
}


const Float CPinyinCandidateView::MIN_ITEM_WIDTH = 22;
String CPinyinCandidateView::SUSPENSION_POINTS = String("...");

CPinyinCandidateView::CPinyinCandidateView()
    : mContentWidth(0)
    , mContentHeight(0)
    , mShowFootnote(TRUE)
    , mUpdateArrowStatusWhenDraw(FALSE)
    , mPageNo(0)
    , mActiveCandInPage(0)
    , mEnableActiveHighlight(TRUE)
    , mPageNoCalculated(-1)
    , mImeCandidateColor(0)
    , mRecommendedCandidateColor(0)
    , mNormalCandidateColor(0)
    , mActiveCandidateColor(0)
    , mImeCandidateTextSize(0)
    , mRecommendedCandidateTextSize(0)
    , mCandidateTextSize(0)
    , mSuspensionPointsWidth(0.f)
    , mCandidateMargin(0.f)
    , mCandidateMarginExtra(0.f)
{
    memset(mHintPositionToInputView, 0, sizeof(mHintPositionToInputView));
    mTimer = new PressTimer(this);
    memset(mLocationTmp, 0, sizeof(mLocationTmp));
}

void CPinyinCandidateView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 mOldWidth = GetMeasuredWidth();
    Int32 mOldHeight = GetMeasuredHeight();

    SetMeasuredDimension(GetDefaultSize(GetSuggestedMinimumWidth(),
            widthMeasureSpec), GetDefaultSize(GetSuggestedMinimumHeight(),
            heightMeasureSpec));

    if (mOldWidth != GetMeasuredWidth() || mOldHeight != GetMeasuredHeight()) {
        OnSizeChanged();
    }
}

void CPinyinCandidateView::OnSizeChanged()
{
    mContentWidth = GetMeasuredWidth() - mPaddingLeft - mPaddingRight;
    mContentHeight = (Int32) ((GetMeasuredHeight() - mPaddingTop - mPaddingBottom) * 0.95f);
    /**
     * How to decide the font size if the height for display is given?
     * Now it is implemented in a stupid way.
     */
    Int32 textSize = 1;
    mCandidatesPaint->SetTextSize(textSize);
    mFmiCandidates = NULL;
    mCandidatesPaint->GetFontMetricsInt((IPaintFontMetricsInt**)&mFmiCandidates);
    Int32 bottom = 0, top = 0;
    while ((mFmiCandidates->GetBottom(&bottom), bottom) - (mFmiCandidates->GetTop(&top), top) < mContentHeight) {
        textSize++;
        mCandidatesPaint->SetTextSize(textSize);
        mFmiCandidates = NULL;
        mCandidatesPaint->GetFontMetricsInt((IPaintFontMetricsInt**)&mFmiCandidates);
    }

    mImeCandidateTextSize = textSize;
    mRecommendedCandidateTextSize = textSize * 3 / 4;
    if (NULL == mDecInfo) {
        mCandidateTextSize = mImeCandidateTextSize;
        mCandidatesPaint->SetTextSize(mCandidateTextSize);
        mFmiCandidates = NULL;
        mCandidatesPaint->GetFontMetricsInt((IPaintFontMetricsInt**)&mFmiCandidates);
        mCandidatesPaint->MeasureText(SUSPENSION_POINTS, &mSuspensionPointsWidth);
    } else {
        // Reset the decoding information to update members for painting.
        SetDecodingInfo(mDecInfo);
    }

    textSize = 1;
    mFootnotePaint->SetTextSize(textSize);
    mFmiFootnote = NULL;
    mFootnotePaint->GetFontMetricsInt((IPaintFontMetricsInt**)&mFmiFootnote);
    while ((mFmiFootnote->GetBottom(&bottom), bottom) - (mFmiFootnote->GetTop(&top), top) < mContentHeight / 2) {
        textSize++;
        mFootnotePaint->SetTextSize(textSize);
        mFmiFootnote = NULL;
        mFootnotePaint->GetFontMetricsInt((IPaintFontMetricsInt**)&mFmiFootnote);
    }
    textSize--;
    mFootnotePaint->SetTextSize(textSize);
    mFmiFootnote = NULL;
    mFootnotePaint->GetFontMetricsInt((IPaintFontMetricsInt**)&mFmiFootnote);

    // When the size is changed, the first page will be displayed.
    // mPageNo = 0;
    // mActiveCandInPage = 0;
}

Boolean CPinyinCandidateView::CalculatePage(
    /* [in] */ Int32 pageNo)
{
    if (pageNo == mPageNoCalculated) return TRUE;

    mContentWidth = GetMeasuredWidth() - mPaddingLeft - mPaddingRight;
    mContentHeight = (Int32) ((GetMeasuredHeight() - mPaddingTop - mPaddingBottom) * 0.95f);

    if (mContentWidth <= 0 || mContentHeight <= 0) return FALSE;

    CDecodingInfo* decInfo = (CDecodingInfo*)mDecInfo.Get();
    assert(decInfo != NULL);
    Int32 candSize = decInfo->mCandidatesList.GetSize();

    // If the size of page exists, only calculate the extra margin.
    Boolean onlyExtraMargin = FALSE;
    Int32 fromPage = decInfo->mPageStart.GetSize() - 1;
    if ((Int32)decInfo->mPageStart.GetSize() > pageNo + 1) {
        onlyExtraMargin = TRUE;
        fromPage = pageNo;
    }

    // If the previous pages have no information, calculate them first.
    for (Int32 p = fromPage; p <= pageNo; p++) {
        Int32 pStart = decInfo->mPageStart[p];
        Int32 pSize = 0;
        Int32 charNum = 0, tmp = 0;
        Float lastItemWidth = 0;

        Float xPos = 0.f;
        xPos += (mSeparatorDrawable->GetIntrinsicWidth(&tmp), tmp);
        Int32 itemPos;
        Float itemWidth;
        String itemStr;
        while (xPos < mContentWidth && pStart + pSize < candSize) {
            itemPos = pStart + pSize;
            itemStr = decInfo->mCandidatesList[itemPos];
            mCandidatesPaint->MeasureText(itemStr, &itemWidth);
            if (itemWidth < MIN_ITEM_WIDTH) itemWidth = MIN_ITEM_WIDTH;

            itemWidth += mCandidateMargin * 2;
            itemWidth += (mSeparatorDrawable->GetIntrinsicWidth(&tmp), tmp);
            if (xPos + itemWidth < mContentWidth || 0 == pSize) {
                xPos += itemWidth;
                lastItemWidth = itemWidth;
                pSize++;
                charNum += itemStr.GetLength();
            } else {
                break;
            }
        }
        if (!onlyExtraMargin) {
            decInfo->mPageStart.PushBack(pStart + pSize);
            decInfo->mCnToPage.PushBack(decInfo->mCnToPage[p] + charNum);
        }

        Float marginExtra = (mContentWidth - xPos) / pSize / 2;

        if (mContentWidth - xPos > lastItemWidth) {
            // Must be the last page, because if there are more items,
            // the next item's width must be less than lastItemWidth.
            // In this case, if the last margin is less than the current
            // one, the last margin can be used, so that the
            // look-and-feeling will be the same as the previous page.
            if (mCandidateMarginExtra <= marginExtra) {
                marginExtra = mCandidateMarginExtra;
            }
        } else if (pSize == 1) {
            marginExtra = 0;
        }
        mCandidateMarginExtra = marginExtra;
    }
    mPageNoCalculated = pageNo;
    return TRUE;
}

void CPinyinCandidateView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    View::OnDraw(canvas);
    // The invisible candidate view(the one which is not in foreground) can
    // also be called to drawn, but its decoding result and candidate list
    // may be empty.
    Boolean empty = FALSE;
    if (NULL == mDecInfo || (mDecInfo->IsCandidatesListEmpty(&empty), empty)) return;

    // Calculate page. If the paging information is ready, the function will
    // return at once.
    CalculatePage(mPageNo);

    CDecodingInfo* decInfo = (CDecodingInfo*)mDecInfo.Get();
    Int32 pStart = decInfo->mPageStart[mPageNo];
    Int32 pSize = decInfo->mPageStart[mPageNo + 1] - pStart;
    Float candMargin = mCandidateMargin + mCandidateMarginExtra;
    if (mActiveCandInPage > pSize - 1) {
        mActiveCandInPage = pSize - 1;
    }

    mCandRects.Clear();

    Float xPos = mPaddingLeft;
    Int32 bottom = 0, top = 0;
    mFmiCandidates->GetBottom(&bottom);
    mFmiCandidates->GetTop(&top);
    Int32 yPos = (GetMeasuredHeight() - (bottom - top)) / 2 - top;
    xPos += DrawVerticalSeparator(canvas, xPos);
    for (Int32 i = 0; i < pSize; i++) {
        Float footnoteSize = 0.f;
        String footnote;
        if (mShowFootnote) {
            footnote = StringUtils::ToString(i + 1);
            mFootnotePaint->MeasureText(footnote, &footnoteSize);
        }

        String cand = decInfo->mCandidatesList[pStart + i];
        Float candidateWidth = 0.f;
        mCandidatesPaint->MeasureText(cand, &candidateWidth);
        Float centerOffset = 0;
        if (candidateWidth < MIN_ITEM_WIDTH) {
            centerOffset = (MIN_ITEM_WIDTH - candidateWidth) / 2;
            candidateWidth = MIN_ITEM_WIDTH;
        }

        Float itemTotalWidth = candidateWidth + 2 * candMargin;

        if (mActiveCandInPage == i && mEnableActiveHighlight) {
            mActiveCellRect->Set(xPos, mPaddingTop + 1, xPos
                    + itemTotalWidth, GetHeight() - mPaddingBottom - 1);

            Float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;
            mActiveCellRect->GetLeft(&left);
            mActiveCellRect->GetTop(&top);
            mActiveCellRect->GetRight(&right);
            mActiveCellRect->GetBottom(&bottom);
            mActiveCellDrawable->SetBounds((Int32) left, (Int32) top, (Int32) right, (Int32) bottom);
            mActiveCellDrawable->Draw(canvas);
        }

        AutoPtr<IRectF> rf;
        CRectF::New((IRectF**)&rf);
        if ((Int32)mCandRects.GetSize() < pSize) mCandRects.PushBack(rf);

        mFmiCandidates->GetBottom(&bottom);
        mFmiCandidates->GetTop(&top);
        mCandRects[i]->Set(xPos - 1, yPos + top, xPos + itemTotalWidth + 1, yPos + bottom);

        // Draw footnote
        if (mShowFootnote) {
            canvas->DrawText(footnote, xPos + (candMargin - footnoteSize)
                    / 2, yPos, mFootnotePaint);
        }

        // Left margin
        xPos += candMargin;
        if (candidateWidth > mContentWidth - xPos - centerOffset) {
            cand = GetLimitedCandidateForDrawing(cand,
                    mContentWidth - xPos - centerOffset);
        }
        if (mActiveCandInPage == i && mEnableActiveHighlight) {
            mCandidatesPaint->SetColor(mActiveCandidateColor);
        } else {
            mCandidatesPaint->SetColor(mNormalCandidateColor);
        }
        canvas->DrawText(cand, xPos + centerOffset, yPos, mCandidatesPaint);

        // Candidate and right margin
        xPos += candidateWidth + candMargin;

        // Draw the separator between candidates.
        xPos += DrawVerticalSeparator(canvas, xPos);
    }

    // Update the arrow status of the container.
    if (NULL != mArrowUpdater && mUpdateArrowStatusWhenDraw) {
        mArrowUpdater->UpdateArrowStatus();
        mUpdateArrowStatusWhenDraw = FALSE;
    }
}

String CPinyinCandidateView::GetLimitedCandidateForDrawing(
    /* [in] */ const String& rawCandidate,
    /* [in] */ Float widthToDraw)
{
    Int32 subLen = rawCandidate.GetLength();
    if (subLen <= 1) return rawCandidate;
    Float width;
    do {
        subLen--;
        mCandidatesPaint->MeasureText(rawCandidate, 0, subLen, &width);
        if (width + mSuspensionPointsWidth <= widthToDraw || 1 >= subLen) {
            return rawCandidate.Substring(0, subLen) + SUSPENSION_POINTS;
        }
    } while (TRUE);
}

Float CPinyinCandidateView::DrawVerticalSeparator(
    /* [in] */ ICanvas* canvas,
    /* [in] */ Float xPos)
{
    Int32 value = 0;
    mSeparatorDrawable->SetBounds((Int32) xPos, mPaddingTop, (Int32) xPos
            + (mSeparatorDrawable->GetIntrinsicWidth(&value), value), GetMeasuredHeight()
            - mPaddingBottom);
    mSeparatorDrawable->Draw(canvas);
    return (mSeparatorDrawable->GetIntrinsicWidth(&value), value);
}

Int32 CPinyinCandidateView::MapToItemInPage(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y)
{
    // mCandRects.size() == 0 happens when the page is set, but
    // touch events occur before onDraw(). It usually happens with
    // monkey test.
    Boolean ready = FALSE;
    if (!(mDecInfo->PageReady(mPageNo, &ready), ready) || mPageNoCalculated != mPageNo
            || mCandRects.IsEmpty()) {
        return -1;
    }

    Int32 pageStart = ((CDecodingInfo*)mDecInfo.Get())->mPageStart[mPageNo];
    Int32 pageSize = ((CDecodingInfo*)mDecInfo.Get())->mPageStart[mPageNo + 1] - pageStart;
    if ((Int32)mCandRects.GetSize() < pageSize) {
        return -1;
    }

    // If not found, try to find the nearest one.
    Float nearestDis = Elastos::Core::Math::FLOAT_MAX_VALUE;
    Int32 nearest = -1;

    List<AutoPtr<IRectF> >::Iterator ator = mCandRects.Begin();
    for (Int32 i = 0; ator != mCandRects.End(), i < pageSize; i++, ++ator) {
        AutoPtr<IRectF> r = *ator;
        Float left = 0.f, right = 0.f, top = 0.f, bottom = 0.f;
        r->GetLeft(&left);
        r->GetRight(&right);
        r->GetTop(&top);
        r->GetBottom(&bottom);
        if (left < x && right > x && top < y && bottom > y) {
            return i;
        }
        Float disx = (left + right) / 2 - x;
        Float disy = (top + bottom) / 2 - y;
        Float dis = disx * disx + disy * disy;
        if (dis < nearestDis) {
            nearestDis = dis;
            nearest = i;
        }
    }

    return nearest;
}

Boolean CPinyinCandidateView::OnTouchEvent(
    /* [in] */ IMotionEvent* event)
{
    return View::OnTouchEvent(event);
}

void CPinyinCandidateView::ShowBalloon(
    /* [in] */ Int32 candPos,
    /* [in] */ Boolean delayedShow)
{
    mBalloonHint->RemoveTimer();

    AutoPtr<IRectF> r = mCandRects[candPos];
    Float left = 0.f, right = 0.f, top = 0.f, bottom = 0.f;
    r->GetLeft(&left);
    r->GetRight(&right);
    r->GetTop(&top);
    r->GetBottom(&bottom);
    Int32 desired_width = (Int32) (right - left);
    Int32 desired_height = (Int32) (bottom - top);
    CDecodingInfo* decInfo = (CDecodingInfo*)mDecInfo.Get();
    mBalloonHint->SetBalloonConfig(decInfo->mCandidatesList[decInfo->mPageStart[mPageNo] + candPos], 44, TRUE,
            mImeCandidateColor, desired_width, desired_height);

    Int32 x = 0, y = 0, value = 0;
    GetLocationOnScreen(&x, &y);
    mLocationTmp[0] = x;
    mLocationTmp[1] = y;

    mHintPositionToInputView[0] = mLocationTmp[0]
            + (Int32) (left - ((mBalloonHint->GetWidth(&value), value) - desired_width) / 2);
    mHintPositionToInputView[1] = -(mBalloonHint->GetHeight(&value), value);

    Int64 delay = CBalloonHint::TIME_DELAY_SHOW;
    if (!delayedShow) delay = 0;
    mBalloonHint->Dismiss();
    Boolean isShowing = FALSE;
    AutoPtr<ArrayOf<Int32> > inputs = ArrayOf<Int32>::Alloc(2);
    (*inputs)[0] = mHintPositionToInputView[0];
    (*inputs)[1] = mHintPositionToInputView[1];
    if (!(mBalloonHint->IsShowing(&isShowing), isShowing)) {
        mBalloonHint->DelayedShow(delay, inputs);
    } else {
        mBalloonHint->DelayedUpdate(0, inputs, -1, -1);
    }
}

ECode CPinyinCandidateView::ShowPage(
    /* [in] */ Int32 pageNo,
    /* [in] */ Int32 activeCandInPage,
    /* [in] */ Boolean enableActiveHighlight)
{
    if (NULL == mDecInfo) return NOERROR;
    mPageNo = pageNo;
    mActiveCandInPage = activeCandInPage;
    if (mEnableActiveHighlight != enableActiveHighlight) {
        mEnableActiveHighlight = enableActiveHighlight;
    }

    if (!CalculatePage(mPageNo)) {
        mUpdateArrowStatusWhenDraw = TRUE;
    } else {
        mUpdateArrowStatusWhenDraw = FALSE;
    }

    return View::Invalidate();
}

ECode CPinyinCandidateView::SetDecodingInfo(
    /* [in] */ IDecodingInfo* decInfo)
{
    if (NULL == decInfo) return NOERROR;
    mDecInfo = decInfo;
    mPageNoCalculated = -1;

    Boolean state = FALSE;
    if (mDecInfo->CandidatesFromApp(&state), state) {
        mNormalCandidateColor = mRecommendedCandidateColor;
        mCandidateTextSize = mRecommendedCandidateTextSize;
    } else {
        mNormalCandidateColor = mImeCandidateColor;
        mCandidateTextSize = mImeCandidateTextSize;
    }

    Float size = 0.f;
    if ((mCandidatesPaint->GetTextSize(&size), size) != mCandidateTextSize) {
        mCandidatesPaint->SetTextSize(mCandidateTextSize);
        mFmiCandidates = NULL;
        mCandidatesPaint->GetFontMetricsInt((IPaintFontMetricsInt**)&mFmiCandidates);
        mCandidatesPaint->MeasureText(SUSPENSION_POINTS, &mSuspensionPointsWidth);
    }

    // Remove any pending timer for the previous list.
    mTimer->RemoveTimer();
    return NOERROR;
}

PInterface CPinyinCandidateView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == Elastos::Droid::View::EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ICandidateView) {
        return (IInterface*)(ICandidateView*)this;
    }

    return View::Probe(riid);
}

ECode CPinyinCandidateView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    Init(context, attrs);

    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);

    AutoPtr<IConfiguration> conf;
    r->GetConfiguration((IConfiguration**)&conf);
    Int32 value = 0;
    if ((conf->GetKeyboard(&value), value) == IConfiguration::KEYBOARD_NOKEYS
            || (conf->GetHardKeyboardHidden(&value), value) == IConfiguration::HARDKEYBOARDHIDDEN_YES) {
        mShowFootnote = FALSE;
    }

    r->GetDrawable(0x7f020005/*R::drawable::candidate_hl_bg*/, (IDrawable**)&mActiveCellDrawable);
    r->GetDrawable(0x7f020007/*R::drawable::candidates_vertical_line*/, (IDrawable**)&mSeparatorDrawable);
    r->GetDimension(0x7f080001/*R::dimen::candidate_margin_left_right*/, &mCandidateMargin);

    r->GetColor(0x7f070003/*R::color::candidate_color*/, &mImeCandidateColor);
    r->GetColor(0x7f070005/*R::color::recommended_candidate_color*/, &mRecommendedCandidateColor);
    mNormalCandidateColor = mImeCandidateColor;
    r->GetColor(0x7f070004/*R::color::active_candidate_color*/, &mActiveCandidateColor);

    CPaint::New((IPaint**)&mCandidatesPaint);
    mCandidatesPaint->SetAntiAlias(TRUE);

    CPaint::New((IPaint**)&mFootnotePaint);
    mFootnotePaint->SetAntiAlias(TRUE);
    Int32 color = 0;
    r->GetColor(0x7f070006/*R::color::footnote_color*/, &color);
    mFootnotePaint->SetColor(color);
    return CRectF::New((IRectF**)&mActiveCellRect);
}

ECode CPinyinCandidateView::Initialize(
    /* [in] */ IArrowUpdater* arrowUpdater,
    /* [in] */ IBalloonHint* balloonHint,
    /* [in] */ IGestureDetector* gestureDetector,
    /* [in] */ ICandidateViewListener* cvListener)
{
    mArrowUpdater = arrowUpdater;
    mBalloonHint = balloonHint;
    mGestureDetector = gestureDetector;
    mCvListener = cvListener;
    return NOERROR;
}

ECode CPinyinCandidateView::GetActiveCandiatePosInPage(
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos);
    *pos = mActiveCandInPage;
    return NOERROR;
}

ECode CPinyinCandidateView::GetActiveCandiatePosGlobal(
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos);
    *pos = ((CDecodingInfo*)mDecInfo.Get())->mPageStart[mPageNo] + mActiveCandInPage;
    return NOERROR;
}

ECode CPinyinCandidateView::EnableActiveHighlight(
    /* [in] */ Boolean enableActiveHighlight)
{
    if (enableActiveHighlight == mEnableActiveHighlight) return NOERROR;

    mEnableActiveHighlight = enableActiveHighlight;
    return View::Invalidate();
}

ECode CPinyinCandidateView::ActiveCursorForward(
    /* [out] */ Boolean* active)
{
    VALIDATE_NOT_NULL(active);
    Boolean result = FALSE;
    if (!(mDecInfo->PageReady(mPageNo, &result), result)) {
        *active = FALSE;
        return NOERROR;
    }
    Int32 pageSize = ((CDecodingInfo*)mDecInfo.Get())->mPageStart[mPageNo + 1]
            - ((CDecodingInfo*)mDecInfo.Get())->mPageStart[mPageNo];
    if (mActiveCandInPage + 1 < pageSize) {
        ShowPage(mPageNo, mActiveCandInPage + 1, TRUE);
        *active = TRUE;
        return NOERROR;
    }
    *active = FALSE;
    return NOERROR;
}

ECode CPinyinCandidateView::ActiveCurseBackward(
    /* [out] */ Boolean* active)
{
    VALIDATE_NOT_NULL(active);
    if (mActiveCandInPage > 0) {
        ShowPage(mPageNo, mActiveCandInPage - 1, TRUE);
        *active = TRUE;
        return NOERROR;
    }
    *active = FALSE;
    return NOERROR;
}

ECode CPinyinCandidateView::OnTouchEventReal(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    // The page in the background can also be touched.
    Boolean state = FALSE;
    if (NULL == mDecInfo || !(mDecInfo->PageReady(mPageNo, &state), state)
            || mPageNoCalculated != mPageNo) {
        *result = TRUE;
        return NOERROR;
    }

    Int32 x, y;
    Float tmp = 0.f;
    x = (Int32) (event->GetX(&tmp), tmp);
    y = (Int32) (event->GetY(&tmp), tmp);

    if (mGestureDetector->OnTouchEvent(event, &state), state) {
        mTimer->RemoveTimer();
        mBalloonHint->DelayedDismiss(0);
        *result = TRUE;
        return NOERROR;
    }

    Int32 clickedItemInPage = -1;

    Int32 action = 0;
    switch ((event->GetAction(&action), action)) {
    case IMotionEvent::ACTION_UP:
        clickedItemInPage = MapToItemInPage(x, y);
        if (clickedItemInPage >= 0) {
            View::Invalidate();
            mCvListener->OnClickChoice(clickedItemInPage
                    + ((CDecodingInfo*)mDecInfo.Get())->mPageStart[mPageNo]);
        }
        mBalloonHint->DelayedDismiss(CBalloonHint::TIME_DELAY_DISMISS);
        break;

    case IMotionEvent::ACTION_DOWN:
        clickedItemInPage = MapToItemInPage(x, y);
        if (clickedItemInPage >= 0) {
            ShowBalloon(clickedItemInPage, TRUE);
            mTimer->StartTimer(CBalloonHint::TIME_DELAY_SHOW, mPageNo,
                    clickedItemInPage);
        }
        break;

    case IMotionEvent::ACTION_CANCEL:
        break;

    case IMotionEvent::ACTION_MOVE:
        clickedItemInPage = MapToItemInPage(x, y);
        if (clickedItemInPage >= 0
                && (clickedItemInPage != mTimer->GetActiveCandOfPageToShow() || mPageNo != mTimer->GetPageToShow())) {
            ShowBalloon(clickedItemInPage, TRUE);
            mTimer->StartTimer(CBalloonHint::TIME_DELAY_SHOW, mPageNo, clickedItemInPage);
        }
    }

    *result = TRUE;
    return NOERROR;
}


} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
