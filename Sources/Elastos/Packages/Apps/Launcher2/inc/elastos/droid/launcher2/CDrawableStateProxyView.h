#ifndef  __ELASTOS_DROID_LAUNCHER2_CDRAWABLESATTEPROXYVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_CDRAWABLESATTEPROXYVIEW_H__

#include "_Elastos_Droid_Launcher2_CDrawableStateProxyView.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CDrawableStateProxyView)
    , public LinearLayout
{
public:
    CAR_OBJECT_DECL();

    CDrawableStateProxyView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);


    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyle);

    //@Override
    CARAPI OnHoverEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI DrawableStateChanged();

private:
    AutoPtr<IView> mView;
    Int32 mViewId;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CDRAWABLESATTEPROXYVIEW_H__