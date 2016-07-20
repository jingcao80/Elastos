
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/R.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CRect.h"
#include "elastos/droid/graphics/CTypefaceHelper.h"
#include "elastos/droid/text/format/CDateFormat.h"
#include "elastos/droid/text/format/CDateUtils.h"
#include "elastos/droid/text/format/CTime.h"
#include "elastos/droid/text/format/CTimeHelper.h"
#include "elastos/droid/widget/SimpleMonthView.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/StringUtils.h"

using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::CTypefaceHelper;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Graphics::ITypefaceHelper;
using Elastos::Droid::Graphics::PaintAlign_CENTER;
using Elastos::Droid::Graphics::PaintStyle_FILL;
using Elastos::Droid::Internal::Widget::EIID_IExploreByTouchHelper;
using Elastos::Droid::Text::Format::CDateFormat;
using Elastos::Droid::Text::Format::CDateUtils;
using Elastos::Droid::Text::Format::CTime;
using Elastos::Droid::Text::Format::CTimeHelper;
using Elastos::Droid::Text::Format::IDateUtils;
using Elastos::Droid::Text::Format::ITimeHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeProvider;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::EIID_IView;
using Elastos::Droid::View::View;
using Elastos::Core::CoreUtils;
using Elastos::Core::CString;
using Elastos::Core::IAppendable;
using Elastos::Core::StringUtils;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Utility::CCalendarHelper;
using Elastos::Utility::CFormatter;
using Elastos::Utility::CLocaleHelper;
using Elastos::Utility::ICalendar;
using Elastos::Utility::ICalendarHelper;
using Elastos::Utility::ILocale;
using Elastos::Utility::ILocaleHelper;

