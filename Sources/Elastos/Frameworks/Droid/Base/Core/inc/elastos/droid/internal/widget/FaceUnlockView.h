
#ifndef __ELASTOS_DROID_INTERNAL_WIDGET_FACEUNLOCKVIEW_H__
#define __ELASTOS_DROID_INTERNAL_WIDGET_FACEUNLOCKVIEW_H__

#include "elastos/droid/widget/RelativeLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Widget::RelativeLayout;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Widget {

class FaceUnlockView
    : public RelativeLayout
    , public IFaceUnlockView
{
public:
    CAR_INTERFACE_DECL()

    FaceUnlockView();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

protected:
    virtual CARAPI_(void) OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    CARAPI_(Int32) ResolveMeasured(
        /* [in] */ Int32 measureSpec,
        /* [in] */ Int32 desired);

private:
    static const String FaceUnlockView_TAG;
};

} // namespace Widget
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_WIDGET_FACEUNLOCKVIEW_H__
