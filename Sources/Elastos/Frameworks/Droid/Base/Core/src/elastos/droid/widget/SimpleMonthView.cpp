
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/R.h"
#include "elastos/droid/graphics/CPaint.h"
#include "elastos/droid/graphics/CTypefaceHelper.h"
#include "elastos/droid/text/format/CDateFormat.h"
#include "elastos/droid/text/format/CDateUtils.h"
#include "elastos/droid/text/format/CTime.h"
#include "elastos/droid/text/format/CTimeHelper.h"
#include "elastos/droid/widget/SimpleMonthView.h"
#include "elastos/core/CoreUtils.h"
#include "elastos/core/StringUtils.h"

using Elastos::Droid::Graphics::CPaint;
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
    /* [in] */ SimpleMonthView* owner)
    : mOwner(owner)
{
    // ==================before translated======================
    // super(host);

    assert(mOwner);
}

ECode SimpleMonthView::MonthViewTouchHelper::SetFocusedVirtualView(
    /* [in] */ Int32 virtualViewId)
{
    // ==================before translated======================
    // getAccessibilityNodeProvider(SimpleMonthView.this).performAction(
    //         virtualViewId, AccessibilityNodeInfo.ACTION_ACCESSIBILITY_FOCUS, null);

    IView* self = THIS_PROBE(IView);
    AutoPtr<IAccessibilityNodeProvider> provider;
    self->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
    Boolean resTmp = FALSE;
    provider->PerformAction(virtualViewId, IAccessibilityNodeInfo::ACTION_ACCESSIBILITY_FOCUS, NULL, &resTmp);
    return NOERROR;
}

ECode SimpleMonthView::MonthViewTouchHelper::ClearFocusedVirtualView()
{
    // ==================before translated======================
    // final int focusedVirtualView = getFocusedVirtualView();
    // if (focusedVirtualView != ExploreByTouchHelper.INVALID_ID) {
    //     getAccessibilityNodeProvider(SimpleMonthView.this).performAction(
    //             focusedVirtualView,
    //             AccessibilityNodeInfo.ACTION_CLEAR_ACCESSIBILITY_FOCUS,
    //             null);
    // }

    IExploreByTouchHelper* self = (IExploreByTouchHelper*)this;
    Int32 focusedVirtualView = 0;
    self->GetFocusedVirtualView(&focusedVirtualView);
    if (focusedVirtualView != IExploreByTouchHelper::INVALID_ID) {
        IView* self = THIS_PROBE(IView);
        AutoPtr<IAccessibilityNodeProvider> provider;
        self->GetAccessibilityNodeProvider((IAccessibilityNodeProvider**)&provider);
        Boolean resTmp = FALSE;
        provider->PerformAction(focusedVirtualView, IAccessibilityNodeInfo::ACTION_CLEAR_ACCESSIBILITY_FOCUS, NULL, &resTmp);
    }
    return NOERROR;
}

Int32 SimpleMonthView::MonthViewTouchHelper::GetVirtualViewAt(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    // ==================before translated======================
    // final int day = getDayFromLocation(x, y);
    // if (day >= 0) {
    //     return day;
    // }
    // return ExploreByTouchHelper.INVALID_ID;

    Int32 day = mOwner->GetDayFromLocation(x, y);
    if (day >= 0) {
        return day;
    }
    return IExploreByTouchHelper::INVALID_ID;
}

void SimpleMonthView::MonthViewTouchHelper::GetVisibleVirtualViews(
    /* [in] */ IList* virtualViewIds)
{
    // ==================before translated======================
    // for (int day = 1; day <= mNumCells; day++) {
    //     virtualViewIds.add(day);
    // }

    for (Int32 day = 1; day <= mOwner->mNumCells; ++day) {
        AutoPtr<IInteger32> integer = CoreUtils::Convert(day);
        virtualViewIds->Add(TO_IINTERFACE(integer));
    }
}