namespace Elastos {
namespace Droid {
namespace Widget {

//=====================================================================
//                SimpleMonthView::MonthViewTouchHelper
//=====================================================================
const String SimpleMonthView::MonthViewTouchHelper::DATE_FORMAT("dd MMMM yyyy");

SimpleMonthView::MonthViewTouchHelper::MonthViewTouchHelper(
    /* [in] */ SimpleMonthView* owner,
    /* [in] */ IView* host)
    : mOwner(owner)
{
    ExploreByTouchHelper::constructor(host);

    assert(mOwner);
    CRect::New((IRect**)&mTempRect);

    AutoPtr<ICalendarHelper> helper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
    helper->GetInstance((ICalendar**)&mTempCalendar);
}

ECode SimpleMonthView::MonthViewTouchHelper::SetFocusedVirtualView(
    /* [in] */ Int32 virtualViewId)
{
    AutoPtr<IAccessibilityNodeProvider> provider;
    GetAccessibilityNodeProvider((IView*)mOwner, (IAccessibilityNodeProvider**)&provider);
    Boolean resTmp = FALSE;
    return provider->PerformAction(virtualViewId, IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS, NULL, &resTmp);
}

ECode SimpleMonthView::MonthViewTouchHelper::ClearFocusedVirtualView()
{
    Int32 focusedVirtualView = 0;
    GetFocusedVirtualView(&focusedVirtualView);
    if (focusedVirtualView != IExploreByTouchHelper::INVALID_ID) {
        AutoPtr<IAccessibilityNodeProvider> provider;
        GetAccessibilityNodeProvider((IView*)mOwner, (IAccessibilityNodeProvider**)&provider);
        Boolean resTmp = FALSE;
        provider->PerformAction(focusedVirtualView, IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS, NULL, &resTmp);
    }
    return NOERROR;
}

Int32 SimpleMonthView::MonthViewTouchHelper::GetVirtualViewAt(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Int32 day = mOwner->GetDayFromLocation(x, y);
    if (day >= 0) {
        return day;
    }
    return IExploreByTouchHelper::INVALID_ID;
}

void SimpleMonthView::MonthViewTouchHelper::GetVisibleVirtualViews(
    /* [in] */ IList* virtualViewIds)
{
    for (Int32 day = 1; day <= mOwner->mNumCells; ++day) {
        virtualViewIds->Add(CoreUtils::Convert(day));
    }
}

void SimpleMonthView::MonthViewTouchHelper::OnPopulateEventForVirtualView(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ IAccessibilityEvent* event)
{
    IAccessibilityRecord* recordTmp = IAccessibilityRecord::Probe(event);
    recordTmp->SetContentDescription(GetItemDescription(virtualViewId));
}

void SimpleMonthView::MonthViewTouchHelper::OnPopulateNodeForVirtualView(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ IAccessibilityNodeInfo* node)
{
    GetItemBounds(virtualViewId, mTempRect);
    node->SetContentDescription(GetItemDescription(virtualViewId));
    node->SetBoundsInParent(mTempRect);
    node->AddAction(IAccessibilityNodeInfo::ACTION_CLICK);

    if (virtualViewId == mOwner->mSelectedDay) {
        node->SetSelected(TRUE);
    }
}

Boolean SimpleMonthView::MonthViewTouchHelper::OnPerformActionForVirtualView(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ Int32 action,
    /* [in] */ IBundle* arguments)
{
    switch (action) {
        case IAccessibilityNodeInfo::ACTION_CLICK:
            mOwner->OnDayClick(virtualViewId);
            return TRUE;
    }
    return FALSE;
}

ECode SimpleMonthView::MonthViewTouchHelper::GetItemBounds(
    /* [in] */ Int32 day,
    /* [in] */ IRect* rect)
{
    VALIDATE_NOT_NULL(rect);

    const Int32 offsetX = mOwner->mPadding;
    const Int32 offsetY = mOwner->mMonthHeaderSize;
    const Int32 cellHeight = mOwner->mRowHeight;
    const Int32 cellWidth = ((mOwner->mWidth - (2 * mOwner->mPadding)) / mOwner->mNumDays);
    const Int32 index = ((day - 1) + mOwner->FindDayOffset());
    const Int32 row = (index / mOwner->mNumDays);
    const Int32 column = (index % mOwner->mNumDays);
    const Int32 x = (offsetX + (column * cellWidth));
    const Int32 y = (offsetY + (row * cellHeight));

    return rect->Set(x, y, (x + cellWidth), (y + cellHeight));
}

AutoPtr<ICharSequence> SimpleMonthView::MonthViewTouchHelper::GetItemDescription(
    /* [in] */ Int32 day)
{
    // ==================before translated======================
    // mTempCalendar.set(mYear, mMonth, day);
    // final CharSequence date = DateFormat.format(DATE_FORMAT,
    //         mTempCalendar.getTimeInMillis());
    //
    // if (day == mSelectedDay) {
    //     return getContext().getString(R.string.item_is_selected, date);
    // }
    //
    // return date;

    using Elastos::Droid::Text::Format::IDateFormat;
    mTempCalendar->Set(mOwner->mYear, mOwner->mMonth, day);
    Int64 timeMillis = 0;
    mTempCalendar->GetTimeInMillis(&timeMillis);
    AutoPtr<IDateFormat> dataFormat;
    CDateFormat::AcquireSingleton((IDateFormat**)&dataFormat);
    AutoPtr<ICharSequence> format;
    CString::New(DATE_FORMAT, (ICharSequence**)&format);
    AutoPtr<ICharSequence> date;
    dataFormat->Format(format, timeMillis, (ICharSequence**)&date);

    if (day == mOwner->mSelectedDay) {
        AutoPtr<IContext> context;
        mOwner->GetContext((IContext**)&context);

        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, date);

        String str;
        context->GetString(R::string::item_is_selected, args, &str);

        AutoPtr<ICharSequence> result;
        CString::New(str, (ICharSequence**)&result);
        return result;
    }
    return date;
}

//=====================================================================
//                           SimpleMonthView
//=====================================================================
const String SimpleMonthView::TAG("SimpleMonthView");
const Int32 SimpleMonthView::DEFAULT_HEIGHT = 32;
const Int32 SimpleMonthView::MIN_HEIGHT = 10;
const Int32 SimpleMonthView::DEFAULT_SELECTED_DAY = -1;
const Int32 SimpleMonthView::DEFAULT_WEEK_START = ICalendar::SUNDAY;
const Int32 SimpleMonthView::DEFAULT_NUM_DAYS = 7;
const Int32 SimpleMonthView::DEFAULT_NUM_ROWS = 6;
const Int32 SimpleMonthView::MAX_NUM_ROWS = 6;
const Int32 SimpleMonthView::SELECTED_CIRCLE_ALPHA = 60;
const Int32 SimpleMonthView::DAY_SEPARATOR_WIDTH = 1;
Float SimpleMonthView::mScale = 0;

CAR_INTERFACE_IMPL(SimpleMonthView, View, ISimpleMonthView)

SimpleMonthView::SimpleMonthView()
    : mMiniDayNumberTextSize(0)
    , mMonthLabelTextSize(0)
    , mMonthDayLabelTextSize(0)
    , mMonthHeaderSize(0)
    , mDaySelectedCircleSize(0)
    , mPadding(0)
    , mMonth(0)
    , mYear(0)
    , mWidth(0)
    , mRowHeight(DEFAULT_HEIGHT)
    , mHasToday(FALSE)
    , mSelectedDay(-1)
    , mToday(DEFAULT_SELECTED_DAY)
    , mWeekStart(DEFAULT_WEEK_START)
    , mNumDays(DEFAULT_NUM_DAYS)
    , mNumCells(mNumDays)
    , mDayOfWeekStart(0)
    , mEnabledDayStart(1)
    , mEnabledDayEnd(31)
    , mNumRows(DEFAULT_NUM_ROWS)
    , mNormalTextColor(0)
    , mDisabledTextColor(0)
    , mSelectedDayColor(0)
{
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    CSimpleDateFormat::New(String("EEEEE"), locale, (ISimpleDateFormat**)&mDayFormatter);

    AutoPtr<ICalendarHelper> calHelper;
    CCalendarHelper::AcquireSingleton((ICalendarHelper**)&calHelper);
    calHelper->GetInstance((ICalendar**)&mCalendar);
    calHelper->GetInstance((ICalendar**)&mDayLabelCalendar);
}

ECode SimpleMonthView::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode SimpleMonthView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, R::attr::datePickerStyle);
}

