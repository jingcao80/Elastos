
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_LINEARLAYOUTWITHDEFAULTTOUCHRECEPIENT_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_LINEARLAYOUTWITHDEFAULTTOUCHRECEPIENT_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Internal::Widget::ILinearLayoutWithDefaultTouchRecepient;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class LinearLayoutWithDefaultTouchRecepient
    : public LinearLayout
    , public ILinearLayoutWithDefaultTouchRecepient
{
public:
    CAR_INTERFACE_DECL()

    LinearLayoutWithDefaultTouchRecepient();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    virtual CARAPI SetDefaultTouchRecepient(
        /* [in] */ IView* defaultTouchRecepient);

    // @Override
    CARAPI DispatchTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

private:
    AutoPtr<IRect> mTempRect;
    AutoPtr<IView> mDefaultTouchRecepient;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_LINEARLAYOUTWITHDEFAULTTOUCHRECEPIENT_H__