void SimpleMonthView::MonthViewTouchHelper::OnPopulateEventForVirtualView(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ IAccessibilityEvent* event)
{
    // ==================before translated======================
    // event.setContentDescription(getItemDescription(virtualViewId));

    IAccessibilityRecord* recordTmp = IAccessibilityRecord::Probe(event);
    recordTmp->SetContentDescription(GetItemDescription(virtualViewId));
}

void SimpleMonthView::MonthViewTouchHelper::OnPopulateNodeForVirtualView(
    /* [in] */ Int32 virtualViewId,
    /* [in] */ IAccessibilityNodeInfo* node)
{
    // ==================before translated======================
    // getItemBounds(virtualViewId, mTempRect);
    //
    // node.setContentDescription(getItemDescription(virtualViewId));
    // node.setBoundsInParent(mTempRect);
    // node.addAction(AccessibilityNodeInfo.ACTION_CLICK);
    //
    // if (virtualViewId == mSelectedDay) {
    //     node.setSelected(true);
    // }

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
    // ==================before translated======================
    // switch (action) {
    //     case AccessibilityNodeInfo.ACTION_CLICK:
    //         onDayClick(virtualViewId);
    //         return true;
    // }
    //
    // return false;

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
    // ==================before translated======================
    // final int offsetX = mPadding;
    // final int offsetY = mMonthHeaderSize;
    // final int cellHeight = mRowHeight;
    // final int cellWidth = ((mWidth - (2 * mPadding)) / mNumDays);
    // final int index = ((day - 1) + findDayOffset());
    // final int row = (index / mNumDays);
    // final int column = (index % mNumDays);
    // final int x = (offsetX + (column * cellWidth));
    // final int y = (offsetY + (row * cellHeight));
    //
    // rect.set(x, y, (x + cellWidth), (y + cellHeight));

    const Int32 offsetX = mOwner->mPadding;
    const Int32 offsetY = mOwner->mMonthHeaderSize;
    const Int32 cellHeight = mOwner->mRowHeight;
    const Int32 cellWidth = ((mOwner->mWidth - (2 * mOwner->mPadding)) / mOwner->mNumDays);
    const Int32 index = ((day - 1) + mOwner->FindDayOffset());
    const Int32 row = (index / mOwner->mNumDays);
    const Int32 column = (index % mOwner->mNumDays);
    const Int32 x = (offsetX + (column * cellWidth));
    const Int32 y = (offsetY + (row * cellHeight));
    rect->Set(x, y, (x + cellWidth), (y + cellHeight));
    return NOERROR;
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
        IView* self = THIS_PROBE(IView);
        AutoPtr<IContext> context;
        self->GetContext((IContext**)&context);

        AutoPtr< ArrayOf<IInterface*> > args = ArrayOf<IInterface*>::Alloc(1);
        args->Set(0, TO_IINTERFACE(date));

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
const Int32 SimpleMonthView::DEFAULT_HEIGHT;
const Int32 SimpleMonthView::MIN_HEIGHT;
const Int32 SimpleMonthView::DEFAULT_SELECTED_DAY;
const Int32 SimpleMonthView::DEFAULT_WEEK_START;
const Int32 SimpleMonthView::DEFAULT_NUM_DAYS;
const Int32 SimpleMonthView::DEFAULT_NUM_ROWS;
const Int32 SimpleMonthView::MAX_NUM_ROWS;
const Int32 SimpleMonthView::SELECTED_CIRCLE_ALPHA;
const Int32 SimpleMonthView::DAY_SEPARATOR_WIDTH;
Float SimpleMonthView::mScale = 0;

CAR_INTERFACE_IMPL(SimpleMonthView, View, ISimpleMonthView)

SimpleMonthView::SimpleMonthView()
{
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    CSimpleDateFormat::New(String("EEEEE"), locale, (ISimpleDateFormat**)&mDayFormatter);
}

ECode SimpleMonthView::constructor(
    /* [in] */ IContext* context)
{
    // ==================before translated======================
    // this(context, null);

    return constructor(context, NULL);
}

ECode SimpleMonthView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    // ==================before translated======================
    // this(context, attrs, R.attr.datePickerStyle);

    return constructor(context, attrs, R::attr::datePickerStyle);
}