ECode SimpleMonthView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    View::constructor(context, attrs);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetString(R::string::day_of_week_label_typeface, &mDayOfWeekTypeface);
    res->GetString(R::string::sans_serif, &mMonthTitleTypeface);

    mStringBuilder = new StringBuilder(50);
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    CFormatter::New(IAppendable::Probe(mStringBuilder), locale, (IFormatter**)&mFormatter);

    res->GetDimensionPixelSize(R::dimen::datepicker_day_number_size, &mMiniDayNumberTextSize);
    res->GetDimensionPixelSize(R::dimen::datepicker_month_label_size, &mMonthLabelTextSize);
    res->GetDimensionPixelSize(R::dimen::datepicker_month_day_label_text_size, &mMonthDayLabelTextSize);
    res->GetDimensionPixelOffset(R::dimen::datepicker_month_list_item_header_height, &mMonthHeaderSize);
    res->GetDimensionPixelSize(R::dimen::datepicker_day_number_select_circle_radius, &mDaySelectedCircleSize);

    Int32 pixelOffsetTmp = 0;
    res->GetDimensionPixelOffset(R::dimen::datepicker_view_animator_height, &pixelOffsetTmp);
    mRowHeight = (pixelOffsetTmp - mMonthHeaderSize) / MAX_NUM_ROWS;

    // Set up accessibility components.
    mTouchHelper = new MonthViewTouchHelper(this, (IView*)this);
    SetAccessibilityDelegate(mTouchHelper);
    SetImportantForAccessibility(IMPORTANT_FOR_ACCESSIBILITY_YES);
    mLockAccessibilityDelegate = TRUE;

    // Sets up any standard paints that will be used
    InitView();
    return NOERROR;
}

ECode SimpleMonthView::SetTextColor(
    /* [in] */ IColorStateList* colors)
{
    VALIDATE_NOT_NULL(colors);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);

    Int32 color = 0;
    res->GetColor(R::color::datepicker_default_normal_text_color_holo_light, &color);
    colors->GetColorForState(ENABLED_STATE_SET, color, &mNormalTextColor);

    mMonthTitlePaint->SetColor(mNormalTextColor);
    mMonthDayLabelPaint->SetColor(mNormalTextColor);

    res->GetColor(R::color::datepicker_default_disabled_text_color_holo_light, &color);
    colors->GetColorForState(EMPTY_STATE_SET, color, &mDisabledTextColor);
    mDayNumberDisabledPaint->SetColor(mDisabledTextColor);

    res->GetColor(R::color::holo_blue_light, &color);
    colors->GetColorForState(ENABLED_SELECTED_STATE_SET, color, &mSelectedDayColor);

    mDayNumberSelectedPaint->SetColor(mSelectedDayColor);
    mDayNumberSelectedPaint->SetAlpha(SELECTED_CIRCLE_ALPHA);
    return NOERROR;
}

