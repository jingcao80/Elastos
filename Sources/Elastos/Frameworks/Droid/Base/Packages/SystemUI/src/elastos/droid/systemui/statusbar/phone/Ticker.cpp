
#include "elastos/droid/systemui/statusbar/phone/Ticker.h"
#include "elastos/droid/systemui/statusbar/StatusBarIconView.h"
#include "../R.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Text.h"
#include <elastos/core/Character.h>
#include <elastos/droid/R.h>

using Elastos::Droid::Internal::StatusBar::CStatusBarIcon;
using Elastos::Droid::Os::CHandler;
using Elastos::Droid::Text::ALIGN_NORMAL;
using Elastos::Droid::Text::ILayout;
using Elastos::Droid::Text::TextUtilsTruncateAt_START;
using Elastos::Droid::Text::CStaticLayout;
using Elastos::Droid::View::Animation::IAnimationUtils;
using Elastos::Droid::View::Animation::CAnimationUtils;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::IViewAnimator;
using Elastos::Droid::Widget::IViewSwitcher;
using Elastos::Core::Character;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Phone {

//=============================================================================
//                Ticker::Segment
//=============================================================================
Ticker::Segment::Segment(
    /* [in] */ IStatusBarNotification* n,
    /* [in] */ IDrawable* icon,
    /* [in] */ ICharSequence* text,
    /* [in] */ Ticker* host)
    : mHost(host)
{
    assert(text != NULL);

    mNotification = n;
    mIcon = icon;
    mText = text;
    Int32 index = 0;
    Int32 len;
    text->GetLength(&len);
    for (; index < len; ++index) {
        Char32 ch;
        text->GetCharAt(index, &ch);
        if (IsGraphicOrEmoji(ch))
            break;
    }

    mCurrent = index;
    mNext = index;
    mFirst = TRUE;
}

AutoPtr<IStaticLayout> Ticker::Segment::GetLayout(
    /* [in] */ ICharSequence* substr)
{
    Int32 tw, pl, pr;
    IView::Probe(mHost->mTextSwitcher)->GetWidth(&tw);
    IView::Probe(mHost->mTextSwitcher)->GetPaddingLeft(&pl);
    IView::Probe(mHost->mTextSwitcher)->GetPaddingRight(&pr);
    Int32 w = tw - pl - pr;
    AutoPtr<IStaticLayout> layout;
    ASSERT_SUCCEEDED(CStaticLayout::New(substr, mHost->mPaint, w, ALIGN_NORMAL,
        1, TextUtilsTruncateAt_START, TRUE, (IStaticLayout**)&layout));
    return layout;
}

AutoPtr<ICharSequence> Ticker::Segment::Rtrim(
    /* [in] */ ICharSequence* substr,
    /* [in] */ Int32 start,
    /* [in] */ Int32 end)
{
    Int32 index = end;
    Char32 ch;
    for (; index > start; --index) {
        substr->GetCharAt(index - 1, &ch);
        if (IsGraphicOrEmoji(ch)) {
            break;
        }
    }

    AutoPtr<ICharSequence> seq;
    if (index > start) {
        substr->SubSequence(start, index, (ICharSequence**)&seq);
    }
    return seq;
}

/** returns null if there is no more text */
AutoPtr<ICharSequence> Ticker::Segment::GetText()
{
    Int32 length = 0;
    if (mText) mText->GetLength(&length);
    if (mCurrent > length) {
        return NULL;
    }

    AutoPtr<ICharSequence> substr;
    mText->SubSequence(mCurrent, length, (ICharSequence**)&substr);
    AutoPtr<IStaticLayout> l = GetLayout(substr);
    Int32 lineCount;
    ILayout::Probe(l)->GetLineCount(&lineCount);
    if (lineCount > 0) {
        Int32 start, end;
        ILayout::Probe(l)->GetLineStart(0, &start);
        ILayout::Probe(l)->GetLineEnd(0, &end);
        mNext = mCurrent + end;
        return Rtrim(substr, start, end);
    }
    else {
        // throw new RuntimeException("lineCount=" + lineCount + " current=" + current +
        //         " text=" + text);
    }
    return NULL;
}

/** returns null if there is no more text */
AutoPtr<ICharSequence> Ticker::Segment::Advance()
{
    mFirst = FALSE;
    Int32 index = mNext;
    Int32 len;
    mText->GetLength(&len);
    Char32 ch;
    for (; index < len; ++index) {
        mText->GetCharAt(index, &ch);
        if (IsGraphicOrEmoji(ch))
            break;
    }

    if (index >= len) {
        return NULL;
    }

    AutoPtr<ICharSequence> substr;
    mText->SubSequence(index, len, (ICharSequence**)&substr);
    AutoPtr<IStaticLayout> l = GetLayout(substr);
    Int32 lineCount;
    ILayout::Probe(l)->GetLineCount(&lineCount);

    Int32 i, start, end, nextStart;
    for (i = 0; i < lineCount; i++) {
        ILayout::Probe(l)->GetLineStart(i, &start);
        ILayout::Probe(l)->GetLineEnd(i, &end);
        if (i == lineCount - 1) {
            mNext = len;
        }
        else {
            ILayout::Probe(l)->GetLineStart(i + 1, &nextStart);
            mNext = index + nextStart;
        }

        AutoPtr<ICharSequence> result = Rtrim(substr, start, end);
        if (result != NULL) {
            mCurrent = index + start;
            return result;
        }
    }
    mCurrent = len;
    return NULL;
}

//=============================================================================
//                Ticker::AdvanceTickerRunnable
//=============================================================================
Ticker::AdvanceTickerRunnable::AdvanceTickerRunnable(
    /* [in] */ Ticker* ticker)
    : mHost(ticker)
{
}

ECode Ticker::AdvanceTickerRunnable::Run()
{
    while (mHost->mSegments.IsEmpty() == FALSE) {
        AutoPtr<Segment> seg = mHost->mSegments[0];

        if (seg->mFirst) {
            // this makes the icon slide in for the first one for a given
            // notification even if there are two notifications with the
            // same icon in a row
            mHost->mIconSwitcher->SetImageDrawable(seg->mIcon);
        }

        AutoPtr<ICharSequence> text = seg->Advance();
        if (text == NULL) {
            mHost->mSegments.Erase(mHost->mSegments.Begin());
            continue;
        }
        mHost->mTextSwitcher->SetText(text);

        mHost->ScheduleAdvance();
        break;
    }

    if (mHost->mSegments.IsEmpty()) {
        mHost->TickerDone();
    }
    return NOERROR;
}

//=============================================================================
//                Ticker
//=============================================================================
const Int32 Ticker::TICKER_SEGMENT_DELAY = 3000;

Boolean Ticker::IsGraphicOrEmoji(
    /* [in] */ Char32 c)
{
    Int32 gc = Character::GetType(c);
    return     gc != Character::CONTROL
            && gc != Character::FORMAT
            && gc != Character::UNASSIGNED
            && gc != Character::LINE_SEPARATOR
            && gc != Character::PARAGRAPH_SEPARATOR
            && gc != Character::SPACE_SEPARATOR;
}

Ticker::Ticker()
    : mIconScale(0)
{
    if (!mAdvanceTicker) {
        mAdvanceTicker = new AdvanceTickerRunnable(this);
    }
    if (!mHandler) {
        CHandler::New((IHandler**)&mHandler);
    }
}

ECode Ticker::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IView* sb)
{
    mContext = context;
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    Int32 outerBounds, imageBounds;
    res->GetDimensionPixelSize(R::dimen::status_bar_icon_size, &outerBounds);
    res->GetDimensionPixelSize(R::dimen::status_bar_icon_drawing_size, &imageBounds);
    mIconScale = (Float)imageBounds / (Float)outerBounds;

    sb->FindViewById(R::id::ticker, (IView**)&mTickerView);

    AutoPtr<IAnimationUtils> animUtils;
    CAnimationUtils::AcquireSingleton((IAnimationUtils**)&animUtils);

    AutoPtr<IView> tmpView;
    sb->FindViewById(R::id::tickerIcon, (IView**)&tmpView);
    mIconSwitcher = IImageSwitcher::Probe(tmpView.Get());

    AutoPtr<IAnimation> pushUpIn, pushUpOut;
    animUtils->LoadAnimation(context, Elastos::Droid::R::anim::push_up_in, (IAnimation**)&pushUpIn);
    animUtils->LoadAnimation(context, Elastos::Droid::R::anim::push_up_out, (IAnimation**)&pushUpOut);
    IViewAnimator::Probe(mIconSwitcher)->SetInAnimation(pushUpIn);
    IViewAnimator::Probe(mIconSwitcher)->SetOutAnimation(pushUpOut);
    IView::Probe(mIconSwitcher)->SetScaleX(mIconScale);
    IView::Probe(mIconSwitcher)->SetScaleY(mIconScale);

    tmpView = NULL;
    sb->FindViewById(R::id::tickerText, (IView**)&tmpView);
    mTextSwitcher = ITextSwitcher::Probe(tmpView.Get());

    AutoPtr<IAnimation> textPushUpIn, textPushUpOut;
    animUtils->LoadAnimation(context, Elastos::Droid::R::anim::push_up_in, (IAnimation**)&textPushUpIn);
    animUtils->LoadAnimation(context, Elastos::Droid::R::anim::push_up_out, (IAnimation**)&textPushUpOut);
    IViewAnimator::Probe(mTextSwitcher)->SetInAnimation(textPushUpIn);
    IViewAnimator::Probe(mTextSwitcher)->SetOutAnimation(textPushUpOut);

    // Copy the paint style of one of the TextSwitchers children to use later for measuring
    tmpView = NULL;
    IViewGroup::Probe(mTextSwitcher)->GetChildAt(0, (IView**)&tmpView);
    AutoPtr<ITextView> textView = ITextView::Probe(tmpView.Get());
    AutoPtr<IPaint> paint;
    textView->GetPaint((ITextPaint**)&mPaint);
    return NOERROR;
}

