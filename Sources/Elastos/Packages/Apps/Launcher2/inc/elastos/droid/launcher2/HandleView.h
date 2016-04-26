#ifndef  __ELASTOS_DROID_LAUNCHER2_HANDLEVIEW_H__
#define  __ELASTOS_DROID_LAUNCHER2_HANDLEVIEW_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/ImageView.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::ImageView;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class HandleView
    : public ImageView
{
public:
    HandleView();

    virtual ~HandleView() {}

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
    CARAPI FocusSearch(
        /* [in] */ Int32 direction,
        /* [out] */ IView** view);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,
        /* [out] */ Boolean* result);

    CARAPI SetLauncher(
        /* [in] */ ILauncher* launcher);

private:
    static const Int32 ORIENTATION_HORIZONTAL;

    AutoPtr<ILauncher> mLauncher;
    Int32 mOrientation;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_HANDLEVIEW_H__