ECode SimpleMonthView::SetAccessibilityDelegate(
    /* [in] */ IAccessibilityDelegate* delegate)
{
    VALIDATE_NOT_NULL(delegate);
    // Workaround for a JB MR1 issue where accessibility delegates on
    // top-level ListView items are overwritten.
    if (!mLockAccessibilityDelegate) {
        View::SetAccessibilityDelegate(delegate);
    }
    return NOERROR;
}

ECode SimpleMonthView::SetOnDayClickListener(
    /* [in] */ IOnDayClickListener* listener)
{
    VALIDATE_NOT_NULL(listener);

    mOnDayClickListener = listener;
    return NOERROR;
}

Boolean SimpleMonthView::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    // First right-of-refusal goes the touch exploration helper.
    Boolean resTmp = FALSE;
    mTouchHelper->DispatchHoverEvent(event, &resTmp);
    if (resTmp) {
        return TRUE;
    }
    return View::DispatchHoverEvent(event);
}

ECode SimpleMonthView::OnTouchEvent(
    /* [in] */ IMotionEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(event);
    VALIDATE_NOT_NULL(result);

    Int32 action = 0;
    event->GetAction(&action);
    switch (action) {
        case IMotionEvent::ACTION_UP:
            Float x, y;
            event->GetX(&x);
            event->GetY(&y);
            const Int32 day = GetDayFromLocation(x, y);
            if (day >= 0) {
                OnDayClick(day);
            }
            break;
    }

    *result = TRUE;
    return NOERROR;
}

ECode SimpleMonthView::SetMonthParams(
    /* [in] */ Int32 selectedDay,
    /* [in] */ Int32 month,
    /* [in] */ Int32 year,
    /* [in] */ Int32 weekStart,
    /* [in] */ Int32 enabledDayStart,
    /* [in] */ Int32 enabledDayEnd)
{
    if (mRowHeight < MIN_HEIGHT) {
        mRowHeight = MIN_HEIGHT;
    }

    mSelectedDay = selectedDay;

    if (IsValidMonth(month)) {
        mMonth = month;
    }
    mYear = year;

    // Figure out what day today is
    AutoPtr<ITimeHelper> helper;
    CTimeHelper::AcquireSingleton((ITimeHelper**)&helper);
    String timeZone;
    helper->GetCurrentTimezone(&timeZone);
    AutoPtr<ITime> today;
    CTime::New(timeZone, (ITime**)&today);
    today->SetToNow();
    mHasToday = FALSE;
    mToday = -1;

    mCalendar->Set(ICalendar::MONTH, mMonth);
    mCalendar->Set(ICalendar::YEAR, mYear);
    mCalendar->Set(ICalendar::DAY_OF_MONTH, 1);
    mCalendar->Get(ICalendar::DAY_OF_WEEK, &mDayOfWeekStart);

    if (IsValidDayOfWeek(weekStart)) {
        mWeekStart = weekStart;
    }
    else {
        mCalendar->GetFirstDayOfWeek(&mWeekStart);
    }

    if (enabledDayStart > 0 && enabledDayEnd < 32) {
        mEnabledDayStart = enabledDayStart;
    }
    if (enabledDayEnd > 0 && enabledDayEnd < 32 && enabledDayEnd >= enabledDayStart) {
        mEnabledDayEnd = enabledDayEnd;
    }

    mNumCells = GetDaysInMonth(mMonth, mYear);
    Int32 day = 0;
    for (Int32 i = 0; i < mNumCells; i++) {
        day = i + 1;
        if (SameDay(day, today)) {
            mHasToday = TRUE;
            mToday = day;
        }
    }
    mNumRows = CalculateNumRows();

    // Invalidate cached accessibility information.
    mTouchHelper->InvalidateRoot();
    return NOERROR;
}

