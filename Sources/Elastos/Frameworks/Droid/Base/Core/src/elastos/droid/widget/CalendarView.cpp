
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Text.h>
#include "elastos/droid/widget/CalendarView.h"
#include "elastos/droid/widget/AbsListView.h"
#include "elastos/droid/content/res/CConfiguration.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/text/format/CDateUtils.h"
#include "elastos/droid/utility/CTypedValueHelper.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::App::IService;
using Elastos::Droid::Content::Res::CConfiguration;
using Elastos::Droid::Database::EIID_IDataSetObserver;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::PaintStyle_FILL;
using Elastos::Droid::Graphics::PaintAlign_CENTER;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::CDateUtils;
using Elastos::Droid::Utility::IDisplayMetrics;
using Elastos::Droid::Utility::ITypedValueHelper;
using Elastos::Droid::Utility::CTypedValueHelper;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::EIID_IViewOnTouchListener;

using Elastos::Core::IInteger32;
using Elastos::Core::CInteger32;
using Elastos::Core::ISystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Core::EIID_IRunnable;
using Elastos::Utility::IDate;
using Elastos::Utility::ILocaleHelper;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::ITimeZone;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Libcore::ICU::ILocaleDataHelper;
using Libcore::ICU::CLocaleDataHelper;
using Libcore::ICU::ILocaleData;