ECode SimpleMonthView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    // ==================before translated======================
    // super(context, attrs);
    //
    // final Resources res = context.getResources();
    //
    // mDayOfWeekTypeface = res.getString(R.string.day_of_week_label_typeface);
    // mMonthTitleTypeface = res.getString(R.string.sans_serif);
    //
    // mStringBuilder = new StringBuilder(50);
    // mFormatter = new Formatter(mStringBuilder, Locale.getDefault());
    //
    // mMiniDayNumberTextSize = res.getDimensionPixelSize(R.dimen.datepicker_day_number_size);
    // mMonthLabelTextSize = res.getDimensionPixelSize(R.dimen.datepicker_month_label_size);
    // mMonthDayLabelTextSize = res.getDimensionPixelSize(
    //         R.dimen.datepicker_month_day_label_text_size);
    // mMonthHeaderSize = res.getDimensionPixelOffset(
    //         R.dimen.datepicker_month_list_item_header_height);
    // mDaySelectedCircleSize = res.getDimensionPixelSize(
    //         R.dimen.datepicker_day_number_select_circle_radius);
    //
    // mRowHeight = (res.getDimensionPixelOffset(R.dimen.datepicker_view_animator_height)
    //         - mMonthHeaderSize) / MAX_NUM_ROWS;
    //
    // // Set up accessibility components.
    // mTouchHelper = new MonthViewTouchHelper(this);
    // setAccessibilityDelegate(mTouchHelper);
    // setImportantForAccessibility(IMPORTANT_FOR_ACCESSIBILITY_YES);
    // mLockAccessibilityDelegate = true;
    //
    // // Sets up any standard paints that will be used
    // initView();

    View::constructor(context, attrs);

    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    res->GetString(R::string::day_of_week_label_typeface, &mDayOfWeekTypeface);
    res->GetString(R::string::sans_serif, &mDayOfWeekTypeface);

    mStringBuilder = new StringBuilder(50);
    IAppendable* appendable = IAppendable::Probe(mStringBuilder);
    AutoPtr<ILocaleHelper> helper;
    CLocaleHelper::AcquireSingleton((ILocaleHelper**)&helper);
    AutoPtr<ILocale> locale;
    helper->GetDefault((ILocale**)&locale);
    CFormatter::New(appendable, locale, (IFormatter**)&mFormatter);

    res->GetDimensionPixelSize(R::dimen::datepicker_day_number_size, &mMiniDayNumberTextSize);
    res->GetDimensionPixelSize(R::dimen::datepicker_month_label_size, &mMonthLabelTextSize);
    res->GetDimensionPixelSize(R::dimen::datepicker_month_day_label_text_size, &mMonthDayLabelTextSize);
    res->GetDimensionPixelOffset(R::dimen::datepicker_month_list_item_header_height, &mMonthHeaderSize);
    res->GetDimensionPixelSize(R::dimen::datepicker_day_number_select_circle_radius, &mDaySelectedCircleSize);
    Int32 pixelOffsetTmp = 0;
    res->GetDimensionPixelOffset(R::dimen::datepicker_view_animator_height, &pixelOffsetTmp);
    mRowHeight = (pixelOffsetTmp - mMonthHeaderSize) / MAX_NUM_ROWS;

    // Set up accessibility components.
    //-- base class has pure func: mTouchHelper = new SimpleMonthView::MonthViewTouchHelper(this);
    IAccessibilityDelegate* delegate = IAccessibilityDelegate::Probe(mTouchHelper);
    SetAccessibilityDelegate(delegate);
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
    // ==================before translated======================
    // final Resources res = getContext().getResources();
    //
    // mNormalTextColor = colors.getColorForState(ENABLED_STATE_SET,
    //         res.getColor(R.color.datepicker_default_normal_text_color_holo_light));
    // mMonthTitlePaint.setColor(mNormalTextColor);
    // mMonthDayLabelPaint.setColor(mNormalTextColor);
    //
    // mDisabledTextColor = colors.getColorForState(EMPTY_STATE_SET,
    //         res.getColor(R.color.datepicker_default_disabled_text_color_holo_light));
    // mDayNumberDisabledPaint.setColor(mDisabledTextColor);
    //
    // mSelectedDayColor = colors.getColorForState(ENABLED_SELECTED_STATE_SET,
    //         res.getColor(R.color.holo_blue_light));
    // mDayNumberSelectedPaint.setColor(mSelectedDayColor);
    // mDayNumberSelectedPaint.setAlpha(SELECTED_CIRCLE_ALPHA);

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
    // ==================before translated======================
    // // Workaround for a JB MR1 issue where accessibility delegates on
    // // top-level ListView items are overwritten.
    // if (!mLockAccessibilityDelegate) {
    //     super.setAccessibilityDelegate(delegate);
    // }

    if (!mLockAccessibilityDelegate) {
        View::SetAccessibilityDelegate(delegate);
    }
    return NOERROR;
}