ECode Ticker::AddEntry(
    /* [in] */ IStatusBarNotification* n)
{
    assert(n != NULL);
    Int32 id;
    n->GetId(&id);
    String pkg;
    n->GetPackageName(&pkg);

    Int32 initialCount = mSegments.GetSize();

    AutoPtr<INotification> nn;
    n->GetNotification((INotification**)&nn);
    Int32 icon, iconLevel;
    AutoPtr<ICharSequence> tickerText;
    AutoPtr<IUserHandle> user;
    nn->GetIcon(&icon);
    nn->GetIconLevel(&iconLevel);
    nn->GetTickerText((ICharSequence**)&tickerText);
    n->GetUser((IUserHandle**)&user);

    // If what's being displayed has the same text and icon, just drop it
    // (which will let the current one finish, this happens when apps do
    // a notification storm).
    if (initialCount > 0) {
        AutoPtr<Segment> seg = mSegments[0];
        Int32 oid;
        String opkg;
        seg->mNotification->GetId(&oid);
        seg->mNotification->GetPackageName(&opkg);
        if (pkg.Equals(opkg)) {
            AutoPtr<INotification> segn;
            seg->mNotification->GetNotification((INotification**)&segn);
                Int32 segIcon, segIconLevel;
                AutoPtr<ICharSequence> segTickerText;
                nn->GetIcon(&segIcon);
                nn->GetIconLevel(&segIconLevel);
                nn->GetTickerText((ICharSequence**)&segTickerText);

            if (icon == segIcon && iconLevel == segIconLevel
                && CharSequencesEqual(segTickerText, tickerText)) {
                return NOERROR;
            }
        }
    }

    AutoPtr<IStatusBarIcon> sbIcon;
    CStatusBarIcon::New(pkg, user, icon, iconLevel, 0, tickerText, (IStatusBarIcon**)&sbIcon);
    AutoPtr<IDrawable> drawableIcon = StatusBarIconView::GetIcon(mContext, sbIcon);
    AutoPtr<Segment> newSegment = new Segment(n, drawableIcon, tickerText, this);

    // If there's already a notification schedule for this package and id, remove it.
    Int32 curId;
    String curPkg;
    List<AutoPtr<Segment> >::ReverseIterator rit;
    for (rit = mSegments.RBegin(); rit != mSegments.REnd();) {
        AutoPtr<Segment> seg = *rit;
        seg->mNotification->GetId(&curId);
        seg->mNotification->GetPackageName(&curPkg);
        if (id == curId && pkg.Equals(curPkg)) {
            rit = List<AutoPtr<Segment> >::ReverseIterator(mSegments.Erase(--(rit.GetBase())));
        }
        else {
            ++rit;
        }
    }

    mSegments.PushBack(newSegment);

    if (initialCount == 0 && mSegments.IsEmpty() == FALSE) {
        AutoPtr<Segment> seg = mSegments[0];
        seg->mFirst = FALSE;

        IViewAnimator::Probe(mIconSwitcher)->SetAnimateFirstView(FALSE);
        IViewSwitcher::Probe(mIconSwitcher)->Reset();
        mIconSwitcher->SetImageDrawable(seg->mIcon);

        IViewAnimator::Probe(mTextSwitcher)->SetAnimateFirstView(FALSE);
        IViewSwitcher::Probe(mTextSwitcher)->Reset();
        AutoPtr<ICharSequence> seq = seg->GetText();
        mTextSwitcher->SetText(seq);

        TickerStarting();
        ScheduleAdvance();
    }
    return NOERROR;
}

