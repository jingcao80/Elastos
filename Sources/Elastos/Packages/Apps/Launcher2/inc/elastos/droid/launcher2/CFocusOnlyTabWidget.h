#ifndef  __ELASTOS_DROID_LAUNCHER2_CFOCUSONLYTABWIDGET_H__
#define  __ELASTOS_DROID_LAUNCHER2_CFOCUSONLYTABWIDGET_H__

#include "_Elastos_Droid_Launcher2_CFocusOnlyTabWidget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/TabWidget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::TabWidget;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CFocusOnlyTabWidget)
    , public TabWidget
    , public IFocusOnlyTabWidget
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI GetSelectedTab(
        /* [out] */ IView** tab);

    CARAPI GetChildTabIndex(
        /* [in] */ IView* v,
        /* [out] */ Int32* index);

    CARAPI SetCurrentTabToFocusedTab();

    CARAPI SuperOnFocusChange(
        /* [in] */ IView* v,
        /* [in] */ Boolean hasFocus);

    //@Override
    virtual CARAPI OnFocusChange(
        /* [in] */ IView* v,
        /* [in] */ Boolean hasFocus);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CFOCUSONLYTABWIDGET_H__