ECode SimpleMonthView::SetOnDayClickListener(
    /* [in] */ IOnDayClickListener* listener)
{
    VALIDATE_NOT_NULL(listener);
    // ==================before translated======================
    // mOnDayClickListener = listener;

    mOnDayClickListener = listener;
    return NOERROR;
}

Boolean SimpleMonthView::DispatchHoverEvent(
    /* [in] */ IMotionEvent* event)
{
    // ==================before translated======================
    // // First right-of-refusal goes the touch exploration helper.
    // if (mTouchHelper.dispatchHoverEvent(event)) {
    //     return true;
    // }
    // return super.dispatchHoverEvent(event);

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
    // ==================before translated======================
    // switch (event.getAction()) {
    //     case MotionEvent.ACTION_UP:
    //         final int day = getDayFromLocation(event.getX(), event.getY());
    //         if (day >= 0) {
    //             onDayClick(day);
    //         }
    //         break;
    // }
    // return true;

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
    // ==================before translated======================
    // if (mRowHeight < MIN_HEIGHT) {
    //     mRowHeight = MIN_HEIGHT;
    // }
    //
    // mSelectedDay = selectedDay;
    //
    // if (isValidMonth(month)) {
    //     mMonth = month;
    // }
    // mYear = year;
    //
    // // Figure out what day today is
    // final Time today = new Time(Time.getCurrentTimezone());
    // today.setToNow();
    // mHasToday = false;
    // mToday = -1;
    //
    // mCalendar.set(Calendar.MONTH, mMonth);
    // mCalendar.set(Calendar.YEAR, mYear);
    // mCalendar.set(Calendar.DAY_OF_MONTH, 1);
    // mDayOfWeekStart = mCalendar.get(Calendar.DAY_OF_WEEK);
    //
    // if (isValidDayOfWeek(weekStart)) {
    //     mWeekStart = weekStart;
    // } else {
    //     mWeekStart = mCalendar.getFirstDayOfWeek();
    // }
    //
    // if (enabledDayStart > 0 && enabledDayEnd < 32) {
    //     mEnabledDayStart = enabledDayStart;
    // }
    // if (enabledDayEnd > 0 && enabledDayEnd < 32 && enabledDayEnd >= enabledDayStart) {
    //     mEnabledDayEnd = enabledDayEnd;
    // }
    //
    // mNumCells = getDaysInMonth(mMonth, mYear);
    // for (int i = 0; i < mNumCells; i++) {
    //     final int day = i + 1;
    //     if (sameDay(day, today)) {
    //         mHasToday = true;
    //         mToday = day;
    //     }
    // }
    // mNumRows = calculateNumRows();
    //
    // // Invalidate cached accessibility information.
    // mTouchHelper.invalidateRoot();

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
    // ==================before translated======================
    // mNumRows = DEFAULT_NUM_ROWS;
    // requestLayout();

    mNumRows = DEFAULT_NUM_ROWS;
    RequestLayout();
    return NOERROR;
}

