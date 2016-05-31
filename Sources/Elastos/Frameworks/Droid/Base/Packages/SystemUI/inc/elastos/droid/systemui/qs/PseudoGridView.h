
#ifndef __ELASTOS_DROID_SYSTEMUI_QS_PSEUDOGRIDVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_PSEUDOGRIDVIEW_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/database/DataSetObserver.h>
#include <elastos/droid/view/ViewGroup.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Database::DataSetObserver;
using Elastos::Droid::View::ViewGroup;
using Elastos::Droid::Widget::IBaseAdapter;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {

/**
 * A view that arranges it's children in a grid with a fixed number of evenly spaced columns.
 *
 * {@see android.widget.GridView}
 */
class PseudoGridView
    : public ViewGroup
    , public IPseudoGridView
{
public:
    /**
     * Bridges between a ViewGroup and a BaseAdapter.
     * <p>
     * Usage: {@code ViewGroupAdapterBridge.link(viewGroup, adapter)}
     * <br />
     * After this call, the ViewGroup's children will be provided by the adapter.
     */
    class ViewGroupAdapterBridge: public DataSetObserver
    {
    public:
        ViewGroupAdapterBridge();

        ~ViewGroupAdapterBridge();

        CARAPI constructor(
            /* [in] */ IViewGroup* viewGroup,
            /* [in] */ IBaseAdapter* adapter);

        static CARAPI_(void) Link(
            /* [in] */ IViewGroup* viewGroup,
            /* [in] */ IBaseAdapter* adapter);

        // @Override
        CARAPI OnChanged();

        // @Override
        CARAPI OnInvalidated();

    private:

        CARAPI_(void) Refresh();

        CARAPI_(void) ReleaseAdapter();

    private:
        AutoPtr<IWeakReference> mViewGroup;  /*<ViewGroup*/
        AutoPtr<IBaseAdapter> mAdapter;
        Boolean mReleased;
    };

public:
    CAR_INTERFACE_DECL()

    PseudoGridView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    // @Override
    CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    // @Override
    CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

private:
    Int32 mNumColumns;
    Int32 mVerticalSpacing;
    Int32 mHorizontalSpacing;
};

} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_PSEUDOGRIDVIEW_H__
