
#ifndef __ELASTOS_DROID_WIDGET_SIMPLEMONTHADAPTER_H__
#define __ELASTOS_DROID_WIDGET_SIMPLEMONTHADAPTER_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "elastos/droid/widget/BaseAdapter.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IColorStateList;
using Elastos::Utility::ICalendar;

namespace Elastos {
namespace Droid {
namespace Widget {

/**
 * An adapter for a list of {@link android.widget.SimpleMonthView} items.
 */
class SimpleMonthAdapter
    : public BaseAdapter
    , public ISimpleMonthAdapter
    , public IOnDayClickListener
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IDatePickerController* controller);

    /**
     * Updates the selected day and related parameters.
     *
     * @param day The day to highlight
     */
    CARAPI SetSelectedDay(
        /* [in] */ ICalendar* day);

    CARAPI SetCalendarTextColor(
        /* [in] */ IColorStateList* colors);

    // @Override
    CARAPI GetCount(
        /* [out] */ Int32* count);

    // @Override
    CARAPI GetItem(
        /* [in] */ Int32 position,
        /* [out] */ IInterface** item);

    // @Override
    CARAPI GetItemId(
        /* [in] */ Int32 position,
        /* [out] */ Int64* id);

    // @Override
    CARAPI HasStableIds(
        /* [out] */ Boolean* has);

    // @SuppressWarnings("unchecked")
    // @Override
    CARAPI GetView(
        /* [in] */ Int32 position,
        /* [in] */ IView* convertView,
        /* [in] */ IViewGroup* parent,
        /* [out] */ IView** view);

    // @Override
    CARAPI OnDayClick(
        /* [in] */ ISimpleMonthView* view,
        /* [in] */ ICalendar* day);

protected:
    /**
     * Set up the gesture detector and selected time
     */
    virtual CARAPI_(void) Init();

    /**
     * Maintains the same hour/min/sec but moves the day to the tapped day.
     *
     * @param day The day that was tapped
     */
    virtual CARAPI_(void) OnDayTapped(
        /* [in] */ ICalendar* day);

private:
    CARAPI_(Boolean) IsSelectedDayInMonth(
        /* [in] */ Int32 year,
        /* [in] */ Int32 month);

private:
    static const String TAG;

    AutoPtr<IContext> mContext;
    AutoPtr<IDatePickerController> mController;
    AutoPtr<ICalendar> mSelectedDay;

    AutoPtr<IColorStateList> mCalendarTextColors;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WIDGET_SIMPLEMONTHADAPTER_H__
