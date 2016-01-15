#ifndef __ELASTOS_DROID_WIDGET_YEARPICKERVIEW_H__
#define __ELASTOS_DROID_WIDGET_YEARPICKERVIEW_H__

#include "elastos/droid/widget/ListView.h"
#include "elastos/droid/widget/ArrayAdapter.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Widget::ListView;
using Elastos::Droid::Widget::ArrayAdapter;

namespace Elastos {
namespace Droid {
namespace Widget{

class YearPickerView
    : public ListView
    , public IYearPickerView
    , public IAdapterViewOnItemClickListener
    , public IOnDateChangedListener
{
private:
    class YearAdapter
        : public ArrayAdapter
    {
    public:
        YearAdapter();

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Int32 resource);

        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        CARAPI SetItemTextAppearance(
            /* [in] */ Int32 resId);

    private:
        Int32 mItemTextAppearanceResId;
    };

public:
    CAR_INTERFACE_DECL()

    YearPickerView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs = NULL,
        /* [in] */ Int32 defStyleAttr = R::attr::listViewStyle,
        /* [in] */ Int32 defStyleRes = 0);

    CARAPI Init(
        /* [in] */ IDatePickerController* controller);

    CARAPI SetYearSelectedCircleColor(
        /* [in] */ Int32 color);

    CARAPI GetYearSelectedCircleColor(
        /* [out] */ Int32* color);

    CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    //package
    CARAPI SetItemTextAppearance(
        /* [in] */ Int32 resId);

    CARAPI PostSetSelectionCentered(
        /* [in] */ Int32 position);

    CARAPI PostSetSelectionFromTop(
        /* [in] */ Int32 position,
        /* [in] */ Int32 offset);

    CARAPI GetFirstPositionOffset(
        /* [out] */ Int32* offset);

    CARAPI OnDateChanged();

    CARAPI OnInitializeAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event);

private:
    CARAPI_(void) UpdateAdapterData();

private:
    AutoPtr<IDatePickerController> mController;
    AutoPtr<YearAdapter> mAdapter;
    Int32 mViewSize;
    Int32 mChildSize;
    Int32 mSelectedPosition;
    Int32 mYearSelectedCircleColor;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_YEARPICKERVIEW_H__