ECode SimpleMonthView::GetAccessibilityFocus(
    /* [out] */ ICalendar** result)
{
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // final int day = mTouchHelper.getFocusedVirtualView();
    // Calendar date = null;
    // if (day >= 0) {
    //     date = Calendar.getInstance();
    //     date.set(mYear, mMonth, day);
    // }
    // return date;

    Int32 day = 0;
    mTouchHelper->GetFocusedVirtualView(&day);
    AutoPtr<ICalendar> date = NULL;
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
    // ==================before translated======================
    // mTouchHelper.clearFocusedVirtualView();

    mTouchHelper->ClearFocusedVirtualView();
    return NOERROR;
}

ECode SimpleMonthView::RestoreAccessibilityFocus(
    /* [in] */ ICalendar* day,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(day);
    VALIDATE_NOT_NULL(result);
    // ==================before translated======================
    // if ((day.get(Calendar.YEAR) != mYear) || (day.get(Calendar.MONTH) != mMonth) ||
    //         (day.get(Calendar.DAY_OF_MONTH) > mNumCells)) {
    //     return false;
    // }
    // mTouchHelper.setFocusedVirtualView(day.get(Calendar.DAY_OF_MONTH));
    // return true;

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
    // ==================before translated======================
    // super.onConfigurationChanged(newConfig);
    //
    // mDayFormatter = new SimpleDateFormat("EEEEE", newConfig.locale);

    View::OnConfigurationChanged(newConfig);
    AutoPtr<ILocale> locale;
    newConfig->GetLocale((ILocale**)&locale);
    CSimpleDateFormat::New(String("EEEEE"), locale, (ISimpleDateFormat**)&mDayFormatter);
}

void SimpleMonthView::OnDraw(
    /* [in] */ ICanvas* canvas)
{
    // ==================before translated======================
    // drawMonthTitle(canvas);
    // drawWeekDayLabels(canvas);
    // drawDays(canvas);

    DrawMonthTitle(canvas);
    DrawWeekDayLabels(canvas);
    DrawDays(canvas);
}

void SimpleMonthView::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    // ==================before translated======================
    // setMeasuredDimension(MeasureSpec.getSize(widthMeasureSpec), mRowHeight * mNumRows
    //         + mMonthHeaderSize);

    SetMeasuredDimension(MeasureSpec::GetSize(widthMeasureSpec), mRowHeight * mNumRows + mMonthHeaderSize);
}

void SimpleMonthView::OnSizeChanged(
    /* [in] */ Int32 w,
    /* [in] */ Int32 h,
    /* [in] */ Int32 oldw,
    /* [in] */ Int32 oldh)
{
    // ==================before translated======================
    // mWidth = w;
    //
    // // Invalidate cached accessibility information.
    // mTouchHelper.invalidateRoot();

    mWidth = w;
    mTouchHelper->InvalidateRoot();
}