ECode SimpleMonthView::Reuse()
{
    mNumRows = DEFAULT_NUM_ROWS;
    RequestLayout();
    return NOERROR;
}

ECode SimpleMonthView::GetAccessibilityFocus(
    /* [out] */ ICalendar** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 day = 0;
    mTouchHelper->GetFocusedVirtualView(&day);
    AutoPtr<ICalendar> date;
    if (day >= 0) {
        AutoPtr<ICalendarHelper> helper;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
        helper->GetInstance((ICalendar**)&date);
        date->Set(mYear, mMonth, day);
    }

    *result = date;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode SimpleMonthView::ClearAccessibilityFocus()
{
    mTouchHelper->ClearFocusedVirtualView();
    return NOERROR;
}

ECode SimpleMonthView::RestoreAccessibilityFocus(
    /* [in] */ ICalendar* day,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(day);
    VALIDATE_NOT_NULL(result);

    Int32 year, month, dayOfMonth;
    day->Get(ICalendar::YEAR, &year);
    day->Get(ICalendar::MONTH, &month);
    day->Get(ICalendar::DAY_OF_MONTH, &dayOfMonth);
    if ((year != mYear) || (month != mMonth) || (dayOfMonth > mNumCells)) {
        *result = FALSE;
        return NOERROR;
    }

    mTouchHelper->SetFocusedVirtualView(dayOfMonth);
    *result = TRUE;
    return NOERROR;
}

void SimpleMonthView::OnConfigurationChanged(
    /* [in] */ IConfiguration* newConfig)
{
    View::OnConfigurationChanged(newConfig);
    AutoPtr<ILocale> locale;
    newConfig->GetLocale((ILocale**)&locale);
    CSimpleDateFormat::New(String("EEEEE"), locale, (ISimpleDateFormat**)&mDayFormatter);
}

void SimpleMonthView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    DrawMonthTitle(canvas);
    DrawWeekDayLabels(canvas);
    DrawDays(canvas);
}

ECode SimpleMonthView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    SetMeasuredDimension(MeasureSpec::GetSize(widthMeasureSpec), mRowHeight * mNumRows + mMonthHeaderSize);
    return NOERROR;
}

void SimpleMonthView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    mWidth = w;

    // Invalidate cached accessibility information.
    mTouchHelper->InvalidateRoot();
}

ECode SimpleMonthView::InitView()
{
    AutoPtr<ITypefaceHelper> helper;
    CTypefaceHelper::AcquireSingleton((ITypefaceHelper**)&helper);

    CPaint::New((IPaint**)&mMonthTitlePaint);
    mMonthTitlePaint->SetAntiAlias(TRUE);
    mMonthTitlePaint->SetColor(mNormalTextColor);
    mMonthTitlePaint->SetTextSize(mMonthLabelTextSize);
    AutoPtr<ITypeface> monthTitleTypeface;
    helper->Create(mMonthTitleTypeface, ITypeface::BOLD, (ITypeface**)&monthTitleTypeface);
    mMonthTitlePaint->SetTypeface(monthTitleTypeface);
    mMonthTitlePaint->SetTextAlign(PaintAlign_CENTER);
    mMonthTitlePaint->SetStyle(PaintStyle_FILL);
    mMonthTitlePaint->SetFakeBoldText(TRUE);

    CPaint::New((IPaint**)&mMonthDayLabelPaint);
    mMonthDayLabelPaint->SetAntiAlias(TRUE);
    mMonthDayLabelPaint->SetColor(mNormalTextColor);
    mMonthDayLabelPaint->SetTextSize(mMonthDayLabelTextSize);
    AutoPtr<ITypeface> monthDayLabelTypeface;
    helper->Create(mDayOfWeekTypeface, ITypeface::NORMAL, (ITypeface**)&monthDayLabelTypeface);
    mMonthDayLabelPaint->SetTypeface(monthDayLabelTypeface);
    mMonthDayLabelPaint->SetTextAlign(PaintAlign_CENTER);
    mMonthDayLabelPaint->SetStyle(PaintStyle_FILL);
    mMonthDayLabelPaint->SetFakeBoldText(TRUE);

    CPaint::New((IPaint**)&mDayNumberSelectedPaint);
    mDayNumberSelectedPaint->SetAntiAlias(TRUE);
    mDayNumberSelectedPaint->SetColor(mSelectedDayColor);
    mDayNumberSelectedPaint->SetAlpha(SELECTED_CIRCLE_ALPHA);
    mDayNumberSelectedPaint->SetTextAlign(PaintAlign_CENTER);
    mDayNumberSelectedPaint->SetStyle(PaintStyle_FILL);
    mDayNumberSelectedPaint->SetFakeBoldText(TRUE);

    CPaint::New((IPaint**)&mDayNumberPaint);
    mDayNumberPaint->SetAntiAlias(TRUE);
    mDayNumberPaint->SetTextSize(mMiniDayNumberTextSize);
    mDayNumberPaint->SetTextAlign(PaintAlign_CENTER);
    mDayNumberPaint->SetStyle(PaintStyle_FILL);
    mDayNumberPaint->SetFakeBoldText(FALSE);

    CPaint::New((IPaint**)&mDayNumberDisabledPaint);
    mDayNumberDisabledPaint->SetAntiAlias(TRUE);
    mDayNumberDisabledPaint->SetColor(mDisabledTextColor);
    mDayNumberDisabledPaint->SetTextSize(mMiniDayNumberTextSize);
    mDayNumberDisabledPaint->SetTextAlign(PaintAlign_CENTER);
    mDayNumberDisabledPaint->SetStyle(PaintStyle_FILL);
    mDayNumberDisabledPaint->SetFakeBoldText(FALSE);
    return NOERROR;
}

