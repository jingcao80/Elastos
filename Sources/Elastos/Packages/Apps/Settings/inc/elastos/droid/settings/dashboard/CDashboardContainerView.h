#ifndef __ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDCONTAINERVIEW_H__
#define __ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDCONTAINERVIEW_H__

#include "elastos/droid/view/ViewGroup.h"
#include "_Elastos_Droid_Settings_Dashboard_CDashboardContainerView.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ViewGroup;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Dashboard {

CarClass(CDashboardContainerView) , public ViewGroup
{
public:
    CAR_OBJECT_DECL()

    CDashboardContainerView();

    ~CDashboardContainerView();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    //@Override
    virtual CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

    //@Override
    virtual CARAPI OnLayout(
        /* [in] */ Boolean changed,
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 r,
        /* [in] */ Int32 b);

private:
    Float mCellGapX;
    Float mCellGapY;

    Int32 mNumRows;
    Int32 mNumColumns;
};

} // namespace Dashboard
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_DASHBOARD_CDASHBOARDCONTAINERVIEW_H__
