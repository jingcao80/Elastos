
#ifndef __ELASTOS_DROID_SETTINGS_DASHBOARD_DASHBOARDTILEVIEW_H__
#define __ELASTOS_DROID_SETTINGS_DASHBOARD_DASHBOARDTILEVIEW_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/FrameLayout.h"
#include "elastos/droid/settings/dashboard/CDashboardTile.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::FrameLayout;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

class DashboardTileView
    : public FrameLayout
{
public:
    class InnerListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("DashboardTileView::InnerListener")

        InnerListener(
            /* [in] */ DashboardTileView* host);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        DashboardTileView* mHost;
    };

public:
    DashboardTileView();

    ~DashboardTileView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI_(AutoPtr<ITextView>) GetTitleTextView();

    CARAPI_(AutoPtr<ITextView>) GetStatusTextView();

    CARAPI_(AutoPtr<IImageView>) GetImageView();

    CARAPI SetTile(
        /* [in] */ CDashboardTile* tile);

    CARAPI SetDividerVisibility(
        /* [in] */ Boolean visible);

    CARAPI_(void) SetColumnSpan(
        /* [in] */ Int32 span);

    CARAPI_(Int32) GetColumnSpan();

    //@Override
    CARAPI OnClick(
        /* [in] */ IView* v);

private:
    static const Int32 DEFAULT_COL_SPAN;

    AutoPtr<IImageView> mImageView;
    AutoPtr<ITextView> mTitleTextView;
    AutoPtr<ITextView> mStatusTextView;
    AutoPtr<IView> mDivider;

    Int32 mColSpan;

    AutoPtr<CDashboardTile> mTile;
};

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DASHBOARD_DASHBOARDTILEVIEW_H__