ECode SimpleMonthView::InitView()
{
    // ==================before translated======================
    // mMonthTitlePaint = new Paint();
    // mMonthTitlePaint.setAntiAlias(true);
    // mMonthTitlePaint.setColor(mNormalTextColor);
    // mMonthTitlePaint.setTextSize(mMonthLabelTextSize);
    // mMonthTitlePaint.setTypeface(Typeface.create(mMonthTitleTypeface, Typeface.BOLD));
    // mMonthTitlePaint.setTextAlign(Align.CENTER);
    // mMonthTitlePaint.setStyle(Style.FILL);
    // mMonthTitlePaint.setFakeBoldText(true);
    //
    // mMonthDayLabelPaint = new Paint();
    // mMonthDayLabelPaint.setAntiAlias(true);
    // mMonthDayLabelPaint.setColor(mNormalTextColor);
    // mMonthDayLabelPaint.setTextSize(mMonthDayLabelTextSize);
    // mMonthDayLabelPaint.setTypeface(Typeface.create(mDayOfWeekTypeface, Typeface.NORMAL));
    // mMonthDayLabelPaint.setTextAlign(Align.CENTER);
    // mMonthDayLabelPaint.setStyle(Style.FILL);
    // mMonthDayLabelPaint.setFakeBoldText(true);
    //
    // mDayNumberSelectedPaint = new Paint();
    // mDayNumberSelectedPaint.setAntiAlias(true);
    // mDayNumberSelectedPaint.setColor(mSelectedDayColor);
    // mDayNumberSelectedPaint.setAlpha(SELECTED_CIRCLE_ALPHA);
    // mDayNumberSelectedPaint.setTextAlign(Align.CENTER);
    // mDayNumberSelectedPaint.setStyle(Style.FILL);
    // mDayNumberSelectedPaint.setFakeBoldText(true);
    //
    // mDayNumberPaint = new Paint();
    // mDayNumberPaint.setAntiAlias(true);
    // mDayNumberPaint.setTextSize(mMiniDayNumberTextSize);
    // mDayNumberPaint.setTextAlign(Align.CENTER);
    // mDayNumberPaint.setStyle(Style.FILL);
    // mDayNumberPaint.setFakeBoldText(false);
    //
    // mDayNumberDisabledPaint = new Paint();
    // mDayNumberDisabledPaint.setAntiAlias(true);
    // mDayNumberDisabledPaint.setColor(mDisabledTextColor);
    // mDayNumberDisabledPaint.setTextSize(mMiniDayNumberTextSize);
    // mDayNumberDisabledPaint.setTextAlign(Align.CENTER);
    // mDayNumberDisabledPaint.setStyle(Style.FILL);
    // mDayNumberDisabledPaint.setFakeBoldText(false);

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
    // ==================before translated======================
    // return day >= Calendar.SUNDAY && day <= Calendar.SATURDAY;

    return (day >= ICalendar::SUNDAY && day <= ICalendar::SATURDAY);
}

Boolean SimpleMonthView::IsValidMonth(
    /* [in] */ Int32 month)
{
    // ==================before translated======================
    // return month >= Calendar.JANUARY && month <= Calendar.DECEMBER;

    return (month >= ICalendar::JANUARY && month <= ICalendar::DECEMBER);
}

Int32 SimpleMonthView::GetDaysInMonth(
    /* [in] */ Int32 month,
    /* [in] */ Int32 year)
{
    // ==================before translated======================
    // switch (month) {
    //     case Calendar.JANUARY:
    //     case Calendar.MARCH:
    //     case Calendar.MAY:
    //     case Calendar.JULY:
    //     case Calendar.AUGUST:
    //     case Calendar.OCTOBER:
    //     case Calendar.DECEMBER:
    //         return 31;
    //     case Calendar.APRIL:
    //     case Calendar.JUNE:
    //     case Calendar.SEPTEMBER:
    //     case Calendar.NOVEMBER:
    //         return 30;
    //     case Calendar.FEBRUARY:
    //         return (year % 4 == 0) ? 29 : 28;
    //     default:
    //         throw new IllegalArgumentException("Invalid Month");
    // }

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
    // ==================before translated======================
    // int offset = findDayOffset();
    // int dividend = (offset + mNumCells) / mNumDays;
    // int remainder = (offset + mNumCells) % mNumDays;
    // return (dividend + (remainder > 0 ? 1 : 0));

    Int32 offset = FindDayOffset();
    Int32 dividend = (offset + mNumCells) / mNumDays;
    Int32 remainder = (offset + mNumCells) % mNumDays;
    return (dividend + (remainder > 0 ? 1 : 0));
}