Boolean Ticker::CharSequencesEqual(
    /* [in] */ ICharSequence* a,
    /* [in] */ ICharSequence* b)
{
    Int32 al = 0, bl = 0;
    a->GetLength(&al);
    b->GetLength(&bl);
    if (al != bl) {
        return FALSE;
    }

    Int32 length = al;
    for (Int32 i = 0; i < length; i++) {
        Char32 ac, bc;
        a->GetCharAt(i, &ac);
        b->GetCharAt(i, &bc);
        if (ac != bc) {
            return FALSE;
        }
    }
    return TRUE;
}

ECode Ticker::RemoveEntry(
    /* [in] */ IStatusBarNotification* n)
{
    assert(n != NULL);
    Int32 id, curId;
    n->GetId(&id);
    String pkg, curPkg;
    n->GetPackageName(&pkg);
    List<AutoPtr<Segment> >::ReverseIterator rit;
    for (rit = mSegments.RBegin(); rit != mSegments.REnd();) {
        AutoPtr<Segment> seg = *rit;
        seg->mNotification->GetId(&curId);
        seg->mNotification->GetPackageName(&curPkg);
        if (id == curId && pkg.Equals(curPkg)) {
            rit = List<AutoPtr<Segment> >::ReverseIterator(mSegments.Erase(--(rit.GetBase())));
        }
        else {
            ++rit;
        }
    }
    return NOERROR;
}

ECode Ticker::Halt()
{
    mHandler->RemoveCallbacks(mAdvanceTicker);
    mSegments.Clear();
    TickerHalting();
    return NOERROR;
}

ECode Ticker::ReflowText()
{
    if (mSegments.IsEmpty() == FALSE) {
        AutoPtr<Segment> seg = mSegments[0];
        AutoPtr<ICharSequence> text = seg->GetText();
        mTextSwitcher->SetCurrentText(text);
    }
    return NOERROR;
}

void Ticker::ScheduleAdvance()
{
    Boolean result;
    mHandler->PostDelayed(mAdvanceTicker, TICKER_SEGMENT_DELAY, &result);
}

}// namespace Phone
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
