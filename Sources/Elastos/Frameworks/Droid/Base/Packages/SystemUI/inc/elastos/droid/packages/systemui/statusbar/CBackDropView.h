
#ifndef  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CBACKDROPVIEW_H__
#define  __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CBACKDROPVIEW_H__

#include "_Elastos_Droid_Packages_SystemUI_StatusBar_CBackDropView.h"
#include <elastos/droid/widget/FrameLayout.h>

using Elastos::Droid::Widget::FrameLayout;

namespace Elastos {
namespace Droid {
namespace Packages {
namespace SystemUI {
namespace StatusBar {

/**
 * A view who contains media artwork.
 */
CarClass(CBackDropView)
    , public FrameLayout
    , public IBackDropView
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

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

    // @Override
    CARAPI HasOverlappingRendering(
        /* [out] */ Boolean* result);

    CARAPI SetOnVisibilityChangedRunnable(
        /* [in] */ IRunnable* runnable);

protected:
    // @Override
    CARAPI OnVisibilityChanged(
        /* [in] */ IView* changedView,
        /* [in] */ Int32 visibility);

private:
    AutoPtr<IRunnable> mOnVisibilityChangedRunnable;
};

} // namespace StatusBar
} // namespace SystemUI
} // namespace Packages
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PACKAGES_SYSTEMUI_STATUSBAR_CBACKDROPVIEW_H__
