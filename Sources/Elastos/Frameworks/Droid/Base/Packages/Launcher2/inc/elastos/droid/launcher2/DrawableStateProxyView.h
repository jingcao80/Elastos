#ifndef  __ELASTOS_DROID_LAUNCHER2_DRAWABLESATTEPROXYVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_DRAWABLESATTEPROXYVIEW_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class DrawableStateProxyView
    : public LinearLayout
    , public IDrawableStateProxyView
{
public:
    CAR_INTERFACE_DECL();

    DrawableStateProxyView();

    CARAPI DrawableStateProxyView(
        /* [in] */ IContext* context);

    CARAPI DrawableStateProxyView(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);


    CARAPI DrawableStateProxyView(
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

#endif // __ELASTOS_DROID_LAUNCHER2_DRAWABLESATTEPROXYVIEW_H__