Boolean SimpleMonthView::IsValidDayOfWeek(
    /* [in] */ Int32 day)
{
    return (day >= ICalendar::SUNDAY && day <= ICalendar::SATURDAY);
}

Boolean SimpleMonthView::IsValidMonth(
    /* [in] */ Int32 month)
{
    return (month >= ICalendar::JANUARY && month <= ICalendar::DECEMBER);
}

Int32 SimpleMonthView::GetDaysInMonth(
    /* [in] */ Int32 month,
    /* [in] */ Int32 year)
{
    switch (month) {
        case ICalendar::JANUARY:
        case ICalendar::MARCH:
        case ICalendar::MAY:
        case ICalendar::JULY:
        case ICalendar::AUGUST:
        case ICalendar::OCTOBER:
        case ICalendar::DECEMBER:
            return 31;
        case ICalendar::APRIL:
        case ICalendar::JUNE:
        case ICalendar::SEPTEMBER:
        case ICalendar::NOVEMBER:
            return 30;
        case ICalendar::FEBRUARY:
            return (year % 4 == 0) ? 29 : 28;
        default:
            //throw new IllegalArgumentException("Invalid Month");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
}

Int32 SimpleMonthView::CalculateNumRows()
{
    Int32 offset = FindDayOffset();
    Int32 dividend = (offset + mNumCells) / mNumDays;
    Int32 remainder = (offset + mNumCells) % mNumDays;
    return (dividend + (remainder > 0 ? 1 : 0));
}

Boolean SimpleMonthView::SameDay(
    /* [in] */ Int32 day,
    /* [in] */ ITime* today)
{
    Int32 year, month, monthDay;
    today->GetYear(&year);
    today->GetMonth(&month);
    today->GetMonthDay(&monthDay);
    return (mYear == year && mMonth == month && day == monthDay);
}

String SimpleMonthView::GetMonthAndYearString()
{
    Int32 flags = IDateUtils::FORMAT_SHOW_DATE | IDateUtils::FORMAT_SHOW_YEAR | IDateUtils::FORMAT_NO_MONTH_DAY;
    mStringBuilder->SetLength(0);
    Int64 millis = 0;
    mCalendar->GetTimeInMillis(&millis);

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);

    AutoPtr<ITimeHelper> helper;
    CTimeHelper::AcquireSingleton((ITimeHelper**)&helper);
    String timeZone;
    helper->GetCurrentTimezone(&timeZone);

    AutoPtr<IDateUtils> dateUtils;
    CDateUtils::AcquireSingleton((IDateUtils**)&dateUtils);
    AutoPtr<IFormatter> formatter;
    dateUtils->FormatDateRange(context, mFormatter, millis, millis, flags, timeZone, (IFormatter**)&formatter);

    String result;
    formatter->ToString(&result);
    return result;
}