namespace Elastos {
namespace Droid {
namespace Widget {

//========================================================================================
//              CalendarView::AbstractCalendarViewDelegate::
//========================================================================================
CAR_INTERFACE_IMPL(CalendarView::AbstractCalendarViewDelegate, Object, ICalendarViewDelegate)

CalendarView::AbstractCalendarViewDelegate::AbstractCalendarViewDelegate(
    /* [in] */ ICalendarView* delegator,
    /* [in] */ IContext* context)
{
    mDelegator = delegator;
    mContext = context;

    // Initialization based on locale
    AutoPtr<ILocaleHelper> hlp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&hlp);
    AutoPtr<ILocale> loc;
    hlp->GetDefault((ILocale**)&loc);
    SetCurrentLocale(loc);
}

void CalendarView::AbstractCalendarViewDelegate::SetCurrentLocale(
    /* [in] */ ILocale* locale)
{
    Boolean bEqls = FALSE;
    if ((locale->Equals(mCurrentLocale, &bEqls), bEqls)) {
        return;
    }
    mCurrentLocale = locale;
}

//========================================================================================
//              CalendarView::LegacyCalendarViewDelegate::OnScrollListener::
//========================================================================================
CAR_INTERFACE_IMPL(CalendarView::LegacyCalendarViewDelegate::OnScrollListener, Object, IAbsListViewOnScrollListener)

CalendarView::LegacyCalendarViewDelegate::OnScrollListener::OnScrollListener(
    /* [in] */ LegacyCalendarViewDelegate* owner)
{
    mOwner = owner;
}

ECode CalendarView::LegacyCalendarViewDelegate::OnScrollListener::OnScrollStateChanged(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 scrollState)
{
    mOwner->OnScrollStateChanged(view, scrollState);
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::OnScrollListener::OnScroll(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 firstVisibleItem,
    /* [in] */ Int32 visibleItemCount,
    /* [in] */ Int32 totalItemCount)
{
    mOwner->OnScroll(view, firstVisibleItem,
            visibleItemCount, totalItemCount);
    return NOERROR;
}

//========================================================================================
//              CalendarView::LegacyCalendarViewDelegate::
//========================================================================================
const Boolean CalendarView::LegacyCalendarViewDelegate::DEFAULT_SHOW_WEEK_NUMBER = TRUE;

const Int64 CalendarView::LegacyCalendarViewDelegate::MILLIS_IN_DAY = 86400000L;

const Int32 CalendarView::LegacyCalendarViewDelegate::DAYS_PER_WEEK = 7;

const Int64 CalendarView::LegacyCalendarViewDelegate::MILLIS_IN_WEEK = DAYS_PER_WEEK * MILLIS_IN_DAY;

const Int32 CalendarView::LegacyCalendarViewDelegate::SCROLL_HYST_WEEKS = 2;

const Int32 CalendarView::LegacyCalendarViewDelegate::GOTO_SCROLL_DURATION = 1000;

const Int32 CalendarView::LegacyCalendarViewDelegate::ADJUSTMENT_SCROLL_DURATION = 500;

const Int32 CalendarView::LegacyCalendarViewDelegate::SCROLL_CHANGE_DELAY = 40;

const String CalendarView::LegacyCalendarViewDelegate::DATE_FORMAT("MM/dd/yyyy");

const String CalendarView::LegacyCalendarViewDelegate::DEFAULT_MIN_DATE("01/01/1900");

const String CalendarView::LegacyCalendarViewDelegate::DEFAULT_MAX_DATE("01/01/2100");

const Int32 CalendarView::LegacyCalendarViewDelegate::DEFAULT_SHOWN_WEEK_COUNT = 6;

const Int32 CalendarView::LegacyCalendarViewDelegate::DEFAULT_DATE_TEXT_SIZE = 14;

const Int32 CalendarView::LegacyCalendarViewDelegate::UNSCALED_SELECTED_DATE_VERTICAL_BAR_WIDTH = 6;

const Int32 CalendarView::LegacyCalendarViewDelegate::UNSCALED_WEEK_MIN_VISIBLE_HEIGHT = 12;

const Int32 CalendarView::LegacyCalendarViewDelegate::UNSCALED_LIST_SCROLL_TOP_OFFSET = 2;

const Int32 CalendarView::LegacyCalendarViewDelegate::UNSCALED_BOTTOM_BUFFER = 20;

const Int32 CalendarView::LegacyCalendarViewDelegate::UNSCALED_WEEK_SEPARATOR_LINE_WIDTH = 1;

const Int32 CalendarView::LegacyCalendarViewDelegate::DEFAULT_WEEK_DAY_TEXT_APPEARANCE_RES_ID = -1;

CalendarView::LegacyCalendarViewDelegate::LegacyCalendarViewDelegate(
    /* [in] */ ICalendarView* delegator,
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
    : AbstractCalendarViewDelegate(delegator, context)
    , mWeekSeperatorLineWidth(0)
    , mDateTextSize(0)
    , mSelectedDateVerticalBarWidth(0)
    , mSelectedWeekBackgroundColor(0)
    , mFocusedMonthDateColor(0)
    , mUnfocusedMonthDateColor(0)
    , mWeekSeparatorLineColor(0)
    , mWeekNumberColor(0)
    , mWeekDayTextAppearanceResId(0)
    , mDateTextAppearanceResId(0)
    , mListScrollTopOffset(2)
    , mWeekMinVisibleHeight(12)
    , mBottomBuffer(20)
    , mShownWeekCount(0)
    , mShowWeekNumber(FALSE)
    , mDaysPerWeek(7)
    , mFriction(.05f)
    , mVelocityScale(0.333f)
    , mFirstDayOfWeek(0)
    , mCurrentMonthDisplayed(-1)
    , mPreviousScrollPosition(0)
    , mIsScrollingUp(FALSE)
    , mPreviousScrollState(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE)
    , mCurrentScrollState(IAbsListViewOnScrollListener::SCROLL_STATE_IDLE)
{
    mScrollStateChangedRunnable = new ScrollStateRunnable(this);

    CSimpleDateFormat::New(DATE_FORMAT, (IDateFormat**)&mDateFormat);

    // initialization based on locale
    AutoPtr<ILocaleHelper> hlp;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&hlp);
    AutoPtr<ILocale> loc;
    hlp->GetDefault((ILocale**)&loc);
    SetCurrentLocale(loc);

    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32*>(R::styleable::CalendarView),
        ArraySize(R::styleable::CalendarView));
    AutoPtr<ITypedArray> attributesArray;
    context->ObtainStyledAttributes(attrs,
            attrIds, defStyleAttr, defStyleRes, (ITypedArray**)&attributesArray);
    attributesArray->GetBoolean(R::styleable::CalendarView_showWeekNumber,
            DEFAULT_SHOW_WEEK_NUMBER, &mShowWeekNumber);
    AutoPtr<ILocaleDataHelper> locDtHlp;
    CLocaleDataHelper::AcquireSingleton((ILocaleDataHelper**)&locDtHlp);
    AutoPtr<ILocaleData> locDt;
    locDtHlp->Get(loc, (ILocaleData**)&locDt);
    AutoPtr<IInteger32> pFDW;
    locDt->GetFirstDayOfWeek((IInteger32**)&pFDW);
    Int32 val = 0;
    pFDW->GetValue(&val);
    attributesArray->GetInt32(R::styleable::CalendarView_firstDayOfWeek,
            val, &mFirstDayOfWeek);
    String minDate;
    attributesArray->GetString(R::styleable::CalendarView_minDate, &minDate);
    if (TextUtils::IsEmpty(minDate) || !ParseDate(minDate, mMinDate)) {
        ParseDate(DEFAULT_MIN_DATE, mMinDate);
    }
    String maxDate;
    attributesArray->GetString(R::styleable::CalendarView_maxDate, &maxDate);
    if (TextUtils::IsEmpty(maxDate) || !ParseDate(maxDate, mMaxDate)) {
        ParseDate(DEFAULT_MAX_DATE, mMaxDate);
    }
    Boolean bIsBf = FALSE;
    if ((mMaxDate->IsBefore(mMinDate, &bIsBf), bIsBf)) {
        // throw new IllegalArgumentException("Max date cannot be before min date.");
        return;
    }
    attributesArray->GetInt32(R::styleable::CalendarView_shownWeekCount,
            DEFAULT_SHOWN_WEEK_COUNT, &mShownWeekCount);
    attributesArray->GetColor(
            R::styleable::CalendarView_selectedWeekBackgroundColor, 0, &mSelectedWeekBackgroundColor);
    attributesArray->GetColor(
            R::styleable::CalendarView_focusedMonthDateColor, 0, &mFocusedMonthDateColor);
    attributesArray->GetColor(
            R::styleable::CalendarView_unfocusedMonthDateColor, 0, &mUnfocusedMonthDateColor);
    attributesArray->GetColor(
            R::styleable::CalendarView_weekSeparatorLineColor, 0, &mWeekSeparatorLineColor);
    attributesArray->GetColor(R::styleable::CalendarView_weekNumberColor, 0, &mWeekNumberColor);
    attributesArray->GetDrawable(
            R::styleable::CalendarView_selectedDateVerticalBar, (IDrawable**)&mSelectedDateVerticalBar);

    attributesArray->GetResourceId(
            R::styleable::CalendarView_dateTextAppearance, R::style::TextAppearance_Small, &mDateTextAppearanceResId);
    UpdateDateTextSize();

    attributesArray->GetResourceId(
            R::styleable::CalendarView_weekDayTextAppearance,
            DEFAULT_WEEK_DAY_TEXT_APPEARANCE_RES_ID, &mWeekDayTextAppearanceResId);
    attributesArray->Recycle();

    AutoPtr<IResources> res;
    IView::Probe(mDelegator)->GetResources((IResources**)&res);
    AutoPtr<IDisplayMetrics> displayMetrics;
    res->GetDisplayMetrics((IDisplayMetrics**)&displayMetrics);
    AutoPtr<ITypedValueHelper> hlpType;
    CTypedValueHelper::AcquireSingleton((ITypedValueHelper**)&hlpType);
    hlpType->ApplyDimension(ITypedValue::COMPLEX_UNIT_DIP,
            UNSCALED_WEEK_MIN_VISIBLE_HEIGHT, displayMetrics, (Float*)&mWeekMinVisibleHeight);
    hlpType->ApplyDimension(ITypedValue::COMPLEX_UNIT_DIP,
            UNSCALED_LIST_SCROLL_TOP_OFFSET, displayMetrics, (Float*)&mListScrollTopOffset);
    hlpType->ApplyDimension(ITypedValue::COMPLEX_UNIT_DIP,
            UNSCALED_BOTTOM_BUFFER, displayMetrics, (Float*)&mBottomBuffer);
    hlpType->ApplyDimension(ITypedValue::COMPLEX_UNIT_DIP,
            UNSCALED_SELECTED_DATE_VERTICAL_BAR_WIDTH, displayMetrics, (Float*)&mSelectedDateVerticalBarWidth);
    hlpType->ApplyDimension(ITypedValue::COMPLEX_UNIT_DIP,
            UNSCALED_WEEK_SEPARATOR_LINE_WIDTH, displayMetrics, (Float*)&mWeekSeperatorLineWidth);

    AutoPtr<IInterface> sv;
    mContext->GetSystemService(IContext::LAYOUT_INFLATER_SERVICE, (IInterface**)&sv);
    AutoPtr<ILayoutInflater> layoutInflater = ILayoutInflater::Probe(sv);
    AutoPtr<IView> content;
    layoutInflater->Inflate(R::layout::calendar_view, NULL, FALSE, (IView**)&content);
    IViewGroup::Probe(mDelegator)->AddView(content);

    AutoPtr<IView> vl;
    IView::Probe(mDelegator)->FindViewById(R::id::list, (IView**)&vl);
    mListView = IListView::Probe(vl);
    AutoPtr<IView> vn;
    content->FindViewById(R::id::day_names, (IView**)&vn);
    mDayNamesHeader = IViewGroup::Probe(vn);
    AutoPtr<IView> vmn;
    content->FindViewById(R::id::month_name, (IView**)&vmn);
    mMonthName = ITextView::Probe(vmn);

    SetUpHeader();
    SetUpListView();
    SetUpAdapter();

    // go to today or whichever is close to today min or max date
    AutoPtr<ISystem> system;
    Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    Int64 nowTime = 0;
    system->GetCurrentTimeMillis(&nowTime);
    mTempDate->SetTimeInMillis(nowTime);
    Boolean bMin = FALSE, bMax = FALSE;
    if ((mTempDate->IsBefore(mMinDate, &bMin), bMin)) {
        GoTo(mMinDate, FALSE, TRUE, TRUE);
    }
    else if ((mMaxDate->IsBefore(mTempDate, &bMax), bMax)) {
        GoTo(mMaxDate, FALSE, TRUE, TRUE);
    }
    else {
        GoTo(mTempDate, FALSE, TRUE, TRUE);
    }

    IView::Probe(mDelegator)->Invalidate();
}

ECode CalendarView::LegacyCalendarViewDelegate::SetShownWeekCount(
    /* [in] */ Int32 count)
{
    if (mShownWeekCount != count) {
        mShownWeekCount = count;
        IView::Probe(mDelegator)->Invalidate();
    }
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetShownWeekCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mShownWeekCount;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::SetSelectedWeekBackgroundColor(
    /* [in] */ Int32 color)
{
    if (mSelectedWeekBackgroundColor != color) {
        mSelectedWeekBackgroundColor = color;
        Int32 childCount = 0;
        IViewGroup::Probe(mListView)->GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child;
            IViewGroup::Probe(mListView)->GetChildAt(i, (IView**)&child);
            AutoPtr<WeekView> weekView = (WeekView*) child.Get();
            if (weekView->mHasSelectedDay) {
                weekView->Invalidate();
            }
        }
    }
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetSelectedWeekBackgroundColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSelectedWeekBackgroundColor;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::SetFocusedMonthDateColor(
    /* [in] */ Int32 color)
{
    if (mFocusedMonthDateColor != color) {
        mFocusedMonthDateColor = color;
        Int32 childCount = 0;
        IViewGroup::Probe(mListView)->GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child;
            IViewGroup::Probe(mListView)->GetChildAt(i, (IView**)&child);
            AutoPtr<WeekView> weekView = (WeekView*) child.Get();
            if (weekView->mHasFocusedDay) {
                weekView->Invalidate();
            }
        }
    }
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetFocusedMonthDateColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFocusedMonthDateColor;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::SetUnfocusedMonthDateColor(
    /* [in] */ Int32 color)
{
    if (mUnfocusedMonthDateColor != color) {
        mUnfocusedMonthDateColor = color;
        Int32 childCount = 0;
        IViewGroup::Probe(mListView)->GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child;
            IViewGroup::Probe(mListView)->GetChildAt(i, (IView**)&child);
            AutoPtr<WeekView> weekView = (WeekView*) child.Get();
            if (weekView->mHasUnfocusedDay) {
                weekView->Invalidate();
            }
        }
    }
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetUnfocusedMonthDateColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFocusedMonthDateColor;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::SetWeekNumberColor(
    /* [in] */ Int32 color)
{
    if (mWeekNumberColor != color) {
        mWeekNumberColor = color;
        if (mShowWeekNumber) {
            InvalidateAllWeekViews();
        }
    }
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetWeekNumberColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWeekNumberColor;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::SetWeekSeparatorLineColor(
    /* [in] */ Int32 color)
{
    if (mWeekSeparatorLineColor != color) {
        mWeekSeparatorLineColor = color;
        InvalidateAllWeekViews();
    }
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetWeekSeparatorLineColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWeekSeparatorLineColor;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::SetSelectedDateVerticalBar(
    /* [in] */ Int32 resourceId)
{
    AutoPtr<IContext> cxt;
    IView::Probe(mDelegator)->GetContext((IContext**)&cxt);
    AutoPtr<IDrawable> drawable;
    cxt->GetDrawable(resourceId, (IDrawable**)&drawable);
    SetSelectedDateVerticalBar(drawable);
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::SetSelectedDateVerticalBar(
    /* [in] */ IDrawable* drawable)
{
    if (mSelectedDateVerticalBar.Get() != drawable) {
        mSelectedDateVerticalBar = drawable;
        Int32 childCount = 0;
        IViewGroup::Probe(mListView)->GetChildCount(&childCount);
        for (Int32 i = 0; i < childCount; i++) {
            AutoPtr<IView> child;
            IViewGroup::Probe(mListView)->GetChildAt(i, (IView**)&child);
            AutoPtr<WeekView> weekView = (WeekView*) child.Get();
            if (weekView->mHasSelectedDay) {
                weekView->Invalidate();
            }
        }
    }
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetSelectedDateVerticalBar(
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSelectedDateVerticalBar;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::SetWeekDayTextAppearance(
    /* [in] */ Int32 resourceId)
{
    if (mWeekDayTextAppearanceResId != resourceId) {
        mWeekDayTextAppearanceResId = resourceId;
        SetUpHeader();
    }
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetWeekDayTextAppearance(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mWeekDayTextAppearanceResId;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::SetDateTextAppearance(
    /* [in] */ Int32 resourceId)
{
    if (mDateTextAppearanceResId != resourceId) {
        mDateTextAppearanceResId = resourceId;
        UpdateDateTextSize();
        InvalidateAllWeekViews();
    }
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetDateTextAppearance(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDateTextAppearanceResId;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::SetEnabled(
    /* [in] */ Boolean enabled)
{
    IView::Probe(mListView)->SetEnabled(enabled);
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return IView::Probe(mListView)->IsEnabled(result);
}

ECode CalendarView::LegacyCalendarViewDelegate::SetMinDate(
    /* [in] */ Int64 minDate)
{
    mTempDate->SetTimeInMillis(minDate);
    if (IsSameDate(mTempDate, mMinDate)) {
        return NOERROR;
    }
    mMinDate->SetTimeInMillis(minDate);
    // make sure the current date is not earlier than
    // the new min date since the latter is used for
    // calculating the indices in the adapter thus
    // avoiding out of bounds error
    AutoPtr<ICalendar> date = mAdapter->mSelectedDate;
    Boolean bMin = FALSE;
    if ((date->IsBefore(mMinDate, &bMin), bMin)) {
        mAdapter->SetSelectedDay(mMinDate);
    }
    // reinitialize the adapter since its range depends on min date
    mAdapter->Init();
    if ((date->IsBefore(mMinDate, &bMin), bMin)) {
        Int64 millis = 0;
        mTempDate->GetTimeInMillis(&millis);
        SetDate(millis);
    }
    else {
        // we go to the current date to force the ListView to query its
        // adapter for the shown views since we have changed the adapter
        // range and the base from which the later calculates item indices
        // note that calling setDate will not work since the date is the same
        GoTo(date, FALSE, TRUE, FALSE);
    }
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetMinDate(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mMinDate->GetTimeInMillis(result);
}

ECode CalendarView::LegacyCalendarViewDelegate::SetMaxDate(
    /* [in] */ Int64 maxDate)
{
    mTempDate->SetTimeInMillis(maxDate);
    if (IsSameDate(mTempDate, mMaxDate)) {
        return NOERROR;
    }
    mMaxDate->SetTimeInMillis(maxDate);
    // reinitialize the adapter since its range depends on max date
    mAdapter->Init();
    AutoPtr<ICalendar> date = mAdapter->mSelectedDate;
    Boolean bAft = FALSE;
    if ((date->IsAfter(mMaxDate, &bAft), bAft)) {
        Int64 millis = 0;
        mMaxDate->GetTimeInMillis(&millis);
        SetDate(millis);
    }
    else {
        // we go to the current date to force the ListView to query its
        // adapter for the shown views since we have changed the adapter
        // range and the base from which the later calculates item indices
        // note that calling setDate will not work since the date is the same
        GoTo(date, FALSE, TRUE, FALSE);
    }
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetMaxDate(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mMaxDate->GetTimeInMillis(result);
}

ECode CalendarView::LegacyCalendarViewDelegate::SetShowWeekNumber(
    /* [in] */ Boolean showWeekNumber)
{
    if (mShowWeekNumber == showWeekNumber) {
        return NOERROR;
    }
    mShowWeekNumber = showWeekNumber;
    mAdapter->NotifyDataSetChanged();
    SetUpHeader();
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetShowWeekNumber(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mShowWeekNumber;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::SetFirstDayOfWeek(
    /* [in] */ Int32 firstDayOfWeek)
{
    if (mFirstDayOfWeek == firstDayOfWeek) {
        return NOERROR;
    }
    mFirstDayOfWeek = firstDayOfWeek;
    mAdapter->Init();
    mAdapter->NotifyDataSetChanged();
    SetUpHeader();
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetFirstDayOfWeek(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mFirstDayOfWeek;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::SetDate(
    /* [in] */ Int64 date)
{
    return SetDate(date, FALSE, FALSE);
}

ECode CalendarView::LegacyCalendarViewDelegate::SetDate(
    /* [in] */ Int64 date,
    /* [in] */ Boolean animate,
    /* [in] */ Boolean center)
{
    mTempDate->SetTimeInMillis(date);
    if (IsSameDate(mTempDate, mAdapter->mSelectedDate)) {
        return NOERROR;
    }
    GoTo(mTempDate, animate, TRUE, center);
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::GetDate(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mAdapter->mSelectedDate->GetTimeInMillis(result);
}

ECode CalendarView::LegacyCalendarViewDelegate::SetOnDateChangeListener(
    /* [in] */ IOnDateChangeListener* listener)
{
    mOnDateChangeListener = listener;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    AutoPtr<CConfiguration> ccf = (CConfiguration*)newConfig;
    return SetCurrentLocale(ccf->mLocale);
}

ECode CalendarView::LegacyCalendarViewDelegate::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    assert(0 && "TODO");
//    event->SetClassName(CalendarView.class.getName());
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    assert(0 && "TODO");
//    info->SetClassName(CalendarView.class.getName());
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::SetCurrentLocale(
    /* [in] */ ILocale* locale)
{
    assert(0 && "TODO");
//    FrameLayout::SetCurrentLocale(locale);

    mTempDate = GetCalendarForLocale(mTempDate, locale);
    mFirstDayOfMonth = GetCalendarForLocale(mFirstDayOfMonth, locale);
    mMinDate = GetCalendarForLocale(mMinDate, locale);
    mMaxDate = GetCalendarForLocale(mMaxDate, locale);
    return NOERROR;
}

void CalendarView::LegacyCalendarViewDelegate::UpdateDateTextSize()
{
    AutoPtr<IContext> cxt;
    IView::Probe(mDelegator)->GetContext((IContext**)&cxt);
    AutoPtr<ArrayOf<Int32> > attrIds = ArrayOf<Int32>::Alloc(
        const_cast<Int32*>(R::styleable::TextAppearance),
        ArraySize(R::styleable::TextAppearance));
    AutoPtr<ITypedArray> dateTextAppearance;
    cxt->ObtainStyledAttributes(
            mDateTextAppearanceResId, attrIds, (ITypedArray**)&dateTextAppearance);
    dateTextAppearance->GetDimensionPixelSize(
            R::styleable::TextAppearance_textSize, DEFAULT_DATE_TEXT_SIZE, &mDateTextSize);
    dateTextAppearance->Recycle();
}

void CalendarView::LegacyCalendarViewDelegate::InvalidateAllWeekViews()
{
    Int32 childCount = 0;
    IViewGroup::Probe(mListView)->GetChildCount(&childCount);
    for (Int32 i = 0; i < childCount; i++) {
        AutoPtr<IView> child;
        IViewGroup::Probe(mListView)->GetChildAt(i, (IView**)&child);
        AutoPtr<IView> view = IView::Probe(child);
        view->Invalidate();
    }
}

AutoPtr<ICalendar> CalendarView::LegacyCalendarViewDelegate::GetCalendarForLocale(
    /* [in] */ ICalendar* oldCalendar,
    /* [in] */ ILocale* locale)
{
    if (oldCalendar == NULL) {
        AutoPtr<ICalendarHelper> hlp;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&hlp);
        AutoPtr<ICalendar> res;
        hlp->GetInstance(locale, (ICalendar**)&res);
        return res;
    }
    else {
        Int64 currentTimeMillis = 0;
        oldCalendar->GetTimeInMillis(&currentTimeMillis);
        AutoPtr<ICalendarHelper> hlp;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&hlp);
        AutoPtr<ICalendar> newCalendar;
        hlp->GetInstance(locale, (ICalendar**)&newCalendar);
        newCalendar->SetTimeInMillis(currentTimeMillis);
        return newCalendar;
    }
}

Boolean CalendarView::LegacyCalendarViewDelegate::IsSameDate(
    /* [in] */ ICalendar* firstDate,
    /* [in] */ ICalendar* secondDate)
{
    Int32 fDay = 0, sDay = 0, fYear = 0, sYear = 0;
    firstDate->Get(ICalendar::DAY_OF_YEAR, &fDay);
    secondDate->Get(ICalendar::DAY_OF_YEAR, &sDay);
    firstDate->Get(ICalendar::YEAR, &fYear);
    secondDate->Get(ICalendar::YEAR, &sYear);
    return (fDay == sDay && fYear == sYear);
}

void CalendarView::LegacyCalendarViewDelegate::SetUpAdapter()
{
    if (mAdapter == NULL) {
        mAdapter = new WeeksAdapter(mContext, this);
        AutoPtr<_DataSetObserver> p = new _DataSetObserver(this);
        mAdapter->RegisterDataSetObserver(IDataSetObserver::Probe(p));
        IAdapterView::Probe(mListView)->SetAdapter(mAdapter);
    }

    // refresh the view with the new parameters
    mAdapter->NotifyDataSetChanged();
}

void CalendarView::LegacyCalendarViewDelegate::SetUpHeader()
{
    mDayLabels = ArrayOf<String>::Alloc(mDaysPerWeek);
    for (Int32 i = mFirstDayOfWeek, count = mFirstDayOfWeek + mDaysPerWeek; i < count; i++) {
        Int32 calendarDay = (i > ICalendar::SATURDAY) ? i - ICalendar::SATURDAY : i;
        AutoPtr<IDateUtils> du;
        CDateUtils::AcquireSingleton((IDateUtils**)&du);
        du->GetDayOfWeekString(calendarDay,
                IDateUtils::LENGTH_SHORTEST, &((*mDayLabels)[i - mFirstDayOfWeek]));
    }

    AutoPtr<IView> child;
    mDayNamesHeader->GetChildAt(0, (IView**)&child);
    AutoPtr<ITextView> label = ITextView::Probe(child);
    if (mShowWeekNumber) {
        IView::Probe(label)->SetVisibility(IView::VISIBLE);
    }
    else {
        IView::Probe(label)->SetVisibility(IView::GONE);
    }
    Int32 count = 0;
    mDayNamesHeader->GetChildCount(&count);
    for (Int32 i = 1; i < count; i++) {
        AutoPtr<IView> child;
        mDayNamesHeader->GetChildAt(0, (IView**)&child);
        label = ITextView::Probe(child);
        if (mWeekDayTextAppearanceResId > -1) {
            label->SetTextAppearance(mContext, mWeekDayTextAppearanceResId);
        }
        if (i < mDaysPerWeek + 1) {
            AutoPtr<ICharSequence> cs;
            CString::New((*mDayLabels)[i - 1], (ICharSequence**)&cs);
            label->SetText(cs);
            IView::Probe(label)->SetVisibility(IView::VISIBLE);
        }
        else {
            IView::Probe(label)->SetVisibility(IView::GONE);
        }
    }
    IView::Probe(mDayNamesHeader)->Invalidate();
}

void CalendarView::LegacyCalendarViewDelegate::SetUpListView()
{
    // Configure the listview
    mListView->SetDivider(NULL);
    mListView->SetItemsCanFocus(TRUE);
    IView::Probe(mListView)->SetVerticalScrollBarEnabled(FALSE);
    AutoPtr<OnScrollListener> p = new OnScrollListener(this);
    IAbsListView::Probe(mListView)->SetOnScrollListener(p);
    // Make the scrolling behavior nicer
    IAbsListView::Probe(mListView)->SetFriction(mFriction);
    IAbsListView::Probe(mListView)->SetVelocityScale(mVelocityScale);
}

void CalendarView::LegacyCalendarViewDelegate::GoTo(
    /* [in] */ ICalendar* date,
    /* [in] */ Boolean animate,
    /* [in] */ Boolean setSelected,
    /* [in] */ Boolean forceScroll)
{
    Boolean bIsBf = FALSE, bIsAf = FALSE;
    if ((date->IsBefore(mMinDate, &bIsBf), bIsBf) || (date->IsAfter(mMaxDate, &bIsAf), bIsAf)) {
        // throw new IllegalArgumentException("Time not between " + mMinDate.getTime()
        //         + " and " + mMaxDate.getTime());
        return;
    }
    // Find the first and last entirely visible weeks
    Int32 firstFullyVisiblePosition = 0;
    IAdapterView::Probe(mListView)->GetFirstVisiblePosition(&firstFullyVisiblePosition);
    AutoPtr<IView> firstChild;
    IViewGroup::Probe(mListView)->GetChildAt(0, (IView**)&firstChild);
    Int32 t = 0;
    if (firstChild != NULL && (firstChild->GetTop(&t), t) < 0) {
        firstFullyVisiblePosition++;
    }
    Int32 lastFullyVisiblePosition = firstFullyVisiblePosition + mShownWeekCount - 1;
    if (firstChild != NULL && (firstChild->GetTop(&t), t) > mBottomBuffer) {
        lastFullyVisiblePosition--;
    }
    if (setSelected) {
        mAdapter->SetSelectedDay(date);
    }
    // Get the week we're going to
    Int32 position = GetWeeksSinceMinDate(date);

    // Check if the selected day is now outside of our visible range
    // and if so scroll to the month that contains it
    if (position < firstFullyVisiblePosition || position > lastFullyVisiblePosition
            || forceScroll) {
        Int64 currentTimeMillis = 0;
        date->GetTimeInMillis(&currentTimeMillis);
        mFirstDayOfMonth->SetTimeInMillis(currentTimeMillis);
        mFirstDayOfMonth->Set(ICalendar::DAY_OF_MONTH, 1);

        SetMonthDisplayed(mFirstDayOfMonth);

        // the earliest time we can scroll to is the min date
        Boolean bIsBf = FALSE;
        if ((mFirstDayOfMonth->IsBefore(mMinDate, &bIsBf), bIsBf)) {
            position = 0;
        }
        else {
            position = GetWeeksSinceMinDate(mFirstDayOfMonth);
        }

        mPreviousScrollState = IAbsListViewOnScrollListener::SCROLL_STATE_FLING;
        if (animate) {
            IAbsListView::Probe(mListView)->SmoothScrollToPositionFromTop(position, mListScrollTopOffset,
                    GOTO_SCROLL_DURATION);
        }
        else {
            IAbsListView::Probe(mListView)->SetSelectionFromTop(position, mListScrollTopOffset);
            // Perform any after scroll operations that are needed
            OnScrollStateChanged(IAbsListView::Probe(mListView), IAbsListViewOnScrollListener::SCROLL_STATE_IDLE);
        }
    }
    else if (setSelected) {
        // Otherwise just set the selection
        SetMonthDisplayed(date);
    }
}

Boolean CalendarView::LegacyCalendarViewDelegate::ParseDate(
    /* [in] */ const String& date,
    /* [in] */ ICalendar* outDate)
{
    AutoPtr<IDate> d;
    mDateFormat->Parse(date, (IDate**)&d);
    outDate->SetTime(d);
    return TRUE;
}

void CalendarView::LegacyCalendarViewDelegate::OnScrollStateChanged(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 scrollState)
{
    mScrollStateChangedRunnable->DoScrollStateChange(view, scrollState);
}

void CalendarView::LegacyCalendarViewDelegate::OnScroll(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 firstVisibleItem,
    /* [in] */ Int32 visibleItemCount,
    /* [in] */ Int32 totalItemCount)
{
    AutoPtr<IView> _child;
    IViewGroup::Probe(view)->GetChildAt(0, (IView**)&_child);
    AutoPtr<WeekView> child = (WeekView*) _child.Get();
    if (child == NULL) {
        return;
    }

    // Figure out where we are
    Int32 h = 0, b = 0, pos = 0;
    child->GetHeight(&h);
    child->GetBottom(&b);
    IAdapterView::Probe(view)->GetFirstVisiblePosition(&pos);
    Int64 currScroll = pos * h - b;

    // If we have moved since our last call update the direction
    if (currScroll < mPreviousScrollPosition) {
        mIsScrollingUp = TRUE;
    }
    else if (currScroll > mPreviousScrollPosition) {
        mIsScrollingUp = FALSE;
    }
    else {
        return;
    }

    // Use some hysteresis for checking which month to highlight. This
    // causes the month to transition when two full weeks of a month are
    // visible when scrolling up, and when the first day in a month reaches
    // the top of the screen when scrolling down.
    Int32 offset = b < mWeekMinVisibleHeight ? 1 : 0;
    if (mIsScrollingUp) {
        AutoPtr<IView> c;
        IViewGroup::Probe(view)->GetChildAt(SCROLL_HYST_WEEKS + offset, (IView**)&c);
        child = (WeekView*)c.Get();
    }
    else if (offset != 0) {
        AutoPtr<IView> c;
        IViewGroup::Probe(view)->GetChildAt(offset, (IView**)&c);
        child = (WeekView*)c.Get();
    }

    if (child != NULL) {
        // Find out which month we're moving into
        Int32 month = 0;
        if (mIsScrollingUp) {
            month = child->GetMonthOfFirstWeekDay();
        }
        else {
            month = child->GetMonthOfLastWeekDay();
        }

        // And how it relates to our current highlighted month
        Int32 monthDiff = 0;
        if (mCurrentMonthDisplayed == 11 && month == 0) {
            monthDiff = 1;
        }
        else if (mCurrentMonthDisplayed == 0 && month == 11) {
            monthDiff = -1;
        }
        else {
            monthDiff = month - mCurrentMonthDisplayed;
        }

        // Only switch months if we're scrolling away from the currently
        // selected month
        if ((!mIsScrollingUp && monthDiff > 0) || (mIsScrollingUp && monthDiff < 0)) {
            AutoPtr<ICalendar> firstDay = child->GetFirstDay();
            if (mIsScrollingUp) {
                firstDay->Add(ICalendar::DAY_OF_MONTH, -DAYS_PER_WEEK);
            }
            else {
                firstDay->Add(ICalendar::DAY_OF_MONTH, DAYS_PER_WEEK);
            }
            SetMonthDisplayed(firstDay);
        }
    }
    mPreviousScrollPosition = currScroll;
    mPreviousScrollState = mCurrentScrollState;
}

void CalendarView::LegacyCalendarViewDelegate::SetMonthDisplayed(
    /* [in] */ ICalendar* calendar)
{
    calendar->Get(ICalendar::MONTH, &mCurrentMonthDisplayed);
    mAdapter->SetFocusMonth(mCurrentMonthDisplayed);
    Int32 flags = IDateUtils::FORMAT_SHOW_DATE | IDateUtils::FORMAT_NO_MONTH_DAY
            | IDateUtils::FORMAT_SHOW_YEAR;
    Int64 millis = 0;
    calendar->GetTimeInMillis(&millis);
    AutoPtr<IDateUtils> du;
    CDateUtils::AcquireSingleton((IDateUtils**)&du);
    String newMonthName;
    du->FormatDateRange(mContext, millis, millis, flags, &newMonthName);
    AutoPtr<ICharSequence> cs;
    CString::New(newMonthName, (ICharSequence**)&cs);
    mMonthName->SetText(cs);
    IView::Probe(mMonthName)->Invalidate();
}

Int32 CalendarView::LegacyCalendarViewDelegate::GetWeeksSinceMinDate(
    /* [in] */ ICalendar* date)
{
    Boolean bMin = FALSE;
    if ((date->IsBefore(mMinDate, &bMin), bMin)) {
        // throw new IllegalArgumentException("fromDate: " + mMinDate.getTime()
        //         + " does not precede toDate: " + date.getTime());
        return -1;
    }
    Int64 mils = 0, minMils = 0;
    date->GetTimeInMillis(&mils);
    mMinDate->GetTimeInMillis(&minMils);
    AutoPtr<ITimeZone> tz, minTz;
    date->GetTimeZone((ITimeZone**)&tz);
    mMinDate->GetTimeZone((ITimeZone**)&minTz);
    Int32 off = 0, minOff = 0;
    tz->GetOffset(mils, &off);
    minTz->GetOffset(minMils, &minOff);
    Int64 endTimeMillis = mils + off;
    Int64 startTimeMillis = minMils + minOff;
    Int32 dw = 0;
    mMinDate->Get(ICalendar::DAY_OF_WEEK, &dw);
    Int64 dayOffsetMillis = (dw - mFirstDayOfWeek) * MILLIS_IN_DAY;
    return (Int32) ((endTimeMillis - startTimeMillis + dayOffsetMillis) / MILLIS_IN_WEEK);
}

//========================================================================================
//              CalendarView::LegacyCalendarViewDelegate::ScrollStateRunnable::
//========================================================================================
CAR_INTERFACE_IMPL(CalendarView::LegacyCalendarViewDelegate::ScrollStateRunnable, Object, IRunnable)

CalendarView::LegacyCalendarViewDelegate::ScrollStateRunnable::ScrollStateRunnable(
    /* [in] */ LegacyCalendarViewDelegate* host)
{
    mHost = host;
}

void CalendarView::LegacyCalendarViewDelegate::ScrollStateRunnable::DoScrollStateChange(
    /* [in] */ IAbsListView* view,
    /* [in] */ Int32 scrollState)
{
    mView = view;
    mNewState = scrollState;
    assert(0 && "TODO");
    // mDelegator->RemoveCallbacks(this);
    // mDelegator->PostDelayed(this, SCROLL_CHANGE_DELAY);
}

ECode CalendarView::LegacyCalendarViewDelegate::ScrollStateRunnable::Run()
{
    mHost->mCurrentScrollState = mNewState;
    // Fix the position after a scroll or a fling ends
    if (mNewState == IAbsListViewOnScrollListener::SCROLL_STATE_IDLE
            && mHost->mPreviousScrollState != IAbsListViewOnScrollListener::SCROLL_STATE_IDLE) {
        AutoPtr<IView> child;
        IViewGroup::Probe(mView)->GetChildAt(0, (IView**)&child);
        if (child == NULL) {
            // The view is no longer visible, just return
            return NOERROR;
        }
        Int32 b = 0;
        child->GetBottom(&b);
        Int32 dist = b - mHost->mListScrollTopOffset;
        if (dist > mHost->mListScrollTopOffset) {
            if (mHost->mIsScrollingUp) {
                Int32 h = 0;
                child->GetHeight(&h);
                mView->SmoothScrollBy(dist - h,
                        ADJUSTMENT_SCROLL_DURATION);
            }
            else {
                mView->SmoothScrollBy(dist, ADJUSTMENT_SCROLL_DURATION);
            }
        }
    }
    mHost->mPreviousScrollState = mNewState;
    return NOERROR;
}

//========================================================================================
//              CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::
//========================================================================================
CAR_INTERFACE_IMPL(CalendarView::LegacyCalendarViewDelegate::WeeksAdapter, BaseAdapter, IViewOnTouchListener)

CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::WeeksAdapter(
    /* [in] */ IContext* context,
    /* [in] */ LegacyCalendarViewDelegate* host)
    : mSelectedWeek(0)
    , mFocusedMonth(0)
    , mTotalWeekCount(0)
{
    AutoPtr<ICalendarHelper> hlp;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&hlp);
    hlp->GetInstance((ICalendar**)&mSelectedDate);
    assert(0 && "TODO");
//    mContext = context;
    AutoPtr<CalendarGestureListener> p = new CalendarGestureListener();
    assert(0 && "TODO");
//    CGestureDetector::New(mContext, p, (IGestureDetector**)&mGestureDetector);
    Init();
    mHost = host;
}

void CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::Init()
{
    mSelectedWeek = mHost->GetWeeksSinceMinDate(mSelectedDate);
    mTotalWeekCount = mHost->GetWeeksSinceMinDate(mHost->mMaxDate);
    Int32 min = 0, max = 0;
    mHost->mMinDate->Get(ICalendar::DAY_OF_WEEK, &min);
    mHost->mMaxDate->Get(ICalendar::DAY_OF_WEEK, &max);
    if (min != mHost->mFirstDayOfWeek
            || max != mHost->mFirstDayOfWeek) {
        mTotalWeekCount++;
    }
    NotifyDataSetChanged();
}

void CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::SetSelectedDay(
    /* [in] */ ICalendar* selectedDay)
{
    Int32 dfy = 0, mDfy = 0, y = 0, mY = 0;
    selectedDay->Get(ICalendar::DAY_OF_YEAR, &dfy);
    mSelectedDate->Get(ICalendar::DAY_OF_YEAR, &mDfy);
    selectedDay->Get(ICalendar::YEAR, &y);
    mSelectedDate->Get(ICalendar::YEAR, &mY);
    if (dfy == mDfy && y == mY) {
        return;
    }
    Int64 mils = 0;
    selectedDay->GetTimeInMillis(&mils);
    mSelectedDate->SetTimeInMillis(mils);
    mSelectedWeek = mHost->GetWeeksSinceMinDate(mSelectedDate);
    mSelectedDate->Get(ICalendar::MONTH, &mFocusedMonth);
    NotifyDataSetChanged();
}

AutoPtr<ICalendar> CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::GetSelectedDay()
{
    return mSelectedDate;
}

ECode CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::GetCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTotalWeekCount;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::GetItem(
    /* [in] */ Int32 position,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::GetItemId(
    /* [in] */ Int32 position,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    *result = position;
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** result)
{
    VALIDATE_NOT_NULL(result)
    AutoPtr<WeekView> weekView;
    if (convertView != NULL) {
        weekView = (WeekView*)convertView;
    }
    else {
        assert(0 && "TODO");
        // weekView = new WeekView(mContext);
        AutoPtr<AbsListView::LayoutParams> params = new AbsListView::LayoutParams();
        params->constructor(IViewGroupLayoutParams::WRAP_CONTENT,
                        IViewGroupLayoutParams::WRAP_CONTENT);
        weekView->SetLayoutParams(params);
        weekView->SetClickable(TRUE);
        weekView->SetOnTouchListener(this);
    }

    Int32 dfw = 0;
    mSelectedDate->Get(ICalendar::DAY_OF_WEEK, &dfw);
    Int32 selectedWeekDay = (mSelectedWeek == position) ? dfw : -1;
    weekView->Init(position, selectedWeekDay, mFocusedMonth);

    *result = weekView;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::SetFocusMonth(
    /* [in] */ Int32 month)
{
    if (mFocusedMonth == month) {
        return;
    }
    mFocusedMonth = month;
    NotifyDataSetChanged();
}

ECode CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::OnTouch(
    /* [in] */ IView* v,
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    Boolean bEnbl = FALSE, bTouch = FALSE;
    if ((IView::Probe(mHost->mListView)->IsEnabled(&bEnbl), bEnbl) &&
         (mGestureDetector->OnTouchEvent(event, &bTouch), bTouch)) {
        AutoPtr<WeekView> weekView = (WeekView*)v;
        // if we cannot find a day for the given location we are done
        Float x = 0.f;
        event->GetX(&x);
        if (!weekView->GetDayFromLocation(x, mHost->mTempDate)) {
            *result = TRUE;
            return NOERROR;
        }
        // it is possible that the touched day is outside the valid range
        // we draw whole weeks but range end can fall not on the week end
        Boolean bIsBf = FALSE, bIsAf = FALSE;
        if ((mHost->mTempDate->IsBefore(mHost->mMinDate, &bIsBf), bIsBf) ||
            (mHost->mTempDate->IsAfter(mHost->mMaxDate, &bIsAf), bIsAf)) {
            *result = TRUE;
            return NOERROR;
        }
        OnDateTapped(mHost->mTempDate);
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

void CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::OnDateTapped(
    /* [in] */ ICalendar* day)
{
    SetSelectedDay(day);
    mHost->SetMonthDisplayed(day);
}

//========================================================================================
//              CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::CalendarGestureListener::
//========================================================================================

ECode CalendarView::LegacyCalendarViewDelegate::WeeksAdapter::CalendarGestureListener::OnSingleTapUp(
    /* [in] */ IMotionEvent* e,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res)
    *res = TRUE;
    return NOERROR;
}

//========================================================================================
//              CalendarView::LegacyCalendarViewDelegate::WeekView::
//========================================================================================

CalendarView::LegacyCalendarViewDelegate::WeekView::WeekView(
    /* [in] */ IContext* context,
    /* [in] */ LegacyCalendarViewDelegate* host)
    : mHasFocusedDay(FALSE)
    , mHasUnfocusedDay(FALSE)
    , mMonthOfFirstWeekDay(-1)
    , mLastWeekDayMonth(-1)
    , mWeek(-1)
    , mWidth(0)
    , mHeight(0)
    , mHasSelectedDay(FALSE)
    , mSelectedDay(-1)
    , mNumCells(0)
    , mSelectedLeft(-1)
    , mSelectedRight(-1)
{
    View::constructor(context);

    CRect::New((IRect**)&mTempRect);

    CPaint::New((IPaint**)&mDrawPaint);

    CPaint::New((IPaint**)&mMonthNumDrawPaint);

    // Sets up any standard paints that will be used
    InitilaizePaints();

    mHost = host;
}

void CalendarView::LegacyCalendarViewDelegate::WeekView::Init(
    /* [in] */ Int32 weekNumber,
    /* [in] */ Int32 selectedWeekDay,
    /* [in] */ Int32 focusedMonth)
{
    mSelectedDay = selectedWeekDay;
    mHasSelectedDay = mSelectedDay != -1;
    mNumCells = mHost->mShowWeekNumber ? mHost->mDaysPerWeek + 1 : mHost->mDaysPerWeek;
    mWeek = weekNumber;
    Int64 mils = 0;
    mHost->mMinDate->GetTimeInMillis(&mils);
    mHost->mTempDate->SetTimeInMillis(mils);

    mHost->mTempDate->Add(ICalendar::WEEK_OF_YEAR, mWeek);
    mHost->mTempDate->SetFirstDayOfWeek(mHost->mFirstDayOfWeek);

    // Allocate space for caching the day numbers and focus values
    mDayNumbers = ArrayOf<String>::Alloc(mNumCells);
    mFocusDay = ArrayOf<Boolean>::Alloc(mNumCells);

    // If we're showing the week number calculate it based on Monday
    Int32 i = 0;
    if (mHost->mShowWeekNumber) {
        AutoPtr<ILocaleHelper> hlp;
        CLocaleHelper::AcquireSingleton((ILocaleHelper**)&hlp);
        AutoPtr<ILocale> loc;
        hlp->GetDefault((ILocale**)&loc);
        Int32 wy = 0;
        mHost->mTempDate->Get(ICalendar::WEEK_OF_YEAR, &wy);
        AutoPtr<IInteger32> pWy;
        CInteger32::New(wy, (IInteger32**)&pWy);
        AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(1);
        (*arr)[0] = pWy;
        (*mDayNumbers)[0] = StringUtils::Format(loc, String("%d"), arr);
        i++;
    }

    // Now adjust our starting day based on the start day of the week
    Int32 dw = 0;
    mHost->mTempDate->Get(ICalendar::DAY_OF_WEEK, &dw);
    Int32 diff = mHost->mFirstDayOfWeek - dw;
    mHost->mTempDate->Add(ICalendar::DAY_OF_MONTH, diff);

    assert(0 && "TODO");
//    mFirstDay = mHost->mTempDate->Clone();
    mHost->mTempDate->Get(ICalendar::MONTH, &mMonthOfFirstWeekDay);

    mHasUnfocusedDay = TRUE;
    for (; i < mNumCells; i++) {
        Int32 m = 0;
        mHost->mTempDate->Get(ICalendar::MONTH, &m);
        Boolean isFocusedDay = (m == focusedMonth);
        (*mFocusDay)[i] = isFocusedDay;
        mHasFocusedDay |= isFocusedDay;
        mHasUnfocusedDay &= !isFocusedDay;
        // do not draw dates outside the valid range to avoid user confusion
        Boolean bIsBf = FALSE, bIsAf = FALSE;
        if ((mHost->mTempDate->IsBefore(mHost->mMinDate, &bIsBf), bIsBf) ||
             (mHost->mTempDate->IsAfter(mHost->mMaxDate, &bIsAf), bIsAf)) {
            (*mDayNumbers)[i] = "";
        }
        else {
            AutoPtr<ILocaleHelper> hlp;
            CLocaleHelper::AcquireSingleton((ILocaleHelper**)&hlp);
            AutoPtr<ILocale> loc;
            hlp->GetDefault((ILocale**)&loc);
            Int32 m = 0;
            mHost->mTempDate->Get(ICalendar::DAY_OF_MONTH, &m);
            AutoPtr<IInteger32> pM;
            CInteger32::New(m, (IInteger32**)&pM);
            AutoPtr<ArrayOf<IInterface*> > arr = ArrayOf<IInterface*>::Alloc(1);
            (*arr)[0] = pM;
            (*mDayNumbers)[i] = StringUtils::Format(loc, String("%d"), arr);
        }
        mHost->mTempDate->Add(ICalendar::DAY_OF_MONTH, 1);
    }
    // We do one extra add at the end of the loop, if that pushed us to
    // new month undo it
    Int32 dm = 0;
    mHost->mTempDate->Get(ICalendar::DAY_OF_MONTH, &dm);
    if (dm == 1) {
        mHost->mTempDate->Add(ICalendar::DAY_OF_MONTH, -1);
    }
    mHost->mTempDate->Get(ICalendar::MONTH, &mLastWeekDayMonth);

    UpdateSelectionPositions();
}

void CalendarView::LegacyCalendarViewDelegate::WeekView::InitilaizePaints()
{
    mDrawPaint->SetFakeBoldText(FALSE);
    mDrawPaint->SetAntiAlias(TRUE);
    mDrawPaint->SetStyle(PaintStyle_FILL);

    mMonthNumDrawPaint->SetFakeBoldText(TRUE);
    mMonthNumDrawPaint->SetAntiAlias(TRUE);
    mMonthNumDrawPaint->SetStyle(PaintStyle_FILL);
    mMonthNumDrawPaint->SetTextAlign(PaintAlign_CENTER);
    mMonthNumDrawPaint->SetTextSize(mHost->mDateTextSize);
}

Int32 CalendarView::LegacyCalendarViewDelegate::WeekView::GetMonthOfFirstWeekDay()
{
    return mMonthOfFirstWeekDay;
}

Int32 CalendarView::LegacyCalendarViewDelegate::WeekView::GetMonthOfLastWeekDay()
{
    return mLastWeekDayMonth;
}

AutoPtr<ICalendar> CalendarView::LegacyCalendarViewDelegate::WeekView::GetFirstDay()
{
    return mFirstDay;
}

Boolean CalendarView::LegacyCalendarViewDelegate::WeekView::GetDayFromLocation(
    /* [in] */ float x,
    /* [in] */ ICalendar* outCalendar)
{
    Boolean isLayoutRtl = FALSE;
    IsLayoutRtl(&isLayoutRtl);

    Int32 start = 0;
    Int32 end = 0;

    if (isLayoutRtl) {
        start = 0;
        end = mHost->mShowWeekNumber ? mWidth - mWidth / mNumCells : mWidth;
    }
    else {
        start = mHost->mShowWeekNumber ? mWidth / mNumCells : 0;
        end = mWidth;
    }

    if (x < start || x > end) {
        outCalendar->Clear();
        return FALSE;
    }

    // Selection is (x - start) / (pixels/day) which is (x - start) * day / pixels
    Int32 dayPosition = (Int32) ((x - start) * mHost->mDaysPerWeek / (end - start));

    if (isLayoutRtl) {
        dayPosition = mHost->mDaysPerWeek - 1 - dayPosition;
    }

    Int64 mils = 0;
    mFirstDay->GetTimeInMillis(&mils);
    outCalendar->SetTimeInMillis(mils);
    outCalendar->Add(ICalendar::DAY_OF_MONTH, dayPosition);

    return TRUE;
}

void CalendarView::LegacyCalendarViewDelegate::WeekView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    DrawBackground(canvas);
    DrawWeekNumbersAndDates(canvas);
    DrawWeekSeparators(canvas);
    DrawSelectedDateVerticalBars(canvas);
}

void CalendarView::LegacyCalendarViewDelegate::WeekView::DrawBackground(
    /* [in] */ ICanvas* canvas)
{
    if (!mHasSelectedDay) {
        return;
    }
    mDrawPaint->SetColor(mHost->mSelectedWeekBackgroundColor);

    AutoPtr<CRect> ctr = (CRect*)mTempRect.Get();
    ctr->mTop = mHost->mWeekSeperatorLineWidth;
    ctr->mBottom = mHeight;

    Boolean isLayoutRtl = FALSE;
    IsLayoutRtl(&isLayoutRtl);

    if (isLayoutRtl) {
        ctr->mLeft = 0;
        ctr->mRight = mSelectedLeft - 2;
    }
    else {
        ctr->mLeft = mHost->mShowWeekNumber ? mWidth / mNumCells : 0;
        ctr->mRight = mSelectedLeft - 2;
    }
    canvas->DrawRect(mTempRect, mDrawPaint);

    if (isLayoutRtl) {
        ctr->mLeft = mSelectedRight + 3;
        ctr->mRight = mHost->mShowWeekNumber ? mWidth - mWidth / mNumCells : mWidth;
    }
    else {
        ctr->mLeft = mSelectedRight + 3;
        ctr->mRight = mWidth;
    }
    canvas->DrawRect(mTempRect, mDrawPaint);
}

void CalendarView::LegacyCalendarViewDelegate::WeekView::DrawWeekNumbersAndDates(
    /* [in] */ ICanvas* canvas)
{
    float textHeight = 0;
    mDrawPaint->GetTextSize(&textHeight);
    Int32 y = (Int32) ((mHeight + textHeight) / 2) - mHost->mWeekSeperatorLineWidth;
    Int32 nDays = mNumCells;
    Int32 divisor = 2 * nDays;

    mDrawPaint->SetTextAlign(PaintAlign_CENTER);
    mDrawPaint->SetTextSize(mHost->mDateTextSize);

    Int32 i = 0;

    Boolean bIsLo = FALSE;
    if ((IsLayoutRtl(&bIsLo), bIsLo)) {
        for (; i < nDays - 1; i++) {
            mMonthNumDrawPaint->SetColor((*mFocusDay)[i] ? mHost->mFocusedMonthDateColor
                    : mHost->mUnfocusedMonthDateColor);
            Int32 x = (2 * i + 1) * mWidth / divisor;
            canvas->DrawText((*mDayNumbers)[nDays - 1 - i], x, y, mMonthNumDrawPaint);
        }
        if (mHost->mShowWeekNumber) {
            mDrawPaint->SetColor(mHost->mWeekNumberColor);
            Int32 x = mWidth - mWidth / divisor;
            canvas->DrawText((*mDayNumbers)[0], x, y, mDrawPaint);
        }
    }
    else {
        if (mHost->mShowWeekNumber) {
            mDrawPaint->SetColor(mHost->mWeekNumberColor);
            Int32 x = mWidth / divisor;
            canvas->DrawText((*mDayNumbers)[0], x, y, mDrawPaint);
            i++;
        }
        for (; i < nDays; i++) {
            mMonthNumDrawPaint->SetColor((*mFocusDay)[i] ? mHost->mFocusedMonthDateColor
                    : mHost->mUnfocusedMonthDateColor);
            Int32 x = (2 * i + 1) * mWidth / divisor;
            canvas->DrawText((*mDayNumbers)[i], x, y, mMonthNumDrawPaint);
        }
    }
}

void CalendarView::LegacyCalendarViewDelegate::WeekView::DrawWeekSeparators(
    /* [in] */ ICanvas* canvas)
{
    // If it is the topmost fully visible child do not draw separator line
    Int32 firstFullyVisiblePosition = 0;
    IAdapterView::Probe(mHost->mListView)->GetFirstVisiblePosition(&firstFullyVisiblePosition);
    AutoPtr<IView> v;
    IViewGroup::Probe(mHost->mListView)->GetChildAt(0, (IView**)&v);
    Int32 t = 0;
    v->GetTop(&t);
    if (t < 0) {
        firstFullyVisiblePosition++;
    }
    if (firstFullyVisiblePosition == mWeek) {
        return;
    }
    mDrawPaint->SetColor(mHost->mWeekSeparatorLineColor);
    mDrawPaint->SetStrokeWidth(mHost->mWeekSeperatorLineWidth);
    float startX = 0.f;
    float stopX = 0.f;
    Boolean bIsLo = FALSE;
    if ((IsLayoutRtl(&bIsLo), bIsLo)) {
        startX = 0;
        stopX = mHost->mShowWeekNumber ? mWidth - mWidth / mNumCells : mWidth;
    }
    else {
        startX = mHost->mShowWeekNumber ? mWidth / mNumCells : 0;
        stopX = mWidth;
    }
    canvas->DrawLine(startX, 0, stopX, 0, mDrawPaint);
}

void CalendarView::LegacyCalendarViewDelegate::WeekView::DrawSelectedDateVerticalBars(
    /* [in] */ ICanvas* canvas)
{
    if (!mHasSelectedDay) {
        return;
    }
    mHost->mSelectedDateVerticalBar->SetBounds(
            mSelectedLeft - mHost->mSelectedDateVerticalBarWidth / 2,
            mHost->mWeekSeperatorLineWidth,
            mSelectedLeft + mHost->mSelectedDateVerticalBarWidth / 2,
            mHeight);
    mHost->mSelectedDateVerticalBar->Draw(canvas);
    mHost->mSelectedDateVerticalBar->SetBounds(
            mSelectedRight - mHost->mSelectedDateVerticalBarWidth / 2,
            mHost->mWeekSeperatorLineWidth,
            mSelectedRight + mHost->mSelectedDateVerticalBarWidth / 2,
            mHeight);
    mHost->mSelectedDateVerticalBar->Draw(canvas);
}

void CalendarView::LegacyCalendarViewDelegate::WeekView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    mWidth = w;
    UpdateSelectionPositions();
}

void CalendarView::LegacyCalendarViewDelegate::WeekView::UpdateSelectionPositions()
{
    if (mHasSelectedDay) {
        Boolean isLayoutRtl = FALSE;
        IsLayoutRtl(&isLayoutRtl);
        Int32 selectedPosition = mSelectedDay - mHost->mFirstDayOfWeek;
        if (selectedPosition < 0) {
            selectedPosition += 7;
        }
        if (mHost->mShowWeekNumber && !isLayoutRtl) {
            selectedPosition++;
        }
        if (isLayoutRtl) {
            mSelectedLeft = (mHost->mDaysPerWeek - 1 - selectedPosition) * mWidth / mNumCells;
        }
        else {
            mSelectedLeft = selectedPosition * mWidth / mNumCells;
        }
        mSelectedRight = mSelectedLeft + mWidth / mNumCells;
    }
}

void CalendarView::LegacyCalendarViewDelegate::WeekView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 h = 0, t = 0, b = 0;
    IView::Probe(mHost->mListView)->GetHeight(&h);
    IView::Probe(mHost->mListView)->GetPaddingTop(&t);
    IView::Probe(mHost->mListView)->GetPaddingBottom(&b);
    mHeight = (h - t - b) / mHost->mShownWeekCount;
    SetMeasuredDimension(MeasureSpec::GetSize(widthMeasureSpec), mHeight);
}

//========================================================================================
//              CalendarView::LegacyCalendarViewDelegate::_DataSetObserver::
//========================================================================================
CAR_INTERFACE_IMPL(CalendarView::LegacyCalendarViewDelegate::_DataSetObserver, Object, IDataSetObserver)

CalendarView::LegacyCalendarViewDelegate::_DataSetObserver::_DataSetObserver(
    /* [in] */ LegacyCalendarViewDelegate* host)
{
    mHost = host;
}

ECode CalendarView::LegacyCalendarViewDelegate::_DataSetObserver::OnChanged()
{
    if (mHost->mOnDateChangeListener != NULL) {
        AutoPtr<ICalendar> selectedDay = mHost->mAdapter->GetSelectedDay();
        Int32 y = 0, m = 0, d = 0;
        selectedDay->Get(ICalendar::YEAR, &y);
        selectedDay->Get(ICalendar::MONTH, &m);
        selectedDay->Get(ICalendar::DAY_OF_MONTH, &d);
        assert(0 && "TODO");
        // mHost->mOnDateChangeListener->OnSelectedDayChange(mDelegator, y, m, d);
    }
    return NOERROR;
}

ECode CalendarView::LegacyCalendarViewDelegate::_DataSetObserver::OnInvalidated()
{
    return NOERROR;
}

//========================================================================================
//              CalendarView::
//========================================================================================
const String CalendarView::TAG("CalendarView");

CAR_INTERFACE_IMPL(CalendarView, FrameLayout, ICalendarView)

ECode CalendarView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CalendarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::calendarViewStyle);
}

ECode CalendarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr)
{
    return constructor(context, attrs, defStyleAttr, 0);
}

ECode CalendarView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes)
{
    FrameLayout::constructor(context, attrs, defStyleAttr, defStyleRes);
    mDelegate = new LegacyCalendarViewDelegate(this, context, attrs, defStyleAttr, defStyleRes);
    return NOERROR;
}

ECode CalendarView::SetShownWeekCount(
    /* [in] */ Int32 count)
{
    return mDelegate->SetShownWeekCount(count);
}

ECode CalendarView::GetShownWeekCount(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetShownWeekCount(result);
}

ECode CalendarView::SetSelectedWeekBackgroundColor(
    /* [in] */ Int32 color)
{
    return mDelegate->SetSelectedWeekBackgroundColor(color);
}

ECode CalendarView::GetSelectedWeekBackgroundColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetSelectedWeekBackgroundColor(result);
}

ECode CalendarView::SetFocusedMonthDateColor(
    /* [in] */ Int32 color)
{
    return mDelegate->SetFocusedMonthDateColor(color);
}

ECode CalendarView::GetFocusedMonthDateColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetFocusedMonthDateColor(result);
}

ECode CalendarView::SetUnfocusedMonthDateColor(
    /* [in] */ Int32 color)
{
    return mDelegate->SetUnfocusedMonthDateColor(color);
}

ECode CalendarView::GetUnfocusedMonthDateColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetUnfocusedMonthDateColor(result);
}

ECode CalendarView::SetWeekNumberColor(
    /* [in] */ Int32 color)
{
    return mDelegate->SetWeekNumberColor(color);
}

ECode CalendarView::GetWeekNumberColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetWeekNumberColor(result);
}

ECode CalendarView::SetWeekSeparatorLineColor(
    /* [in] */ Int32 color)
{
    return mDelegate->SetWeekSeparatorLineColor(color);
}

ECode CalendarView::GetWeekSeparatorLineColor(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetWeekSeparatorLineColor(result);
}

ECode CalendarView::SetSelectedDateVerticalBar(
    /* [in] */ Int32 resourceId)
{
    return mDelegate->SetSelectedDateVerticalBar(resourceId);
}

ECode CalendarView::SetSelectedDateVerticalBar(
    /* [in] */ IDrawable* drawable)
{
    return mDelegate->SetSelectedDateVerticalBar(drawable);
}

ECode CalendarView::GetSelectedDateVerticalBar(
    /* [out] */ IDrawable** result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetSelectedDateVerticalBar(result);
}

ECode CalendarView::SetWeekDayTextAppearance(
    /* [in] */ Int32 resourceId)
{
    return mDelegate->SetWeekDayTextAppearance(resourceId);
}

ECode CalendarView::GetWeekDayTextAppearance(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetWeekDayTextAppearance(result);
}

ECode CalendarView::SetDateTextAppearance(
    /* [in] */ Int32 resourceId)
{
    return mDelegate->SetDateTextAppearance(resourceId);
}

ECode CalendarView::GetDateTextAppearance(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetDateTextAppearance(result);
}

ECode CalendarView::SetEnabled(
    /* [in] */ Boolean enabled)
{
    return mDelegate->SetEnabled(enabled);
}

ECode CalendarView::IsEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->IsEnabled(result);
}

ECode CalendarView::GetMinDate(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetMinDate(result);
}

ECode CalendarView::SetMinDate(
    /* [in] */ Int64 minDate)
{
    return mDelegate->SetMinDate(minDate);
}

ECode CalendarView::GetMaxDate(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetMaxDate(result);
}

ECode CalendarView::SetMaxDate(
    /* [in] */ Int64 maxDate)
{
    return mDelegate->SetMaxDate(maxDate);
}

ECode CalendarView::SetShowWeekNumber(
    /* [in] */ Boolean showWeekNumber)
{
    return mDelegate->SetShowWeekNumber(showWeekNumber);
}

ECode CalendarView::GetShowWeekNumber(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetShowWeekNumber(result);
}

ECode CalendarView::GetFirstDayOfWeek(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetFirstDayOfWeek(result);
}

ECode CalendarView::SetFirstDayOfWeek(
    /* [in] */ Int32 firstDayOfWeek)
{
    return mDelegate->SetFirstDayOfWeek(firstDayOfWeek);
}

ECode CalendarView::SetOnDateChangeListener(
    /* [in] */ IOnDateChangeListener* listener)
{
    return mDelegate->SetOnDateChangeListener(listener);
}

ECode CalendarView::GetDate(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result)
    return mDelegate->GetDate(result);
}

ECode CalendarView::SetDate(
    /* [in] */ Int64 date)
{
    return mDelegate->SetDate(date);
}

ECode CalendarView::SetDate(
    /* [in] */ Int64 date,
    /* [in] */ Boolean animate,
    /* [in] */ Boolean center)
{
    return mDelegate->SetDate(date, animate, center);
}

void CalendarView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    FrameLayout::OnConfigurationChanged(newConfig);
    mDelegate->OnConfigurationChanged(newConfig);
}

ECode CalendarView::OnInitializeAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event)
{
    FrameLayout::OnInitializeAccessibilityEvent(event);
    return mDelegate->OnInitializeAccessibilityEvent(event);
}

ECode CalendarView::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IAccessibilityNodeInfo* info)
{
    FrameLayout::OnInitializeAccessibilityNodeInfo(info);
    return mDelegate->OnInitializeAccessibilityNodeInfo(info);
}

}   //namespace Widget
}   //namespace Droid
}   //namespace Elastos