Boolean SimpleMonthView::SameDay(
    /* [in] */ Int32 day,
    /* [in] */ ITime* today)
{
    // ==================before translated======================
    // return mYear == today.year &&
    //         mMonth == today.month &&
    //         day == today.monthDay;

    Int32 year, month, monthDay;
    today->GetYear(&year);
    today->GetMonth(&month);
    today->GetMonthDay(&monthDay);
    return (mYear == year && mMonth == month && day == monthDay);
}

String SimpleMonthView::GetMonthAndYearString()
{
    // ==================before translated======================
    // int flags = DateUtils.FORMAT_SHOW_DATE | DateUtils.FORMAT_SHOW_YEAR
    //         | DateUtils.FORMAT_NO_MONTH_DAY;
    // mStringBuilder.setLength(0);
    // long millis = mCalendar.getTimeInMillis();
    // return DateUtils.formatDateRange(getContext(), mFormatter, millis, millis, flags,
    //         Time.getCurrentTimezone()).toString();

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
    // ==================before translated======================
    // final float x = (mWidth + 2 * mPadding) / 2f;
    // final float y = (mMonthHeaderSize - mMonthDayLabelTextSize) / 2f;
    // canvas.drawText(getMonthAndYearString(), x, y, mMonthTitlePaint);

    const Float x = (mWidth + 2 * mPadding) / 2.0f;
    const Float y = (mMonthHeaderSize - mMonthDayLabelTextSize) / 2.0f;
    canvas->DrawText(GetMonthAndYearString(), x, y, mMonthTitlePaint);
}

void SimpleMonthView::DrawWeekDayLabels(
    /* [in] */ ICanvas* canvas)
{
    assert(canvas);
    // ==================before translated======================
    // final int y = mMonthHeaderSize - (mMonthDayLabelTextSize / 2);
    // final int dayWidthHalf = (mWidth - mPadding * 2) / (mNumDays * 2);
    //
    // for (int i = 0; i < mNumDays; i++) {
    //     final int calendarDay = (i + mWeekStart) % mNumDays;
    //     mDayLabelCalendar.set(Calendar.DAY_OF_WEEK, calendarDay);
    //
    //     final String dayLabel = mDayFormatter.format(mDayLabelCalendar.getTime());
    //     final int x = (2 * i + 1) * dayWidthHalf + mPadding;
    //     canvas.drawText(dayLabel, x, y, mMonthDayLabelPaint);
    // }

    const Int32 y = mMonthHeaderSize - (mMonthDayLabelTextSize / 2);
    const Int32 dayWidthHalf = (mWidth - mPadding * 2) / (mNumDays * 2);
    String dayLabel;
    Int32 x, calendarDay;
    Elastos::Text::IDateFormat* dateFormat = Elastos::Text::IDateFormat::Probe(mDayFormatter);
    AutoPtr<IDate> date;
    mDayLabelCalendar->GetTime((IDate**)&date);
    for (Int32 i = 0; i < mNumDays; i++) {
        calendarDay = (i + mWeekStart) % mNumDays;
        mDayLabelCalendar->Set(ICalendar::DAY_OF_WEEK, calendarDay);

        dateFormat->Format(date, &dayLabel);
        x = (2 * i + 1) * dayWidthHalf + mPadding;
        canvas->DrawText(dayLabel, x, y, mMonthDayLabelPaint);
    }
}

