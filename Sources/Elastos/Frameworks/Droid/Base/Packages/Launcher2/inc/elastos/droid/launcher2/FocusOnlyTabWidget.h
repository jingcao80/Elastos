#ifndef  __ELASTOS_DROID_LAUNCHER2_FOCUSONLYTABWIDGET_H__
#define  __ELASTOS_DROID_LAUNCHER2_FOCUSONLYTABWIDGET_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class FocusOnlyTabWidget
    : public TabWidget
    , public IFocusOnlyTabWidget
{
public:
    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

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
    CARAPI OnFocusChange(
        /* [in] */ IView* v,
        /* [in] */ Boolean hasFocus);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_FOCUSONLYTABWIDGET_H__