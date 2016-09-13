#ifndef __ELASTOS_DROID_WIDGET_YEARPICKERVIEW_H__
#define __ELASTOS_DROID_WIDGET_YEARPICKERVIEW_H__

#include "elastos/droid/widget/ListView.h"
#include "elastos/droid/widget/ArrayAdapter.h"

using Elastos::Droid::Widget::ListView;
using Elastos::Droid::Widget::ArrayAdapter;

namespace Elastos {
namespace Droid {
namespace Widget{

class YearPickerView
    : public ListView
    , public IYearPickerView
{
private:
    class InnerListener
        : public Object
        , public IAdapterViewOnItemClickListener
        , public IOnDateChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ YearPickerView* host);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

        CARAPI OnDateChanged();

    private:
        YearPickerView* mHost;
    };

    class YearAdapter
        : public ArrayAdapter
    {
    public:
        YearAdapter();

        CARAPI constructor(
            /* [in] */ YearPickerView* host,
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
        YearPickerView* mHost;
        Int32 mItemTextAppearanceResId;
    };

    class YearRunnable
        : public Runnable
    {
    public:
        YearRunnable(
            /* [in] */ YearPickerView* host,
            /* [in] */ Int32 position,
            /* [in] */ Int32 offset);

        ~YearRunnable();

        // @Override
        CARAPI Run();

    private:
        YearPickerView* mHost;
        Int32 mPosition;
        Int32 mOffset;
    };

public:
    CAR_INTERFACE_DECL()

    YearPickerView();

    ~YearPickerView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

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
    AutoPtr<InnerListener> mListener;
};

} // namespace Widget
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WIDGET_YEARPICKERVIEW_H__