void SimpleMonthView::DrawDays(
    /* [in] */ ICanvas* canvas)
{
    assert(canvas);
    // ==================before translated======================
    // int y = (((mRowHeight + mMiniDayNumberTextSize) / 2) - DAY_SEPARATOR_WIDTH)
    //         + mMonthHeaderSize;
    // int dayWidthHalf = (mWidth - mPadding * 2) / (mNumDays * 2);
    // int j = findDayOffset();
    // for (int day = 1; day <= mNumCells; day++) {
    //     int x = (2 * j + 1) * dayWidthHalf + mPadding;
    //     if (mSelectedDay == day) {
    //         canvas.drawCircle(x, y - (mMiniDayNumberTextSize / 3), mDaySelectedCircleSize,
    //                 mDayNumberSelectedPaint);
    //     }
    //
    //     if (mHasToday && mToday == day) {
    //         mDayNumberPaint.setColor(mSelectedDayColor);
    //     } else {
    //         mDayNumberPaint.setColor(mNormalTextColor);
    //     }
    //     final Paint paint = (day < mEnabledDayStart || day > mEnabledDayEnd) ?
    //             mDayNumberDisabledPaint : mDayNumberPaint;
    //     canvas.drawText(String.format("%d", day), x, y, paint);
    //     j++;
    //     if (j == mNumDays) {
    //         j = 0;
    //         y += mRowHeight;
    //     }
    // }

    Int32 y = (((mRowHeight + mMiniDayNumberTextSize) / 2) - DAY_SEPARATOR_WIDTH) + mMonthHeaderSize;
    Int32 dayWidthHalf = (mWidth - mPadding * 2) / (mNumDays * 2);
    Int32 j = FindDayOffset();
    Int32 x = 0;
    String sDay;

    for (Int32 day = 1; day <= mNumCells; ++day) {
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

        sDay = StringUtils::ToString(day);
        IPaint* paint = (day < mEnabledDayStart || day > mEnabledDayEnd) ? mDayNumberDisabledPaint : mDayNumberPaint;
        canvas->DrawText(sDay, x, y, paint);
        ++j;
        if (j == mNumDays) {
            j = 0;
            y += mRowHeight;
        }
    }
}

Int32 SimpleMonthView::FindDayOffset()
{
    // ==================before translated======================
    // return (mDayOfWeekStart < mWeekStart ? (mDayOfWeekStart + mNumDays) : mDayOfWeekStart)
    //         - mWeekStart;

    return (mDayOfWeekStart < mWeekStart ? (mDayOfWeekStart + mNumDays) : mDayOfWeekStart) - mWeekStart;
}

Int32 SimpleMonthView::GetDayFromLocation(
    /* [in] */ Float x,
    /* [in] */ Float y)
{
    // ==================before translated======================
    // int dayStart = mPadding;
    // if (x < dayStart || x > mWidth - mPadding) {
    //     return -1;
    // }
    // // Selection is (x - start) / (pixels/day) == (x -s) * day / pixels
    // int row = (int) (y - mMonthHeaderSize) / mRowHeight;
    // int column = (int) ((x - dayStart) * mNumDays / (mWidth - dayStart - mPadding));
    //
    // int day = column - findDayOffset() + 1;
    // day += row * mNumDays;
    // if (day < 1 || day > mNumCells) {
    //     return -1;
    // }
    // return day;

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
    // ==================before translated======================
    // if (mOnDayClickListener != null) {
    //     Calendar date = Calendar.getInstance();
    //     date.set(mYear, mMonth, day);
    //     mOnDayClickListener.onDayClick(this, date);
    // }
    //
    // // This is a no-op if accessibility is turned off.
    // mTouchHelper.sendEventForVirtualView(day, AccessibilityEvent.TYPE_VIEW_CLICKED);

    if (mOnDayClickListener != NULL) {
        AutoPtr<ICalendarHelper> helper;
        CCalendarHelper::AcquireSingleton((ICalendarHelper**)&helper);
        AutoPtr<ICalendar> date;
        helper->GetInstance((ICalendar**)&date);
        date->Set(mYear, mMonth, day);
        mOnDayClickListener->OnDayClick(this, date);
    }

    // This is a no-op if accessibility is turned off.
    // car need contain this func: mTouchHelper->SendEventForVirtualView(day, IAccessibilityEvent::TYPE_VIEW_CLICKED);
}

} // namespace Widget
} // namespace Droid
} // namespace Elastos