void SimpleMonthView::DrawMonthTitle(
    /* [in] */ ICanvas* canvas)
{
    assert(canvas);
    const Float x = (mWidth + 2 * mPadding) / 2.0f;
    const Float y = (mMonthHeaderSize - mMonthDayLabelTextSize) / 2.0f;
    canvas->DrawText(GetMonthAndYearString(), x, y, mMonthTitlePaint);
}

void SimpleMonthView::DrawWeekDayLabels(
    /* [in] */ ICanvas* canvas)
{
    assert(canvas);

    const Int32 y = mMonthHeaderSize - (mMonthDayLabelTextSize / 2);
    const Int32 dayWidthHalf = (mWidth - mPadding * 2) / (mNumDays * 2);

    String dayLabel;
    Int32 x, calendarDay;
    using Elastos::Text::IDateFormat;
    IDateFormat* dateFormat = IDateFormat::Probe(mDayFormatter);
    for (Int32 i = 0; i < mNumDays; i++) {
        calendarDay = (i + mWeekStart) % mNumDays;
        mDayLabelCalendar->Set(ICalendar::DAY_OF_WEEK, calendarDay);

        AutoPtr<IDate> date;
        mDayLabelCalendar->GetTime((IDate**)&date);
        dateFormat->Format(date, &dayLabel);
        x = (2 * i + 1) * dayWidthHalf + mPadding;
        canvas->DrawText(dayLabel, x, y, mMonthDayLabelPaint);
    }
}

void SimpleMonthView::DrawDays(
    /* [in] */ ICanvas* canvas)
{
    assert(canvas);

    Int32 y = (((mRowHeight + mMiniDayNumberTextSize) / 2) - DAY_SEPARATOR_WIDTH) + mMonthHeaderSize;
    Int32 dayWidthHalf = (mWidth - mPadding * 2) / (mNumDays * 2);
    Int32 j = FindDayOffset();
    Int32 x = 0;

    for (Int32 day = 1; day <= mNumCells; day++) {
        x = (2 * j + 1) * dayWidthHalf + mPadding;
        if (mSelectedDay == day) {
            canvas->DrawCircle(x, y - (mMiniDayNumberTextSize / 3), mDaySelectedCircleSize, mDayNumberSelectedPaint);
        }

        if (mHasToday && mToday == day) {
            mDayNumberPaint->SetColor(mSelectedDayColor);
        }
        else {
            mDayNumberPaint->SetColor(mNormalTextColor);
        }

        IPaint* paint = (day < mEnabledDayStart || day > mEnabledDayEnd) ? mDayNumberDisabledPaint : mDayNumberPaint;
        String str("");
        str.AppendFormat("%d", day);
        canvas->DrawText(str, x, y, paint);
        j++;
        if (j == mNumDays) {
            j = 0;
            y += mRowHeight;
        }
    }
}

Int32 SimpleMonthView::FindDayOffset()
{
    return (mDayOfWeekStart < mWeekStart ? (mDayOfWeekStart + mNumDays) : mDayOfWeekStart) - mWeekStart;
}

Int32 SimpleMonthView::GetDayFromLocation(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    Int32 dayStart = mPadding;
    if (x < dayStart || x > mWidth - mPadding) {
        return -1;
    }
    // Selection is (x - start) / (pixels/day) == (x -s) * day / pixels
    Int32 row = (Int32) (y - mMonthHeaderSize) / mRowHeight;
    Int32 column = (Int32) ((x - dayStart) * mNumDays / (mWidth - dayStart - mPadding));

    Int32 day = column - FindDayOffset() + 1;
    day += row * mNumDays;
    if (day < 1 || day > mNumCells) {
        return -1;
    }
    return day;
}

void SimpleMonthView::OnDayClick(
    /* [in] */ Int32 day)
{
    if (mOnDayClickListener != NULL) {
        AutoPtr<ICalendarHelper> helper;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
        AutoPtr<ICalendar> date;
        helper->GetInstance((ICalendar**)&date);
        date->Set(mYear, mMonth, day);
        mOnDayClickListener->OnDayClick(this, date);
    }

    // This is a no-op if accessibility is turned off.
    Boolean res;
    mTouchHelper->SendEventForVirtualView(day, IAccessibilityEvent::TYPE_VIEW_CLICKED, &res);
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos
